// P2PApplication behaviour (where we will be basically building the gnutella like behaviour)
// https://www.sfu.ca/~ljilja/ENSC427/Spring13/Projects/team6/Ensc427_final_project_report.pdf
// http://computer.howstuffworks.com/file-sharing1.htm
// http://www2.ensc.sfu.ca/~ljilja/ENSC427/Spring10/Projects/team7/final_report_Gnutella.pdf
#include "p2p-application.h"

#include "p2p-packet.h"
#include "p2p-util.h"

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4.h"
#include "ns3/node-list.h"
#include "ns3/packet.h"
#include "ns3/udp-socket.h"

#include <unordered_set>
#include <vector>

// for adding routing for backwards forwarding on qh
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-static-routing.h"

// global variables
#define DEFAULT_TTL 999
#define DEFALT_PORT 5000

// logging/debugging purposes
NS_LOG_COMPONENT_DEFINE("P2PApplication");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(P2PApplication);

P2PApplication::P2PApplication()
    : m_port(DEFALT_PORT)
{
}

P2PApplication::P2PApplication(uint16_t port)
    : m_port(port)
{
} // Custom constructor NEED TO LOOK INTO THIS WARNINGS

TypeId
P2PApplication::GetTypeId()
{
    static TypeId tid = TypeId("ns3::P2PApplication")
                            .SetParent<Application>()
                            .SetGroupName("Tutorial")
                            .AddConstructor<P2PApplication>();
    return tid;
}

/**
 * Starts the application by binding each IP with a port to send and receive data.
 */
void
P2PApplication::StartApplication()
{
    Ptr<Node> node = GetNode();
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();

    // first interface is a loopback one. Avoid it.
    for (uint32_t i = 1; i < ipv4->GetNInterfaces(); i++)
    {
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        Ptr<Socket> socket = Socket::CreateSocket(GetNode(), tid);
        Ipv4Address address = ipv4->GetAddress(i, 0).GetLocal();

        if (address != Ipv4Address::GetZero())
        { // Only bind to valid addresses
            InetSocketAddress local = InetSocketAddress(address, m_port);
            socket->Bind(local);
            socket->SetRecvCallback(MakeCallback(&P2PApplication::RecievePacket, this));
            m_sockets.push_back(socket);
        }
    }

    // debug
    // PrintSocketBindings();
}

/**
 * Stops the application by closing all sockets.
 */
void
P2PApplication::StopApplication()
{
    for (Ptr<Socket> m_socket : m_sockets)
    {
        if (m_socket)
        {
            m_socket->Close();
            m_socket = nullptr;
        }
    }
}

// ----------------------------------------------------------------------------
//                                FORWARDING QUERY HIT
// --------------------------------------------------------------------------

/**
 * logic for backward forwarding when there is a query hit
 */
void
P2PApplication::ForwardQueryHit(P2PPacket ppacket, int lastHopIndex)
{
    Ipv4Address lastHopIPV4 = m_neighbours[lastHopIndex];
    Ipv4Address curIPV4 = m_ipv4Addresses[lastHopIndex];

    // set te destination to the last hop and forward the packet to it
    ppacket.SetMessageType(QUERY_HIT);
    ppacket.SetDestIp(lastHopIPV4);
    ppacket.RemoveLastHop();
    // so right now each time it forward this it will increase the ttl, not sure if this is a good
    // idea right now
    ppacket.SetTtl(ppacket.GetTtl() + 2);

    Ptr<Packet> newPacket = Create<Packet>();
    newPacket->AddHeader(ppacket);
    // forward the packet to the previous hop
    NS_LOG_INFO("FQH from " << curIPV4 << " back to " << lastHopIPV4 << " type "
                            << ppacket.GetMessageType() << " with ttl " << (int)ppacket.GetTtl());
    m_sockets[lastHopIndex]->SendTo(newPacket, 0, InetSocketAddress(lastHopIPV4, m_port));
}

// ----------------------------------------------------------------------------
//                                SENDING QUERIES
// ----------------------------------------------------------------------------

/**
 * sends the first packet from src. Call from Initial Flood, Initial Random Walk or Initial
 * Normalized Flood
 */
void
P2PApplication::SendPacketFromSrc(MessageType type,
                                  Ipv4Address dest,
                                  uint8_t ttl,
                                  Ipv4Address curIP,
                                  uint32_t sinkn,
                                  int neighbourIndex)
{
    Ptr<Packet> packet = Create<Packet>();
    uint32_t msgid = 0;
    // start hop at 0, and path at empty
    P2PPacket p2pPacket(type, msgid, curIP, dest, ttl, 0, sinkn, {});
    p2pPacket.AddToPath(curIP);
    packet->AddHeader(p2pPacket);

    // Send packet over the socket (how ns3 sends stuff)
    m_sockets[neighbourIndex]->SendTo(packet, 0, InetSocketAddress(dest, m_port));
    NS_LOG_INFO("Sent " << type << " packet to " << dest << " from " << curIP);
}

// ----------------------------------------------------------------------------
//                                FLOODING
// ----------------------------------------------------------------------------

/**
 * first flooding from src
 */
void
P2PApplication::InitialFlood(uint32_t sinknode)
{
    for (int i = 0; i < m_ipv4Addresses.size(); ++i)
    {
        Ipv4Address curIPV4 = m_ipv4Addresses[i];
        Ipv4Address curNeighbor = m_neighbours[i];

        SendPacketFromSrc(QUERY, curNeighbor, DEFAULT_TTL, curIPV4, sinknode, i);
    }
}

/**
 * Handles Flooding in intermediate nodes.
 * Idea is that it should the query to all it's neighbours except for the sender
 *
 * @param excludeIndex - is the index of a neighbour and ipv4address that request came from
 */
void
P2PApplication::FloodExceptSender(P2PPacket p2pPacket, int excludeIndex)
{
    for (int i = 0; i < m_ipv4Addresses.size(); ++i)
    {
        // skip the ipv4 the request came from!
        if (i == excludeIndex)
        {
            continue;
        }

        Ipv4Address curIPV4 = m_ipv4Addresses[i];
        Ipv4Address curNeighbor = m_neighbours[i];

        if (curNeighbor != p2pPacket.GetSenderIp())
        {
            // Create a copy of the packet before modifying
            P2PPacket packetCopy = p2pPacket;
            packetCopy.AddToPath(curIPV4);

            Ptr<Packet> newPacket = Create<Packet>();
            newPacket->AddHeader(packetCopy); // Attach the updated copy

            NS_LOG_INFO("rp: forwarding packet from " << curIPV4 << " to " << curNeighbor);
            packetCopy.PrintPath();
            NS_LOG_INFO("----------\n");

            m_sockets[i]->SendTo(newPacket, 0, InetSocketAddress(curNeighbor, m_port));
        }
    }
}

// ----------------------------------------------------------------------------
//                                NORMALIZED FLOODING
// ----------------------------------------------------------------------------

/**
 * first normalized flooding from src
 */
void
P2PApplication::InitialNormalizedFlood(uint32_t sinknode,
                                       int howManyNodes) // feel free to add anything here
{
}

/**
 * Handles Normalized Flooding in intermediate nodes.
 * Idea is that it should the query to all it's neighbours except for the sender
 *
 * @param excludeIndex - is the index of a neighbour and ipv4address that request came from
 */
void
P2PApplication::NormalizedFloodExceptSender(P2PPacket p2pPacket, int excludeIndex, int howManyNodes)
{
}

// ----------------------------------------------------------------------------
//                                RANDOM WALK
// ----------------------------------------------------------------------------

/**
 * first random walk from src
 */
void
P2PApplication::InitialRandomWalk(uint32_t sinknode) // feel free to add anything here
{
}

/**
 * Handles Random Walk in intermediate nodes.
 * Idea is that it should the query to all it's neighbours except for the sender
 *
 * @param excludeIndex - is the index of a neighbour and ipv4address that request came from
 */
void
P2PApplication::RandomWalkExceptSender(P2PPacket p2pPacket, int excludeIndex)
{
}

// ----------------------------------------------------------------------------
//                                RECEIVING REQUESTS
// --------------------------------------------------------------------------

/**
 * Handles the logic for receiving a packet in the P2P network.
 */
void
P2PApplication::RecievePacket(Ptr<Socket> socket)
{
    // Retrieve the incoming packet from the socket
    Ptr<Packet> packet = socket->Recv();
    P2PPacket p2pPacket;
    packet->RemoveHeader(p2pPacket); // Extract the custom P2P packet header

    uint32_t curNodeId = GetNode()->GetId();

    // Check if the packet has returned to the original sender node
    for (Ipv4Address ipv4 : m_ipv4Addresses)
    {
        if (p2pPacket.GetSenderIp() == ipv4)
        {
            NS_LOG_INFO("\nBACK AT THE SENDER NODE. IP IS " << ipv4 << " | NODE ID IS "
                                                            << curNodeId);
            return; // Stop processing since we don't need to forward it back to the sender
        }
    }

    // Identify the sender node and determine from which neighbor the packet was received
    Ipv4Address senderIP = p2pPacket.GetLastHop();
    int senderIndex = GetNeighbourIndexFromNeighbourIP(senderIP);
    // At this point, 'senderIndex' holds the index of the neighbor that sent the packet

    // Retrieve the IP address of the current node based on senderIndex
    Ipv4Address curIP = m_ipv4Addresses[senderIndex];
    NS_LOG_INFO("\nrp: at " << curIP << " from " << senderIP << " of type "
                            << p2pPacket.GetMessageType());

    // Check if the current node is the intended sink node or if it's a QUERY_HIT packet

    bool isSinkNode = (p2pPacket.GetMessageType() == QUERY && p2pPacket.GetSinkNode() == curNodeId);

    // If the packet is a query that reached the sink node or a query hit, handle it accordingly
    if (isSinkNode || p2pPacket.GetMessageType() == QUERY_HIT)
    {
        if (isSinkNode) // The packet has reached the intended destination for the first time
            NS_LOG_INFO("rp: query hit at " << curNodeId << " !!!!!!");

        ForwardQueryHit(p2pPacket, senderIndex); // Send a response back to the requester
        return;
    }

    // Check if the node has any neighbors left to forward the packet to
    // If the only neighbor is the one that sent the packet, it must be dropped
    if (m_neighbours.size() == 1)
    {
        NS_LOG_INFO("rp: at " << curIP << " from " << senderIP << " of type "
                              << p2pPacket.GetMessageType()
                              << " Dropping the packet since no new neighbours");
        return;
    }

    // Drop the packet if its TTL (Time-To-Live) has already reached zero
    if (p2pPacket.GetTtl() == 0)
    {
        NS_LOG_INFO("TTL 0, dropping packet at " << curIP);
        return;
    }

    // Reduce the TTL before forwarding the packet to prevent infinite loops
    p2pPacket.DecrementTtl();

    // Flood the packet to all neighbors except the one it was received from
    FloodExceptSender(p2pPacket, senderIndex);
}

// ----------------------------------------------------------------------------
//                                UTIL
// ----------------------------------------------------------------------------

/**
 * Retrieves the index of a neighbor given its sender's IP address.
 * Right now we have nodeNeighbours and ipv4Addresses. Each of them are linked together on index i.
 * For Example at index 0 ipv4Addresses[0] is linked to nodeNeighbours[0] so now we can match the
 * addresses and find the index in nodeNeighbours
 */
int
P2PApplication::GetNeighbourIndexFromNeighbourIP(Ipv4Address senderIP)
{
    // Loop through the list of IPv4 addresses
    int index = 0;
    for (Ipv4Address ip : m_neighbours)
    {
        // If the IP matches, return the index
        if (ip == senderIP)
        {
            return index;
        }
        index++;
    }

    // Log if no match was found, SHOULD NOT REACH THIS POINT AT ALL
    NS_LOG_ERROR("ERROR: Could not find a match for sender IP: " << senderIP);
    throw std::runtime_error("ERROR: Sender IP not found in the neighbor list.");
}

// ----------------------------------------------------------------------------
//                                SETTERS
// ----------------------------------------------------------------------------

// after neighbours are set in network sim, init peers as neighbouts
void
P2PApplication::SetPeers(std::vector<Ipv4Address> neighbours)
{
    m_neighbours = neighbours;
}

// sets node's ipv4 addresses. each ipv4 address index i corresponds to nodeNeighbours at i
void
P2PApplication::SetAddresses()
{
    m_ipv4Addresses.clear(); // Clear any previous addresses

    Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();

    for (uint32_t i = 1; i < ipv4->GetNInterfaces(); i++)
    {
        Ipv4Address address = ipv4->GetAddress(i, 0).GetLocal();

        if (address != Ipv4Address::GetZero()) // Avoid empty addresses
        {
            m_ipv4Addresses.push_back(address);
        }
    }
    // DEBUG
    // PrintAddresses();
}

// ----------------------------------------------------------------------------
//                                GETTERS
// ----------------------------------------------------------------------------

const std::vector<Ptr<Socket>>&
P2PApplication::GetSockets() const
{
    return m_sockets;
}

const std::vector<Ipv4Address>&
P2PApplication::GetIpv4Addresses() const
{
    return m_ipv4Addresses;
}

const std::vector<Ipv4Address>&
P2PApplication::GetNeighbours() const
{
    return m_neighbours;
}

// ----------------------------------------------------------------------------
//                                DEBUG FUNCTIONS
// --------------------------------------------------------------------------

void
P2PApplication::LogNodeInfo()
{
    P2PUtil::PrintNodeInfo(this);
}

// prints socket information
void
P2PApplication::LogSocketBindings()
{
    P2PUtil::PrintSocketBindings(this);
}

// prints ipv4 addresses
void
P2PApplication::LogAddresses()
{
    P2PUtil::PrintAddresses(this);
}

// virtual destructor to cklean this (need this to run for the headerhaving ~ declared. missing a
// vtable otherwise)
P2PApplication::~P2PApplication()
{
    StopApplication();
}

} // namespace ns3
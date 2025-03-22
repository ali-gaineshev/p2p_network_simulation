// P2PApplication behaviour (where we will be basically building the gnutella like behaviour)
// https://www.sfu.ca/~ljilja/ENSC427/Spring13/Projects/team6/Ensc427_final_project_report.pdf
// http://computer.howstuffworks.com/file-sharing1.htm
// http://www2.ensc.sfu.ca/~ljilja/ENSC427/Spring10/Projects/team7/final_report_Gnutella.pdf
#include "p2p-application.h"

#include "p2p-packet.h"

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
#define DEFAULT_TTL 5
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

// called on start, binds each ip with a port to recieve and send from
void
P2PApplication::StartApplication()
{
    Ptr<Node> node = GetNode();
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();

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

// on stop, close all sockets
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

// logic for backward forwarding when there is a query hit
void
P2PApplication::ForwardQueryHit(P2PPacket ppacket, int lastHopIndex)
{
    Ipv4Address lastHopIPV4 = m_neighbours[lastHopIndex];
    Ipv4Address curIPV4 = m_ipv4Addresses[lastHopIndex];

    if (ppacket.GetSenderIp() == curIPV4)
    {
        NS_LOG_INFO("back to sender peer " << curIPV4);
        return;
    }
    ppacket.SetMessageType(QUERY_HIT);
    ppacket.SetDestIp(lastHopIPV4);
    ppacket.RemoveLastHop();
    ppacket.SetTtl(ppacket.GetTtl() + 2);

    Ptr<Packet> newPacket = Create<Packet>();
    newPacket->AddHeader(ppacket);
    // forward the packet to the previous hop
    NS_LOG_INFO("FQH from " << curIPV4 << " back to " << lastHopIPV4 << " type "
                            << ppacket.GetMessageType() << " with ttl " << (int)ppacket.GetTtl());
    m_sockets[lastHopIndex]->SendTo(newPacket, 0, InetSocketAddress(lastHopIPV4, m_port));
}

// packet sends a query to find sink node via FLOOD
void
P2PApplication::Flood(uint32_t sinknode)
{
    for (int i = 0; i < m_ipv4Addresses.size(); ++i)
    {
        Ipv4Address curIPV4 = m_ipv4Addresses[i];
        Ipv4Address curNeighbor = m_neighbours[i];

        SendPacket(QUERY, curNeighbor, DEFAULT_TTL, curIPV4, sinknode, i);
    }
}

void
P2PApplication::FloodButNotAll(P2PPacket p2pPacket, int excludeIndex)
{
    for (int i = 0; i < m_ipv4Addresses.size(); ++i)
    {
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

// send the packet, typically used for ping and query
void
P2PApplication::SendPacket(MessageType type,
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

// handles logic when reciveing a packet
void
P2PApplication::RecievePacket(Ptr<Socket> socket)
{
    Ptr<Packet> packet = socket->Recv();
    P2PPacket p2pPacket;
    packet->RemoveHeader(p2pPacket);

    // need to check if received to src node with query hit
    for (Ipv4Address ipv4 : m_ipv4Addresses)
    {
        if (p2pPacket.GetSenderIp() == ipv4)
        {
            NS_LOG_INFO("back to sender peer " << ipv4);
            return;
        }
    }

    Ipv4Address senderIP = p2pPacket.GetLastHop();
    int senderIndex = GetNeighbourIndexFromNeighbourIP(senderIP);
    // at this point we have an index of a neighbour that it was sent from
    // we have all addresses and neighbours so we need to send packets to all except for that index

    Ipv4Address curIP = m_ipv4Addresses[senderIndex];
    NS_LOG_INFO("\nrp: at " << curIP << " from " << senderIP << " of type "
                            << p2pPacket.GetMessageType());

    // check if query / sinknode reached
    uint32_t curNodeId = GetNode()->GetId();
    if (p2pPacket.GetMessageType() == QUERY && p2pPacket.GetSinkNode() == curNodeId)
    {
        NS_LOG_INFO("rp: query hit at " << curNodeId << " !!!!!!");
        ForwardQueryHit(p2pPacket, senderIndex);
        return;
    }
    // if query hit the path structure is different
    if (p2pPacket.GetMessageType() == QUERY_HIT)
    {
        ForwardQueryHit(p2pPacket, senderIndex);
        return;
    }
    // drop packet if TTL is already 0 before processing further
    if (p2pPacket.GetTtl() == 0)
    {
        NS_LOG_INFO("TTL 0, dropping packet at " << curIP);
        return;
    }

    // check if node has any neighbours at all (except for the one that it was sent from)
    if (m_neighbours.size() == 1)
    {
        NS_LOG_INFO("rp: at " << curIP << " from " << senderIP << " of type "
                              << p2pPacket.GetMessageType()
                              << " Dropping the packet since no new neighbours");
    }
    // decrement TTL before forwarding
    p2pPacket.DecrementTtl();

    // forward the packet to the neighbours
    FloodButNotAll(p2pPacket, senderIndex);
}

// so right now we have nodeNeighbours and ipv4Addresses. Each of them are linked together on index
// i i.e at index 0 ipv4Addresses[0] is linked to nodeNeighbours[0] so now we can match the
// addresses and find the index in nodeNeighbours
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

// virtual destructor to cklean this (need this to run for the headerhaving ~ declared. missing a
// vtable otherwise)
P2PApplication::~P2PApplication()
{
    StopApplication();
}

// DEBUG functions

void
P2PApplication::LogNodeInfo()
{
    uint32_t nodeId = GetNode()->GetId();
    NS_LOG_INFO("Node " << nodeId << " Information:");

    // Log IP addresses
    for (size_t i = 0; i < m_ipv4Addresses.size(); i++)
    {
        NS_LOG_INFO("  IP Address " << i << " - " << m_ipv4Addresses[i]);
    }

    // Log neighbors
    for (size_t i = 0; i < m_neighbours.size(); i++)
    {
        NS_LOG_INFO("  Node Neighbour " << i << " - " << m_neighbours[i]);
    }

    // Log sockets
    for (size_t i = 0; i < m_sockets.size(); i++)
    {
        if (m_sockets[i])
        {
            Address localAddr;
            m_sockets[i]->GetSockName(localAddr);
            NS_LOG_INFO("  Socket " << i << " - "
                                    << InetSocketAddress::ConvertFrom(localAddr).GetIpv4() << ":"
                                    << InetSocketAddress::ConvertFrom(localAddr).GetPort());
        }
        else
        {
            NS_LOG_INFO("  Socket " << i << " - NULL (not initialized)");
        }
    }
}

// prints socket information
void
P2PApplication::PrintSocketBindings()
{
    NS_LOG_INFO("Total Sockets: " << m_sockets.size());

    for (size_t i = 0; i < m_sockets.size(); i++)
    {
        Ptr<Socket> socket = m_sockets[i];
        Address localAddress;
        socket->GetSockName(localAddress);

        InetSocketAddress inetAddr = InetSocketAddress::ConvertFrom(localAddress);
        Ipv4Address ip = inetAddr.GetIpv4();
        uint16_t port = inetAddr.GetPort();

        NS_LOG_INFO("Socket " << i << " bound to IP: " << ip << " Port: " << port);
    }
}

// prints ipv4 addresses
void
P2PApplication::PrintAddresses()
{
    NS_LOG_INFO("Node " << GetNode()->GetId() << " IP Addresses:");

    for (const auto& addr : m_ipv4Addresses)
    {
        NS_LOG_INFO(" - " << addr);
    }
}

} // namespace ns3
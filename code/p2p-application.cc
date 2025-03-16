// P2PApplication behaviour (where we will be basically building the gnutella like behaviour)
// https://www.sfu.ca/~ljilja/ENSC427/Spring13/Projects/team6/Ensc427_final_project_report.pdf
// http://computer.howstuffworks.com/file-sharing1.htm
// http://www2.ensc.sfu.ca/~ljilja/ENSC427/Spring10/Projects/team7/final_report_Gnutella.pdf

#include "p2p-packet.h"
#include "p2p-application.h"
#include "ns3/header.h"
#include "ns3/packet.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-address.h"
#include <unordered_set>

// logging/debugging purposes
NS_LOG_COMPONENT_DEFINE("P2PApplication");


namespace ns3 {

    NS_OBJECT_ENSURE_REGISTERED(P2PApplication);
    //P2PApplication application;

    P2PApplication::P2PApplication() : m_port(5000), messageIdCount(0) {}   
    P2PApplication::P2PApplication(uint16_t port) : m_port(port), messageIdCount(0) {} // Custom constructor


    

    // keeps a counter of the messages to generate a unique message id
    uint32_t P2PApplication::GenerateMessageId() {
        return messageIdCount ++;
    }

    // called on start, binds each ip with a port to recieve and send from 
    void P2PApplication::StartApplication() {
        if (!m_socket) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            if (!m_socket) {
                NS_LOG_ERROR("socket creation failed for node " << GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal());
                return;
            }

            Ipv4Address localAddress = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
            if (m_socket->Bind(InetSocketAddress(localAddress, m_port)) == -1) {
                NS_LOG_ERROR("socket bind failed on node " << localAddress);
            } 
            else {
                NS_LOG_INFO("socket successfully bound on node " << localAddress);
            }

            m_socket->SetRecvCallback(MakeCallback(&P2PApplication::RecievePacket, this));
        }
    }

    void P2PApplication::StopApplication() {
        if (m_socket) {
            m_socket->Close();
            m_socket = nullptr;
        }
    }
    // after neighbours are set in network sim, init peers as neighbouts
    void P2PApplication::SetPeers(std::vector<Ipv4Address> neighbours) {
        m_neighbours = neighbours;
        // NS_LOG_INFO("set peers " << GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal());
        // for (const auto& neighbor : m_neighbours) {
        //     NS_LOG_INFO("  -> Neighbor: " << neighbor);
        // }
    }

// PEER INTIALIZATION (each node registers itself upon sim start, store neighbour info in local vector)
//  start running as a p2p peer
//  maintiain a list of connected neighbours
//  assign each peer an id and ip address

// QUERY PROPAGATION 
//  A peer should be able to search for a file by sending a query message.
//  Queries can use Flooding, Random Walk, or Expanding Ring techniques.
//  Each query contains a unique ID to prevent loops.
//  Peers forward queries based on the selected technique.

// sendQuery() function


// basic sending and recieving 
// TODO: handle logic for different messages. also will prob need to modify to better suit the gnutella packet

TypeId P2PApplication::GetTypeId() {
    static TypeId tid = TypeId("ns3::P2PApplication")
        .SetParent<Application>()
        .SetGroupName("Tutorial")
        .AddConstructor<P2PApplication>();
    return tid;
}


// ping gets current address, and calls sendpacket for each neighbour 
void P2PApplication::SendPing() {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("Current neighbors: ");
    for (const auto& neighbor : m_neighbours) {
        NS_LOG_INFO("SendingNode: " << curaddy);
        if (neighbor != curaddy) {

            SendPacket(PING, neighbor, 5, curaddy);
        }
    }
}
// send the packet, typically used for ping and query
void P2PApplication::SendPacket(MessageType type, Ipv4Address dest, uint8_t ttl, Ipv4Address curaddy) {
    Ptr<Packet> packet = Create<Packet>();
    uint32_t msgid = GenerateMessageId();
    P2PPacket p2pPacket(type, msgid, curaddy, dest, ttl, 0, curaddy);
    packet->AddHeader(p2pPacket);

    // Send packet over the socket (how ns3 sends stuff)
    m_socket->SendTo(packet, 0, InetSocketAddress(dest, m_port));
    NS_LOG_INFO("Sent " << type << " packet to " << dest);
}


// handles logic when reciveing a packet 
// TODO: modify to change when the type is a query  
void P2PApplication::RecievePacket(Ptr<Socket> socket){
    Ptr<Packet> packet = socket->Recv();  
    P2PPacket p2pPacket;
    packet->RemoveHeader(p2pPacket);  
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("\nReceived packet at " << curaddy << " from " << p2pPacket.GetPrevHop());

    // Drop packet if TTL is already 0 before processing further
    if (p2pPacket.GetTtl() == 0) {
        NS_LOG_INFO("TTL already 0, dropping packet at " << curaddy);
        return;
    }

    // Decrement TTL before forwarding
    p2pPacket.DecrementTtl();

    // Forward the packet to the neighbors
    for (const auto& neighbor : m_neighbours) {
        if (neighbor == curaddy) {
            NS_LOG_WARN("Skipping self-forwarding for " << curaddy);
            continue;
        }

        if (neighbor != p2pPacket.GetSenderIp() && neighbor != p2pPacket.GetPrevHop()) {
            Ptr<Packet> newPacket = Create<Packet>();
            p2pPacket.SetPrevHop(curaddy);  // Update previous hop
            newPacket->AddHeader(p2pPacket);
        
            NS_LOG_INFO("Forwarding packet from " << curaddy << " to " << neighbor);
            m_socket->SendTo(newPacket, 0, InetSocketAddress(neighbor, m_port));
        } else {
            continue;
            //NS_LOG_INFO("Skipping forwarding to " << neighbor << " to prevent loops.");
        }
    }
}

// HandleQuery() -> check if peer has file, if found then send queryHit, else forward toward requester


// NEIGHBOUR DISCOVERY AND PEER CONNECTINOS
//  maintain dynamic list of neighbours
//  send keep-alive messages to detect dead peers 
//  randomly discover new peers, peers send heartbeat messages to keep cache alive 


//  pong()
// Pong descriptors MAY ONLY be sent along the same path that carried the incoming Ping descriptor. 
// This ensures that only those servents that routed the Ping descriptor will see the Pong descriptor in response. 
// A servent that receives a Pong descriptor with Descriptor ID = n, but has not seen a Ping descriptor with Descriptor ID = n 
// SHOULD remove the Pong descriptor from the network and not forward it to any connected servent.

// HANDLING PER JOIN AND LEAVES
//  peer join -> find neighbours and accept queries
// JoinNetwork() registers peer

//  peer leaves -> neighbours update connection list
//  LeaveNetwork() removes peer 


// virtual destructor to cklean this (need this to run for the headerhaving ~ declared. missing a vtable otherwise)
P2PApplication::~P2PApplication() {
    // Cleanup resources, such as closing the socket
    if (m_socket) {
        m_socket->Close();
        m_socket = nullptr;
    }
}

} // namespace ns3
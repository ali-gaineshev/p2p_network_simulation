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

void P2PApplication::StartApplication() {
    if (!m_socket) {
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        if (!m_socket) {
            NS_LOG_ERROR("Socket creation failed!");
            return;
        }
        m_socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_port));
        m_socket->SetRecvCallback(MakeCallback(&P2PApplication::RecievePacket, this));
    }
}
void P2PApplication::StopApplication() {
    if (m_socket) {
        m_socket->Close();
        m_socket = nullptr;
    }
}
void P2PApplication::SetPeers(std::vector<Ipv4Address> neighbours) {
    m_neighbours = neighbours;
    NS_LOG_INFO("Node " << GetNode()->GetId() << " set peers: ");
    // for (const auto& addr : m_neighbours) {
    //     NS_LOG_INFO("  -> " << addr);
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


// PACKET SENDING STUFF  MODIFY THIS FOR LOGIC PING / PONG FIRST
// void P2PApplication::SendPacket(MessageType type, Ipv4Address dest) {
//     Ptr<Packet> packet = Create<Packet>();
//     P2PPacket p2pPacket(type, GenerateMessageId(), GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(), 5);
//     packet->AddHeader(p2pPacket);

//     // Send packet over the socket (how ns3 sends stuff)
//     m_socket->SendTo(packet, 0, InetSocketAddress(dest, m_port));
//     NS_LOG_INFO("Sent " << type << " packet to " << dest);
// }
// void P2PApplication::RecievePacket(Ptr<Socket> socket){
//     Ptr<Packet> packet = socket->Recv();  
//     P2PPacket p2pPacket;
//     packet->RemoveHeader(p2pPacket);  

//     if (p2pPacket.GetMessageType() == PING && p2pPacket.GetTtl() != 0) {
//         p2pPacket.DecrementTtl();
//         NS_LOG_INFO("new ttl for packet " << p2pPacket.GetTtl() );
//         // Forward the packet to the neighbors
//         for (const auto& neighbor : m_neighbours) {
//             Ptr<Packet> newPacket = Create<Packet>();
//             newPacket->AddHeader(p2pPacket);
//             m_socket->SendTo(newPacket, 0, InetSocketAddress(neighbor, m_port));
//             NS_LOG_INFO("Forwarded PING packet to neighbor " << neighbor << " from " << p2pPacket.GetSenderIp());
//         }

//     }

//     NS_LOG_INFO("Received packet of type " << p2pPacket.GetMessageType()
//                 << " from " << p2pPacket.GetSenderIp());
// }

//  ping()
// A servent SHOULD forward incoming Ping and Query descriptors to ALL of its directly connected 
// servents, except the one that delivered the incoming Ping or Query.
// A servent receiving a descriptor with the same Payload Descriptor and Descriptor ID as one it has received before, 
// SHOULD attempt to avoid forwarding the descriptor to any connected servent.
// Its intended recipients have already received such a descriptor, and sending it again merely wastes network bandwidth.

// NEW STUFF BASICALLY THE SAME BUT JUST SO WE CAN EDIT THEM LATER. left the other stuff commented out for now if we need
// to go back for some reason. as you can see the packet keeps not actualy sending to the next node ß
// Starting simulation...
// Current neighbors: 
// Neighbor IP: 10.1.1.2
// Sent 0 packet to 10.1.1.2
// current node after sending 10.1.1.2
// send from 10.1.1.2 to node at 10.1.1.3
// current node after sending 10.1.1.2
// send from 10.1.1.2 to node at 10.1.1.3
// current node after sending 10.1.1.2
// send from 10.1.1.2 to node at 10.1.1.3
// current node after sending 10.1.1.2
// send from 10.1.1.2 to node at 10.1.1.3
// current node after sending 10.1.1.2
// TTL reached 0, dropping packet at 10.1.1.2
// Simulation complete



// ping gets current address, and calls sendpacket for each neighbour 
void P2PApplication::SendPing() {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("Current neighbors: ");
    for (const auto& neighbor : m_neighbours) {
        NS_LOG_INFO("Neighbor IP: " << neighbor);
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
    if (p2pPacket.GetMessageType() == PING && p2pPacket.GetTtl() > 0) {
        p2pPacket.DecrementTtl();
        Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        NS_LOG_INFO("current node after sending " << curaddy);
        
        // explicit packet drop if ttl is 0
        if (p2pPacket.GetTtl() == 0) {
            NS_LOG_INFO("TTL reached 0, dropping packet at " << curaddy);
            return;
        }
        // Forward the packet to the neighbors
        for (const auto& neighbor : m_neighbours) {
            if (neighbor != p2pPacket.GetSenderIp() && neighbor != p2pPacket.GetPrevHop()) {
                Ptr<Packet> newPacket = Create<Packet>();
                p2pPacket.SetPrevHop(curaddy);
                newPacket->AddHeader(p2pPacket);
                NS_LOG_INFO("send from " << curaddy << " to node at " << neighbor );
                m_socket->SendTo(newPacket, 0, InetSocketAddress(neighbor, m_port));
            }
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
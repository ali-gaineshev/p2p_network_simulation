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
#include "ns3/ipv4-header.h"
#include "ns3/udp-socket.h"
#include <unordered_set>
#include <vector>
//#include <udp-socket-impl.h>

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
            //NS_LOG_INFO("Setting receive callback for node " << GetNode()->GetId());
            m_socket->SetRecvCallback(MakeCallback(&P2PApplication::RecievePacket, this));
            //NS_LOG_INFO("Receive callback successfully set on node " << GetNode()->GetId() << " on ip " << localAddress);

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

TypeId P2PApplication::GetTypeId() {
    static TypeId tid = TypeId("ns3::P2PApplication")
        .SetParent<Application>()
        .SetGroupName("Tutorial")
        .AddConstructor<P2PApplication>();
    return tid;
}

// once there is a hit, process and forward backward through the path 
void P2PApplication::QueryHit(P2PPacket ppacket) {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("in QH: curaddy " << curaddy << " prevhop " << ppacket.GetLastHop() << " type " << ppacket.GetMessageType());
    // base case -> at the og addy
    if (ppacket.GetSenderIp() == curaddy) {
        NS_LOG_INFO("query made it back to sender at " << curaddy);
        return;
    }
    // if not at og addy remove last hop, change type, forward
    Ipv4Address nextHop = ppacket.GetLastHop();

    // if (std::find(ppacket.GetPath().begin(), ppacket.GetPath().end(), curaddy) != ppacket.GetPath().end()) {
    //     NS_LOG_WARN("Loop detected! Packet already visited " << curaddy << ". Dropping packet.");
    //     return;
    // }
    ppacket.RemoveLastHop();
    if (nextHop != Ipv4Address("0.0.0.0")) {
        Ptr<Packet> newPacket = Create<Packet>();
        ppacket.SetMessageType(QUERY_HIT);
        ppacket.SetTtl(ppacket.GetTtl()+ 2);
        newPacket->AddHeader(ppacket);
        NS_LOG_INFO("sending QueryHit from " << curaddy << " back to " << nextHop << " type " << ppacket.GetMessageType() << " with ttl " << (int)ppacket.GetTtl());
        int bytesSent = m_socket->SendTo(newPacket, 0, InetSocketAddress(nextHop, m_port));
        //int bytesSent = m_socket->SendTo(newPacket, 0, InetSocketAddress(nextHop, m_port));
        NS_LOG_INFO(bytesSent);
    } else {
        NS_LOG_WARN("no hop found at " << curaddy);
    }
}

// logic for backward forwarding when there is a query hit 
void P2PApplication::ForwardQueryHit(P2PPacket ppacket) {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("in QHF: curaddy " << curaddy << " prevhop " << ppacket.GetLastHop() << " type " << ppacket.GetMessageType());
    if (ppacket.GetSenderIp() == curaddy) {
        NS_LOG_INFO("backt ot originating node " << curaddy);
        return;
    }

    // get the next address in the path
    Ipv4Address nextHop = ppacket.GetLastHop();
    ppacket.RemoveLastHop();

    if (nextHop != Ipv4Address("0.0.0.0")) {
        Ptr<Packet> newPacket = Create<Packet>();
        newPacket->AddHeader(ppacket);
        NS_LOG_INFO("forwarding QH from " << curaddy << " to " << nextHop << " with ttl " << (int)ppacket.GetTtl());
        m_socket->SendTo(newPacket, 0, InetSocketAddress(nextHop, m_port));
    } else {
        NS_LOG_WARN("no hop for QH at " << curaddy);
    }
}
// packet sends a query to find sink node 
void P2PApplication::SendQuery(uint32_t sinknode) {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    for (const auto& neighbor : m_neighbours) {
        SendPacket(QUERY, neighbor, 5, curaddy, sinknode);
    }
}

// ping gets current address, and calls sendpacket for each neighbour 
void P2PApplication::SendPing() {
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("Current neighbors: ");
    for (const auto& neighbor : m_neighbours) {
        NS_LOG_INFO("SendingNode: " << curaddy);
        if (neighbor != curaddy) {
            SendPacket(PING, neighbor, 5, curaddy, -1);
        }
    }
}
// send the packet, typically used for ping and query
void P2PApplication::SendPacket(MessageType type, Ipv4Address dest, uint8_t ttl, Ipv4Address curaddy, uint32_t sinkn) {
    Ptr<Packet> packet = Create<Packet>();
    uint32_t msgid = GenerateMessageId();
    P2PPacket p2pPacket(type, msgid, curaddy, dest, ttl, 0, sinkn, {});
    p2pPacket.AddToPath(curaddy);
    packet->AddHeader(p2pPacket);

    // Send packet over the socket (how ns3 sends stuff)
    m_socket->SendTo(packet, 0, InetSocketAddress(dest, m_port));
    NS_LOG_INFO("Sent " << type << " packet to " << dest);
}


// handles logic when reciveing a packet 
// TODO: modify to change when the type is a query  
void P2PApplication::RecievePacket(Ptr<Socket> socket){
    Ipv4Address ca = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_ERROR("IN RECIEVE PACKET " << ca );
    Ptr<Packet> packet = socket->Recv();  
    P2PPacket p2pPacket;
    packet->RemoveHeader(p2pPacket);  
    Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    NS_LOG_INFO("\nReceived packet at " << curaddy << " from " << p2pPacket.GetLastHop() << " of type " << p2pPacket.GetMessageType());
    //check if query / sinknode reached
    uint32_t curNodeId = GetNode()->GetId();
    if (p2pPacket.GetMessageType() == QUERY && p2pPacket.GetSinkNode() == curNodeId) {
        NS_LOG_INFO("Query Hit at " << curNodeId << " !!!!!!");
        QueryHit(p2pPacket);
        return;
    }
    if (p2pPacket.GetMessageType() == QUERY_HIT) {
        ForwardQueryHit(p2pPacket);
        return;
    }
    // Ipv4Header ipHeader;
    // packet->PeekHeader(ipHeader);
    // NS_LOG_INFO("NS3 IP Layer TTL: " << (int)ipHeader.GetTtl());

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

        if (neighbor != p2pPacket.GetSenderIp() && neighbor != p2pPacket.GetLastHop()) {
            //NS_LOG_INFO("neighbour " << neighbor << " lasthop" << p2pPacket.GetLastHop());
            Ptr<Packet> newPacket = Create<Packet>();
            // Update previous hop
            p2pPacket.AddToPath(curaddy);
            newPacket->AddHeader(p2pPacket);
        
            NS_LOG_INFO("Forwarding packet from " << curaddy << " to " << neighbor);
            m_socket->SendTo(newPacket, 0, InetSocketAddress(neighbor, m_port));
        } else {
            continue;
            //NS_LOG_INFO("Skipping forwarding to " << neighbor << " to prevent loops.");
        }
    }
}

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
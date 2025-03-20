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

// for adding routing for backwards forwarding on qh
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-routing-helper.h"

// logging/debugging purposes
NS_LOG_COMPONENT_DEFINE("P2PApplication");


namespace ns3 {

    NS_OBJECT_ENSURE_REGISTERED(P2PApplication);

    P2PApplication::P2PApplication() : m_port(5000), messageIdCount(0) {}   
    P2PApplication::P2PApplication(uint16_t port) : m_port(port), messageIdCount(0) {} // Custom constructor NEED TO LOOK INTO THIS WARNINGS

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

            // bind a udp socket (bidirectional) to each node address 
            Ipv4Address localAddress = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
            m_socket->Bind(InetSocketAddress(localAddress, m_port));
            // DEBUG -> checking if socket has been bound correctly to the port 
            // if (m_socket->Bind(InetSocketAddress(localAddress, m_port)) == -1) {
            //     NS_LOG_ERROR("socket bind failed on node " << localAddress);
            // } 
            // else {
            //     NS_LOG_INFO("socket successfully bound on node " << localAddress);
            // }

            // when a packet is sent, autocall the recieve packet function to locally deal with logic 
            m_socket->SetRecvCallback(MakeCallback(&P2PApplication::RecievePacket, this));
            //NS_LOG_INFO("Receive callback successfully set on node " << GetNode()->GetId() << " on ip " << localAddress);

        }
    }

    // on stop, close all sockets 
    void P2PApplication::StopApplication() {
        if (m_socket) {
            m_socket->Close();
            m_socket = nullptr;
        }
    }
    // after neighbours are set in network sim, init peers as neighbouts
    void P2PApplication::SetPeers(std::vector<Ipv4Address> neighbours) {
        m_neighbours = neighbours;

        // DEBUG -> test to see what the peers are after setting neighbours 
        // NS_LOG_INFO("set peers " << GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal());
        // for (const auto& neighbor : m_neighbours) {
        //     NS_LOG_INFO("  -> Neighbor: " << neighbor);
        // }
    }


    TypeId P2PApplication::GetTypeId() {
        static TypeId tid = TypeId("ns3::P2PApplication")
            .SetParent<Application>()
            .SetGroupName("Tutorial")
            .AddConstructor<P2PApplication>();
        return tid;
    }

    // when QueryHit or QueryForwarding are called, will add a route to the previous node
    void P2PApplication::AddReverseRoute(Ipv4Address nextHop) {
        if (nextHop == Ipv4Address("0.0.0.0")) {
            NS_LOG_WARN("invalid nextHop address.");
            return;
        }
        // get ipv4 object and protocol. cast the protocol to ipv4routing
        Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4>();
        Ptr<Ipv4RoutingProtocol> routing = ipv4->GetRoutingProtocol();
        Ptr<Ipv4ListRouting> listRouting = DynamicCast<Ipv4ListRouting>(routing);

        if (listRouting) {
            Ptr<Ipv4StaticRouting> staticRouting;
            // iterate through routing protocols to find static routing 
            for (uint32_t i = 0; i < listRouting->GetNRoutingProtocols(); ++i) {
                int16_t priority;
                Ptr<Ipv4RoutingProtocol> protocol = listRouting->GetRoutingProtocol(i, priority);
                staticRouting = DynamicCast<Ipv4StaticRouting>(protocol);
                if (staticRouting) {
                    // add route to nexthop through interface 1
                    staticRouting->AddHostRouteTo(nextHop, 1);
                    return;
                }
            }
        }
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
        // if not at og addy remove last hop, add backward route, change type, forward
        Ipv4Address nextHop = ppacket.GetLastHop();
        AddReverseRoute(nextHop);
        ppacket.SetMessageType(QUERY_HIT);
        ppacket.RemoveLastHop();
        ppacket.SetTtl(ppacket.GetTtl()+ 2);
        ppacket.SetDestIp(nextHop);
        Ptr<Packet> newPacket = Create<Packet>();
        newPacket->AddHeader(ppacket);
        // forward the packet to the previous hop
        NS_LOG_INFO("QH from " << curaddy << " back to " << nextHop << " type " << ppacket.GetMessageType() << " with ttl " << (int)ppacket.GetTtl());
        m_socket->SendTo(newPacket, 0, InetSocketAddress(nextHop, m_port));
    }

    // logic for backward forwarding when there is a query hit 
    void P2PApplication::ForwardQueryHit(P2PPacket ppacket) {
        Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        NS_LOG_INFO("in FQH: curaddy " << curaddy << " prevhop " << ppacket.GetLastHop() << " type " << ppacket.GetMessageType());
        
        // packet is back at sender 
        if (ppacket.GetSenderIp() == curaddy) {
            NS_LOG_INFO("back to sender peer " << curaddy);
            return;
        }
        // if not at og addy remove last hop, add backward route, forward
        Ipv4Address nextHop = ppacket.GetLastHop();
        ppacket.RemoveLastHop();
        AddReverseRoute(nextHop);
        Ptr<Packet> newPacket = Create<Packet>();
        newPacket->AddHeader(ppacket);
        // forward the packet to the previous hop
        NS_LOG_INFO("FQH from " << curaddy << " to " << nextHop << " with ttl " << (int)ppacket.GetTtl());
        m_socket->SendTo(newPacket, 0, InetSocketAddress(nextHop, m_port));
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
                // set sink node -1 becuase its a ping
                SendPacket(PING, neighbor, 5, curaddy, -1);
            }
        }
    }

    // send the packet, typically used for ping and query
    void P2PApplication::SendPacket(MessageType type, Ipv4Address dest, uint8_t ttl, Ipv4Address curaddy, uint32_t sinkn) {
        Ptr<Packet> packet = Create<Packet>();
        uint32_t msgid = GenerateMessageId();
        // start hop at 0, and path at empty
        P2PPacket p2pPacket(type, msgid, curaddy, dest, ttl, 0, sinkn, {});
        p2pPacket.AddToPath(curaddy);
        packet->AddHeader(p2pPacket);

        // Send packet over the socket (how ns3 sends stuff)
        m_socket->SendTo(packet, 0, InetSocketAddress(dest, m_port));
        NS_LOG_INFO("Sent " << type << " packet to " << dest);
    }


    // handles logic when reciveing a packet 
    void P2PApplication::RecievePacket(Ptr<Socket> socket){
        Ptr<Packet> packet = socket->Recv();  
        P2PPacket p2pPacket;
        packet->RemoveHeader(p2pPacket);  
        Ipv4Address curaddy = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        NS_LOG_INFO("\nrp: at " << curaddy << " from " << p2pPacket.GetLastHop() << " of type " << p2pPacket.GetMessageType());
        
        //check if query / sinknode reached
        uint32_t curNodeId = GetNode()->GetId();
        if (p2pPacket.GetMessageType() == QUERY && p2pPacket.GetSinkNode() == curNodeId) {
            NS_LOG_INFO("rp: query hit at " << curNodeId << " !!!!!!");
            QueryHit(p2pPacket);
            return;
        }
        // else check if the message is a query hit, send to forward for proper logic 
        if (p2pPacket.GetMessageType() == QUERY_HIT) {
            ForwardQueryHit(p2pPacket);
            return;
        }
        // drop packet if TTL is already 0 before processing further
        if (p2pPacket.GetTtl() == 0) {
            NS_LOG_INFO("TTL 0, dropping packet at " << curaddy);
            return;
        }
        // decrement TTL before forwarding 
        p2pPacket.DecrementTtl();

        // forward the packet to the neighbours
        for (const auto& neighbor : m_neighbours) {
            if (neighbor == curaddy) {
                NS_LOG_WARN("rp: skipping self-forwarding for " << curaddy);
                continue;
            }
            if (neighbor != p2pPacket.GetSenderIp() && neighbor != p2pPacket.GetLastHop()) {
                // update previous hop and attach to packet
                Ptr<Packet> newPacket = Create<Packet>();
                p2pPacket.AddToPath(curaddy);
                newPacket->AddHeader(p2pPacket);
            
                NS_LOG_INFO("rp: forwarding packet from " << curaddy << " to " << neighbor);
                m_socket->SendTo(newPacket, 0, InetSocketAddress(neighbor, m_port));
            } else {
                continue;
            }
        }
    }

    // virtual destructor to cklean this (need this to run for the headerhaving ~ declared. missing a vtable otherwise)
    P2PApplication::~P2PApplication() {
        if (m_socket) {
            m_socket->Close();
            m_socket = nullptr;
        }
    }

} // namespace ns3
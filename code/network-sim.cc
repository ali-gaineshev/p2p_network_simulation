#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/animation-interface.h"
#include "subdir/p2p/p2p-packet.h"
#include "subdir/p2p/p2p-application.h"
#include <unordered_set>

// define logging component -> P2PNetworkSim for debugging
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int main(int argc, char *argv[]) {

    // pass cmd line args (number of nodes / data range)
    CommandLine cmd;
    cmd.Parse(argc, argv);


    // add NS_LOG_INFO to print assigned ip / asciiTraceHelper to gen logs
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_ALL);
    LogComponentEnable("P2PApplication", LOG_LEVEL_INFO);

    NS_LOG_INFO("entering code");
    // creating the peers
    NodeContainer nodes;
    nodes.Create(5);

    // setting up wired point to point connections between each
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // internet stack on the nodes (TCP/IP protocol stack -> ip addresses)
    // assign those addresses (w/i 10.1.1.0/24 subnet)
    InternetStackHelper internet;
    Ipv4AddressHelper ipv4;
    internet.Install(nodes);
    ipv4.SetBase("10.1.1.0", "255.255.255.0"); 

    // DEBUG -> creates pcap file for wireshark look 
    //p2p.EnablePcapAll("p2p-network-trace");


    // create peer links and assign neighbours from the same spot. added stuff to ensure it is only adding the correct ips
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;
    std::vector<std::vector<Ipv4Address>> nodeNeighbors(nodes.GetN());
    std::vector<Ipv4Address> nodeIps(nodes.GetN());
    for (uint32_t i = 0; i < nodes.GetN() - 1; ++i) {
        NodeContainer pair(nodes.Get(i), nodes.Get(i + 1));
        devices = p2p.Install(pair);
        Ipv4InterfaceContainer tempInterfaces = ipv4.Assign(devices);

        // Assign only one IP per node (store in a separate list with nodes first and secodn ip)
        if (nodeIps[i] == Ipv4Address("10.1.1.0")) {
        nodeIps[i] = tempInterfaces.GetAddress(0);  
        }
        nodeIps[i + 1] = tempInterfaces.GetAddress(1);  

        if (nodeIps[i + 1] != Ipv4Address("10.1.1.0") && nodeIps[i + 1] != Ipv4Address("102.102.102.102")) {
            nodeNeighbors[i].push_back(nodeIps[i + 1]);
        } else {
            NS_LOG_ERROR("Invalid IP detected for Node " << i << " neighbor: " << nodeIps[i + 1]);
        }

        if (nodeIps[i] != Ipv4Address("10.1.1.0") && nodeIps[i] != Ipv4Address("102.102.102.102")) {
            nodeNeighbors[i + 1].push_back(nodeIps[i]);
        } else {
            NS_LOG_ERROR("Invalid IP detected for Node " << i + 1 << " neighbor: " << nodeIps[i]);
        }
    }

    // install the P2PApplication for each node
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        //NS_LOG_INFO("loop " << interfaces.GetAddress(i, 0));
        Ptr<P2PApplication> app = CreateObject<P2PApplication>();
        nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(1.0));
        app->SetStopTime(Seconds(10.0)); // adjust

        app->SetPeers(nodeNeighbors[i]);
    }

    // Start the simulation
    NS_LOG_INFO("Starting simulation...");
    
    // DEBUGGING -> to show all underlying ipv4 protocol logs (helps to see if packet being dropped)
    //LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_ALL);

    // Send a ping from Node 0 to Node 1 (simulation will continue to forward it)
    Simulator::Schedule(Seconds(2.0), &P2PApplication::SendPing, 
                        DynamicCast<P2PApplication>(nodes.Get(0)->GetApplication(0)));
    // Send a query dest at 3
    Simulator::Schedule(Seconds(4.0),MakeEvent(&P2PApplication::SendQuery, 
                        DynamicCast<P2PApplication>(nodes.Get(0)->GetApplication(0)), 3));
    // Send a query dest at 2
    Simulator::Schedule(Seconds(6.0),MakeEvent(&P2PApplication::SendQuery, 
                        DynamicCast<P2PApplication>(nodes.Get(0)->GetApplication(0)), 2));
    // // Send a query dest at 4
    Simulator::Schedule(Seconds(8.0),MakeEvent(&P2PApplication::SendQuery, 
                        DynamicCast<P2PApplication>(nodes.Get(0)->GetApplication(0)), 4));
    
    // UNCOMMENT AND CHANGE THE NAME TO MAKE XML FILE FOR ANIMATION 
    //AnimationInterface anim("p2p-network-q6.xml");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Simulation complete");

    return 0;

}
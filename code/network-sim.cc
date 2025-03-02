#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/animation-interface.h"

// WE WILL END UP CHANGING THIS RN THE NODES ARE CONNECTED LIKE 1-2-3-4-5 just basic for now 
// TODO -> once we have the p2p applicaiton we can use that here 


// define logging component -> P2PNetworkSim for debugging
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int main(int argc, char *argv[]) {
    
    // pass cmd line args (number of nodes / data range)
    CommandLine cmd;
    cmd.Parse(argc, argv);


    // add NS_LOG_INFO to print assigned ip / asciiTraceHelper to gen logs
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);

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

    // connect nodes in chain topology sequentially
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;
    for (uint32_t i = 0; i < nodes.GetN() - 1; ++i) {
        NodeContainer pair(nodes.Get(i), nodes.Get(i + 1));
        // adds p2p link and ip addresses
        devices = p2p.Install(pair);
        interfaces.Add(ipv4.Assign(devices));
    }
    // log IP addresses for verification
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
    // Get the corresponding IP address for the interface (index 1 corresponds to the first assigned IP)
    NS_LOG_INFO("Node " << i << " IP: " << interfaces.GetAddress(i, 0)); // Index 0 corresponds to the first interface
    }

    // actually running this 
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
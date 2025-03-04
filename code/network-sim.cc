#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/animation-interface.h"
#include "subdir/p2p/p2p-packet.h"
#include "subdir/p2p/p2p-application.h"

// WE WILL END UP CHANGING THIS RN THE NODES ARE CONNECTED LIKE 1-2-3-4-5 just basic for now 
// TODO -> once we have the p2p applicaiton we can use that here 

// WHAT DID I ADD
// intialize and serialize a p2p packet, eserialized and verified, general logging
// GOAL IS TO TEST THE PACKET CLASS IS WORKING WELL ENOUGH 

// define logging component -> P2PNetworkSim for debugging
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int main(int argc, char *argv[]) {

    //NS_OBJECT_ENSURE_REGISTERED(P2PPacket);
    
    // pass cmd line args (number of nodes / data range)
    CommandLine cmd;
    cmd.Parse(argc, argv);


    // add NS_LOG_INFO to print assigned ip / asciiTraceHelper to gen logs
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_INFO);


    NS_LOG_INFO("starting sim");
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

    // test packet with info from p2ppacket
    // serializing is just convertin class or instnace to linear format so it can be transmitted had to google
    NS_LOG_INFO("test packet and serialization stuff ");
    P2PPacket testP(PING, 1234, Ipv4Address("10.1.1.1"), 5);
    NS_LOG_INFO("Created packet: Type=" << testP.GetMessageType()
                << ", MessageId=" << testP.GetMessageId()
                << ", SenderIP=" << testP.GetSenderIp()
                << ", TTL=" << (uint32_t)testP.GetTtl());

    // print out packet size 
    Ptr<Packet> ns3Packet = Create<Packet>();
    ns3Packet -> AddHeader(testP);
    NS_LOG_INFO("Serialized packet size: " << ns3Packet->GetSize() << " bytes");

    //deserialize to check correctness (shoudl be the same afterword)
    P2PPacket receivedPacket;
    ns3Packet->RemoveHeader(receivedPacket);
    NS_LOG_INFO("Deserialized packet: Type=" << receivedPacket.GetMessageType()
                << ", MessageId=" << receivedPacket.GetMessageId()
                << ", SenderIP=" << receivedPacket.GetSenderIp()
                << ", TTL=" << (uint32_t)receivedPacket.GetTtl());

    // actually running this 
    NS_LOG_INFO("starting simulation");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("simulation complete");

    return 0;
}
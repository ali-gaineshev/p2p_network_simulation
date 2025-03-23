#include "subdir/p2p/p2p-application.h"
#include "subdir/p2p/p2p-network-builder.h"
#include "subdir/p2p/p2p-packet.h"
#include "subdir/p2p/p2p-util.h"

#include "ns3/animation-interface.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <unordered_set>

// define logging component -> P2PNetworkSim for debugging
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int
main(int argc, char* argv[])
{
    // pass cmd line args (number of nodes / data range)
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // enable logging
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_INFO);
    LogComponentEnable("P2PApplication", LOG_LEVEL_INFO);
    LogComponentEnable("NetworkBuilder", LOG_LEVEL_INFO);
    LogComponentEnable("Util", LOG_LEVEL_INFO);

    NS_LOG_INFO("Starting P2P simulation...");
    // GLOBAL VARIABLES

    /* COMBINED LINEAR AND TREE*/

    // numList will usually take first value in the list when it creates a topology
    // the idea is for combined structure, you will specify how many nodes in each topology
    // there will be. For combined Linear_TREE topology, first value is linear and then tree
    std::vector<uint32_t> nodeNumList = {5, 10};
    int srcIndex = 0;
    int sinkIndex = 12; // sum up nodeNumList to find how many indecies work
    NetworkType networkType = COMBINED_LINEAR_TREE;

    NodeContainer treeNodes; // need a reference for a tree to plot it in netAnim. Still use
                             // net.nodes for the whole toplogy
    P2PNetwork net = CreateP2PNetwork(networkType, nodeNumList, treeNodes);

    /* JUST TREE*/
    // std::vector<uint32_t> nodeNumList = {10};
    // int srcIndex = 0;
    // int sinkIndex = 9;
    // NetworkType networkType = TREE;
    // NodeContainer treeNodes;
    // P2PNetwork net = CreateP2PNetwork(networkType, nodeNumList, treeNodes);

    //  Enable global routing so different subnets can communicate
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Install the P2PApplication for each node
    for (uint32_t i = 0; i < net.nodes.GetN(); ++i)
    {
        Ptr<P2PApplication> app = CreateObject<P2PApplication>();
        net.nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(1.0));
        app->SetStopTime(Seconds(10.0)); // adjust runtime
        app->SetAddresses();
        app->SetPeers(net.nodeNeighbors[i]);
        // logger
        Simulator::Schedule(Seconds(1.1), &P2PApplication::LogNodeInfo, app);
    }

    // DEBUGGING -> to show all underlying ipv4 protocol logs (helps to see if packet being dropped)
    // LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_ALL);

    // Simulate query from node src to sink index
    Simulator::Schedule(
        Seconds(8.0),
        MakeEvent(&P2PApplication::InitialFlood,
                  DynamicCast<P2PApplication>(net.nodes.Get(srcIndex)->GetApplication(srcIndex)),
                  sinkIndex));

    // Create XML animation file

    AnimationInterface anim("p2p-network-routing.xml");

    anim.UpdateNodeDescription(srcIndex, "Src");
    anim.UpdateNodeColor(srcIndex, 0, 0, 255);
    anim.UpdateNodeDescription(sinkIndex, "Sink");
    anim.UpdateNodeColor(sinkIndex, 0, 0, 255);

    // assumption is that there is only 1 tree for now
    if (networkType == COMBINED_LINEAR_TREE)
    {
        P2PUtil::PositionLinearNodes(0, 0, 5, 5, anim, net.nodes, nodeNumList[0]);
        P2PUtil::PositionTreeNodes(0, 45.5, 10.0, 20, 15, anim, treeNodes);
    }

    if (networkType == TREE)
    {
        P2PUtil::PositionTreeNodes(0, 45.5, 10.0, 20, 15, anim, net.nodes);
    }

    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Simulation complete");

    return 0;
}

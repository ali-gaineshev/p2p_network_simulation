#include "subdir/p2p/p2p-application.h"
#include "subdir/p2p/p2p-network-builder.h"
#include "subdir/p2p/p2p-packet.h"

#include "ns3/animation-interface.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <unordered_set>

// define logging component -> P2PNetworkSim for debugging
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

void
PositionTreeNodes(uint32_t nodeIndex,
                  double x,
                  double y,
                  double xOffset,
                  double yOffset,
                  AnimationInterface& anim,
                  NodeContainer& nodes)
{
    uint32_t numNodes = nodes.GetN();
    if (nodeIndex == numNodes)
        return;

    anim.SetConstantPosition(nodes.Get(nodeIndex), x, y);

    uint32_t leftChild = 2 * nodeIndex + 1;
    uint32_t rightChild = 2 * nodeIndex + 2;

    if (leftChild < numNodes)
        PositionTreeNodes(leftChild, x - xOffset, y + yOffset, xOffset / 2, yOffset, anim, nodes);

    if (rightChild < numNodes)
        PositionTreeNodes(rightChild, x + xOffset, y + yOffset, xOffset / 2, yOffset, anim, nodes);
}

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

    // GLOBAL VARIABLES
    uint32_t numNodes = 10;
    int srcIndex = 0;
    int sinkIndex = 9;

    NS_LOG_INFO("Starting P2P simulation...");
    P2PNetwork net = CreateP2PNetwork(TREE, numNodes);
    // Enable global routing so different subnets can communicate
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

    // Simulate query from node 0 to node 4
    Simulator::Schedule(Seconds(8.0),
                        MakeEvent(&P2PApplication::Flood,
                                  DynamicCast<P2PApplication>(net.nodes.Get(0)->GetApplication(0)),
                                  sinkIndex));

    // Create XML animation file
    AnimationInterface anim("p2p-network-routing.xml");

    anim.UpdateNodeDescription(srcIndex, "Src");
    anim.UpdateNodeColor(srcIndex, 0, 0, 255);
    anim.UpdateNodeDescription(sinkIndex, "Sink");
    anim.UpdateNodeColor(sinkIndex, 0, 0, 255);

    PositionTreeNodes(0, 45.5, 1.0, 15, 15, anim, net.nodes);

    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Simulation complete");

    return 0;
}

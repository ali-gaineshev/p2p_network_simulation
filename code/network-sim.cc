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

#define DEFAULT_TTL 1

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int
main(int argc, char* argv[])
{
    // enable logging
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_INFO);
    LogComponentEnable("P2PApplication", LOG_LEVEL_INFO);
    LogComponentEnable("P2PApplication", LOG_LEVEL_DEBUG);
    LogComponentEnable("NetworkBuilder", LOG_LEVEL_INFO);
    LogComponentEnable("Util", LOG_LEVEL_INFO);

    // DEFAULT VALUES
    uint32_t nodeNum = 5;
    int srcIndex = 0;
    int sinkIndex = 4;
    int networkTypeInt = TREE;
    std::string fileName = "";

    // Pass cl args. Add more if you feel like it
    CommandLine cmd;
    cmd.AddValue("nodeNum", "Number of nodes in the network", nodeNum);
    cmd.AddValue("srcIndex", "Source node index", srcIndex);
    cmd.AddValue("sinkIndex", "Sink node index", sinkIndex);
    cmd.AddValue("networkType",
                 "Network type (0: LINEAR, 1: TREE, 2: MESH, 3: D-REGULAR)",
                 networkTypeInt);
    cmd.AddValue("fileName",
                 "Name of the file. May be empty. Use this for D-Regular and Clusters. For "
                 "example: 'scratch/code/subdir/p2p/5_regular_with_10_nodes.txt'",
                 fileName);
    cmd.Parse(argc, argv);

    // Convert int to enum
    NetworkType networkType = static_cast<NetworkType>(networkTypeInt);

    // Ensure valid indices
    if (srcIndex < 0 || srcIndex >= (int)nodeNum || sinkIndex < 0 || sinkIndex >= (int)nodeNum)
    {
        NS_LOG_ERROR("Invalid source or sink index.");
        return 1;
    }

    P2PNetwork net = CreateP2PNetwork(networkType, nodeNum, fileName);

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
        // Simulator::Schedule(Seconds(1.1), &P2PApplication::LogNodeInfo, app);
    }

    // DEBUGGING -> to show all underlying ipv4 protocol logs (helps to see if packet being dropped)
    // LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_ALL);

    // Simulate query from node src to sink index

    auto app = DynamicCast<P2PApplication>(net.nodes.Get(srcIndex)->GetApplication(0));
    if (app)
    {
        NS_LOG_INFO("Starting P2P simulation...");
        Simulator::Schedule(Seconds(2),
                            &P2PApplication::ScheduleSearchWithRetry,
                            app,
                            FLOOD,
                            sinkIndex,
                            DEFAULT_TTL,
                            0);
    }

    // simulate random walk query
    // Simulator::Schedule(
    //     Seconds(8.0),
    //     MakeEvent(&P2PApplication::InitialRandomWalk,
    //               DynamicCast<P2PApplication>(net.nodes.Get(srcIndex)->GetApplication(srcIndex)),
    //               sinkIndex,
    //               2));

    // simulate a normalized floodign query
    // Simulator::Schedule(
    //          Seconds(8.0),
    //          MakeEvent(&P2PApplication::InitialNormalizedFlood,
    //                  DynamicCast<P2PApplication>(net.nodes.Get(srcIndex)->GetApplication(srcIndex)),
    //                  sinkIndex, 1));

    // Create XML animation file
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.InstallAll();

    AnimationInterface anim("p2p-network-routing.xml");

    anim.UpdateNodeDescription(srcIndex, "Src");
    anim.UpdateNodeColor(srcIndex, 0, 0, 255);
    anim.UpdateNodeDescription(sinkIndex, "Sink");
    anim.UpdateNodeColor(sinkIndex, 0, 0, 255);

    // assumption is that there is only 1 tree for now

    if (networkType == TREE)
    {
        P2PUtil::PositionTreeNodes(0, 45.5, 10.0, 20, 15, anim, net.nodes);
    }

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}

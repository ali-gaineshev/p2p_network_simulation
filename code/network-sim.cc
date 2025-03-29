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

#define DEFAULT_TTL 5
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("P2PNetworkSim");

int
main(int argc, char* argv[])
{
    // enable logging
    LogComponentEnable("P2PNetworkSim", LOG_LEVEL_INFO);
    LogComponentEnable("P2PPacket", LOG_LEVEL_INFO);
    LogComponentEnable("P2PApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("P2PApplication", LOG_LEVEL_DEBUG);
    LogComponentEnable("NetworkBuilder", LOG_LEVEL_INFO);
    LogComponentEnable("Util", LOG_LEVEL_INFO);

    std::string fileName;
    uint32_t nodeNum = -1;
    int srcIndex = -1;
    int sinkIndex = -1;
    int networkTypeInt = -1;
    int searchAlgorithmInt = -1;
    uint32_t walkers = -1;
    uint32_t ttl = DEFAULT_TTL;
    // Pass cl args.
    CommandLine cmd;
    cmd.AddValue("nodeNum", "Number of nodes in the network", nodeNum);
    cmd.AddValue("srcIndex", "Source node index", srcIndex);
    cmd.AddValue("sinkIndex", "Sink node index", sinkIndex);
    cmd.AddValue("networkType",
                 "Network type (0: LINEAR, 1: TREE, 2: MESH, 3: D-REGULAR)",
                 networkTypeInt);
    cmd.AddValue("fileName",
                 "Name of the file. May be empty. Use this for D-Regular and Clusters.",
                 fileName);
    cmd.AddValue("walkers", "K-walkers in Random Walk or k in Normalized Flood ", walkers);
    cmd.AddValue("ttl", "Default TTL for the query. Default is 5", ttl);
    cmd.AddValue("searchAlg",
                 "Search Algorithm (0: FLOOD, 1: RANDOM_WALK, 2: NORMALIZED_FLOOD)",
                 searchAlgorithmInt);
    cmd.Parse(argc, argv);

    if (searchAlgorithmInt == -1 || srcIndex == -1 || sinkIndex == -1 || networkTypeInt == -1 ||
        (fileName.empty() && nodeNum == -1))
    {
        NS_LOG_ERROR("Please provide all the required parameters.");
        return 1;
    }

    if ((searchAlgorithmInt == 1 || searchAlgorithmInt == 2) && walkers == -1)
    {
        NS_LOG_ERROR("Please provide a valid number of walkers.");
        return 1;
    }

    // Convert int to enum
    NetworkType networkType = static_cast<NetworkType>(networkTypeInt);
    SearchAlgorithm searchAlgorithm = static_cast<SearchAlgorithm>(searchAlgorithmInt);

    // Build the network
    P2PNetwork net = CreateP2PNetwork(networkType, nodeNum, fileName);
    // reassign
    nodeNum = net.nodes.GetN();

    // Install the P2PApplication for each node
    for (uint32_t i = 0; i < net.nodes.GetN(); ++i)
    {
        Ptr<P2PApplication> app = CreateObject<P2PApplication>();
        net.nodes.Get(i)->AddApplication(app);
        app->SetStartTime(Seconds(1.0));
        app->SetStopTime(Seconds(15.0)); // adjust runtime
        app->SetAddresses();
        app->SetPeers(net.nodeNeighbors[i]);
        // logger
        // Simulator::Schedule(Seconds(1.1), &P2PApplication::LogNodeInfo, app);
    }

    // DEBUGGING -> to show all underlying ipv4 protocol logs (helps to see if packet being dropped)
    // LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_ALL);

    // Simulation runner
    auto app = DynamicCast<P2PApplication>(net.nodes.Get(srcIndex)->GetApplication(0));
    if (app)
    {
        NS_LOG_INFO("\nStarting P2P simulation...\n");
        Simulator::Schedule(Seconds(5),
                            &P2PApplication::ScheduleSearchWithRetry,
                            app,
                            searchAlgorithm,
                            sinkIndex,
                            ttl,
                            walkers);
    }

    // Create XML animation file

    // this generates warnings but if it's not tree (file) then the animation is not correct
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X",
                                  StringValue("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"),
                                  "Y",
                                  StringValue("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
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

    NS_LOG_INFO("\nSimulation Statistics:");
    NS_LOG_INFO("----------------------");

    for (int i = 0; i < nodeNum; i++)
    {
        Ptr<P2PApplication> app = DynamicCast<P2PApplication>(net.nodes.Get(i)->GetApplication(0));
        if (app)
        {
            NS_LOG_INFO("Node " << i << ":");

            if (i == srcIndex || i == sinkIndex)
            {
                NS_LOG_INFO("  Query Hits: " << app->GetQueryHits());
                auto hops = app->GetHopsForQueryHits();
                for (int i = 1; i < hops.size() + 1; i++)
                {
                    NS_LOG_INFO("       Hops for Query Hit " << i << " is " << hops[i - 1]);
                }
            }

            NS_LOG_INFO("  Received Requests: " << app->GetReceivedRequests());
            NS_LOG_INFO("  Sent Requests: " << app->GetSentRequests());
            NS_LOG_INFO("  Forwarded Query Hits: " << app->GetForwardedQueryHits());
            if (i == srcIndex)
            {
                NS_LOG_INFO("  Tried Requests: " << app->GetTriedRequests());
                NS_LOG_INFO("  Initialized Requests: " << app->GetInitializedRequests());
            }
        }
    }

    Simulator::Destroy();

    return 0;
}

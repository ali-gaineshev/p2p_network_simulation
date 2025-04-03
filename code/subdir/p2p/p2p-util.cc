#include "p2p-util.h"

#include "ns3/animation-interface.h"
#include "ns3/inet-socket-address.h"
#include "ns3/log.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <unistd.h>
#include <vector>

#define NS3_FOLDER "scratch/code/stats/"
NS_LOG_COMPONENT_DEFINE("Util");

struct FILENAMES
{
    std::string stats;
    std::string query_hits;
};

namespace ns3
{

FILENAMES
generateFileName(std::string algorithmFolder, int searchAlgorithmInt)
{
    std::string algorithm;
    if (searchAlgorithmInt == 0)
    {
        algorithm = "flood";
    }
    else if (searchAlgorithmInt == 1)
    {
        algorithm = "random_walk";
    }
    else if (searchAlgorithmInt == 2)
    {
        algorithm = "n_flood";
    }

    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H%M%S");
    std::string timestamp = oss.str();

    FILENAMES filenames = {
        NS3_FOLDER + algorithmFolder + "/" + algorithm + "_" + timestamp + ".csv",
        NS3_FOLDER + algorithmFolder + "/" + algorithm + "_queryhits_" + timestamp + ".csv"};
    return filenames;
}

void
P2PUtil::saveStatsAsCSV(NodeContainer nodes, std::string algorithmFolder, int searchAlgorithmInt)
{
    FILENAMES filenames = generateFileName(algorithmFolder, searchAlgorithmInt);
    std::string filename = filenames.stats;
    std::string queryHitsFilename = filenames.query_hits;

    std::ofstream csvFile(filename);
    std::ofstream queryhitCsvFile(queryHitsFilename);
    if (!csvFile.is_open() || !queryhitCsvFile.is_open())
    {
        NS_LOG_ERROR("Failed to open CSV files");
        return;
    }

    // ----------- main csv file ----------------
    // Write CSV header
    csvFile << "NodeID,IsSink,IsSource,QueryHits,ReceivedRequests,"
            << "SentRequests,ForwardedQueryHits,TriedRequests,"
            << "InitializedRequests\n";

    int nodeNum = nodes.GetN();
    for (int i = 0; i < nodeNum; i++)
    {
        Ptr<P2PApplication> app = DynamicCast<P2PApplication>(nodes.Get(i)->GetApplication(0));
        if (!app)
            continue;

        bool isSinkNode = app->IsSinkNode();
        bool isSrcNode = app->IsSrcNode();

        // Write basic node info
        csvFile << i << "," << (isSinkNode ? "1" : "0") << "," << (isSrcNode ? "1" : "0") << ","
                << app->GetQueryHits() << "," << app->GetReceivedRequests() << ","
                << app->GetSentRequests() << "," << app->GetForwardedQueryHits() << ",";

        // Source-specific stats
        if (isSrcNode)
        {
            csvFile << app->GetTriedRequests() << "," << app->GetInitializedRequests();
        }
        else
        {
            csvFile << "0,0"; // Empty values for non-source nodes
        }

        csvFile << "\n";
    }

    csvFile.close();
    NS_LOG_INFO("Saved statistics to: " << filename);

    // ----------- query hit csv file ----------------
    queryhitCsvFile << "QueryHitId, Hops, Seconds\n";
    Ptr<P2PApplication> app = DynamicCast<P2PApplication>(nodes.Get(0)->GetApplication(0));
    auto hops = app->GetHopsForQueryHits();
    auto seconds = app->GetSecondsForQueryHits();

    for (int i = 1; i < hops.size() + 1; i++)
    {
        double second = seconds[i - 1];

        queryhitCsvFile << i << "," << hops[i - 1] << "," << second << "\n";
    }
    queryhitCsvFile.close();
    NS_LOG_INFO("Saved query hits to: " << queryHitsFilename);

    // DEBUG
    // for (int i = 0; i < nodeNum; i++)
    // {
    //     Ptr<P2PApplication> app = DynamicCast<P2PApplication>(nodes.Get(i)->GetApplication(0));
    //     if (app)
    //     {
    //         NS_LOG_INFO("Node " << i << ":");

    //         bool isSinkNode = app->IsSinkNode();
    //         bool isSrcNode = app->IsSrcNode();

    //         if (isSinkNode || isSrcNode)
    //         {
    //             NS_LOG_INFO("  Query Hits: " << app->GetQueryHits());
    //             auto hops = app->GetHopsForQueryHits();
    //             for (int i = 1; i < hops.size() + 1; i++)
    //             {
    //                 NS_LOG_INFO("       Hops for Query Hit " << i << " is " << hops[i - 1]);
    //             }
    //         }

    //         NS_LOG_INFO("  Received Requests: " << app->GetReceivedRequests());
    //         NS_LOG_INFO("  Sent Requests: " << app->GetSentRequests());
    //         NS_LOG_INFO("  Forwarded Query Hits: " << app->GetForwardedQueryHits());
    //         if (isSrcNode)
    //         {
    //             NS_LOG_INFO("  Tried Requests: " << app->GetTriedRequests());
    //             NS_LOG_INFO("  Initialized Requests: " << app->GetInitializedRequests());
    //         }
    //     }
    // }
}

std::vector<std::vector<int>>
P2PUtil::readGraphFromFile(const std::string& filename)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        std::cout << "Current working dir: " << cwd << std::endl;

    NS_LOG_INFO("FILENAME: " << filename);
    std::ifstream file(filename);
    if (!file)
    {
        NS_LOG_INFO("Couldn't read the file");
        exit(1);
    }

    int numNodes;
    file >> numNodes; // Read number of nodes

    std::vector<std::vector<int>> adjList(numNodes); // Initialize adjacency list

    int u, v;
    while (file >> u >> v)
    { // Read edges
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    file.close();
    return adjList;
}

// void
// P2PUtil::printGraph(const std::vector<std::vector<int>>& adjList)
// {
//     for (size_t i = 0; i < adjList.size(); i++)
//     {
//         std::cout << i << ": ";
//         for (int neighbor : adjList[i])
//         {
//             std::cout << neighbor << " ";
//         }
//         std::cout << std::endl;
//     }
// }

void
P2PUtil::PositionLinearNodes(int curIndex,
                             double x,
                             double y,
                             double xOffset,
                             AnimationInterface& anim,
                             NodeContainer& nodes,
                             int totalLinearNodes)
{
    if (curIndex == totalLinearNodes)
    {
        return;
    }

    anim.SetConstantPosition(nodes.Get(curIndex), x, y);

    PositionLinearNodes(curIndex + 1, x + xOffset, y, xOffset, anim, nodes, totalLinearNodes);
}

// Positions Nodes in NetAnim --------------------------------------- network-sim.cc
void
P2PUtil::PositionTreeNodes(uint32_t nodeIndex,
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

// Positions Nodes in NetAnim --------------------------------------- network-sim.cc
void
P2PUtil::PositionClusterNodes(uint32_t nodeIndex,
                              double centerX,
                              double centerY,
                              double repRadius,
                              double memberRadius,
                              AnimationInterface& anim,
                              NodeContainer& nodes)
{
    uint32_t totalNodes = nodes.GetN();
    if (totalNodes == 0)
        return;

    anim.SetConstantPosition(nodes.Get(0), centerX, centerY);
    uint32_t currentNode = 1;
    uint32_t numClusters = 0;

    // Estimate number of clusters (nodes directly connected to 0)
    for (uint32_t i = 1; i < totalNodes; ++i)
    {
        if (nodes.Get(i)->GetObject<Ipv4>()->GetNInterfaces() > 2)
        {
            numClusters++;
        }
    }

    double angleIncrement = 2 * M_PI / numClusters;
    uint32_t clusterIndex = 0;

    while (currentNode < totalNodes)
    {
        double angle = clusterIndex * angleIncrement;
        double repX = centerX + repRadius * cos(angle);
        double repY = centerY + repRadius * sin(angle);

        // Place representative
        anim.SetConstantPosition(nodes.Get(currentNode), repX, repY);
        ++currentNode;

        // Now place its cluster members in a small circle around it
        uint32_t membersInCluster = 0;
        uint32_t estMembers = nodes.Get(currentNode - 1)->GetObject<Ipv4>()->GetNInterfaces() - 1;
        for (uint32_t j = 0; j < estMembers && currentNode < totalNodes; ++j, ++currentNode)
        {
            double memberAngle = 2 * M_PI * j / estMembers;
            double memberX = repX + memberRadius * cos(memberAngle);
            double memberY = repY + memberRadius * sin(memberAngle);
            anim.SetConstantPosition(nodes.Get(currentNode), memberX, memberY);
            ++clusterIndex;
        }
    }
}

// Prints Node Information --------------------------------------- p2p-application.cc
void
P2PUtil::PrintNodeInfo(Ptr<P2PApplication> app)
{
    auto ipv4Addresses = app->GetIpv4Addresses();
    auto neighbours = app->GetNeighbours();
    auto sockets = app->GetSockets();

    uint32_t nodeId = app->GetNode()->GetId();
    NS_LOG_INFO("Node ID " << nodeId << " Information:");

    for (size_t i = 0; i < ipv4Addresses.size(); i++)
    {
        NS_LOG_INFO("  IP Address " << i << " - " << ipv4Addresses[i]);
    }

    for (size_t i = 0; i < neighbours.size(); i++)
    {
        NS_LOG_INFO("  Node Neighbour " << i << " - " << neighbours[i]);
    }

    for (size_t i = 0; i < sockets.size(); i++)
    {
        if (sockets[i])
        {
            Address localAddr;
            sockets[i]->GetSockName(localAddr);
            NS_LOG_INFO("  Socket " << i << " - "
                                    << InetSocketAddress::ConvertFrom(localAddr).GetIpv4() << ":"
                                    << InetSocketAddress::ConvertFrom(localAddr).GetPort());
        }
        else
        {
            NS_LOG_INFO("  Socket " << i << " - NULL (not initialized)");
        }
    }
}

void
P2PUtil::PrintSocketBindings(Ptr<P2PApplication> app)
{
    auto sockets = app->GetSockets();
    NS_LOG_INFO("Total Sockets: " << sockets.size());

    for (size_t i = 0; i < sockets.size(); i++)
    {
        Ptr<Socket> socket = sockets[i];
        Address localAddress;
        socket->GetSockName(localAddress);

        InetSocketAddress inetAddr = InetSocketAddress::ConvertFrom(localAddress);
        Ipv4Address ip = inetAddr.GetIpv4();
        uint16_t port = inetAddr.GetPort();

        NS_LOG_INFO("Socket " << i << " bound to IP: " << ip << " Port: " << port);
    }
}

void
P2PUtil::PrintAddresses(Ptr<P2PApplication> app)
{
    auto ipv4Addresses = app->GetIpv4Addresses();
    NS_LOG_INFO("Node " << app->GetNode()->GetId() << " IP Addresses:");

    for (const auto& addr : ipv4Addresses)
    {
        NS_LOG_INFO(" - " << addr);
    }
}

// Prints Network Information --------------------------------------- p2p-network-builder.cc

void
P2PUtil::PrintNetworkInfo(const P2PNetwork& net)
{
    NS_LOG_INFO("------------------------------");
    NS_LOG_INFO("Network Information from p2p-util.cc:");

    for (uint32_t i = 0; i < net.nodes.GetN(); ++i)
    {
        Ptr<Node> node = net.nodes.Get(i);
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();

        NS_LOG_INFO("Node " << i << " | Node ID - " << node->GetId()
                            << " | NetDevices: " << ipv4->GetNInterfaces() - 1
                            << " | Neighbors: " << net.nodeNeighbors[i].size());

        for (uint32_t j = 1; j < ipv4->GetNInterfaces(); ++j)
        { // Skip loopback interface
            NS_LOG_INFO("  Interface " << j << " -> IP: " << ipv4->GetAddress(j, 0).GetLocal()
                                       << " | Neighbor: " << net.nodeNeighbors[i][j - 1]);
        }
    }
    NS_LOG_INFO("------------------------------\n");
}
} // namespace ns3

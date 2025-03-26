#include "p2p-util.h"

#include "ns3/animation-interface.h"
#include "ns3/inet-socket-address.h"
#include "ns3/log.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
NS_LOG_COMPONENT_DEFINE("Util");

namespace ns3
{

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

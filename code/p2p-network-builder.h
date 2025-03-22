#ifndef P2P_NETWORK_BUILDER_H
#define P2P_NETWORK_BUILDER_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

enum NetworkType
{
    LINEAR,
    TREE,
    MESH
};

struct P2PNetwork
{
    NodeContainer nodes;
    std::vector<std::vector<Ipv4Address>> nodeNeighbors;
};

P2PNetwork CreateP2PNetwork(NetworkType networkType, uint32_t numNodes);

void PrintNetworkInfo(const P2PNetwork& net);

#endif

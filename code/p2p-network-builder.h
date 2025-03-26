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
    MESH,
    COMBINED_LINEAR_TREE
};

struct P2PNetwork
{
    NodeContainer nodes;
    std::vector<std::vector<Ipv4Address>> nodeNeighbors;
};

P2PNetwork CreateP2PNetwork(NetworkType, uint32_t);

P2PNetwork AddNewNodeToExistingNet(P2PNetwork&, int);

P2PNetwork CreateLinearNetwork(uint32_t);
P2PNetwork CreateTreeNetwork(uint32_t);
P2PNetwork CreateMeshNetwork(uint32_t, int, double);
P2PNetwork CreateCombinedLinearTreeNetwork(uint32_t, uint32_t, NodeContainer&);
Ipv4Address generateIpv4Base(int, std::string);

#endif

#include "p2p-network-builder.h"

#include "p2p-util.h"

#include "ns3/ipv4.h"
#include "ns3/log.h"

#define LINEAR_BASE_IPV4 "30."
#define TREE_LEFT_BASE_IPV4 "10."
#define TREE_RIGHT_BASE_IPV4 "20."
#define BASE_IPV4_ENDING ".1.0"
#define NETWORK_MASK "255.255.255.0"

P2PNetwork CreateLinearNetwork(uint32_t);
P2PNetwork CreateTreeNetwork(uint32_t);
P2PNetwork CreateCombinedLinearTreeNetwork(uint32_t, uint32_t, NodeContainer&);

NS_LOG_COMPONENT_DEFINE("NetworkBuilder");

// ASSUMPTIONS:
// from testing whenever you create a node, ns3 has an internal counter for node Id
// that means we can create structures seprately and then join them

P2PNetwork
CreateP2PNetwork(NetworkType networkType, std::vector<uint32_t> numList, NodeContainer& treeNodes)
{
    switch (networkType)
    {
    case LINEAR:
        return CreateLinearNetwork(numList[0]);
    case TREE:
        return CreateTreeNetwork(numList[0]);
    case COMBINED_LINEAR_TREE:
        return CreateCombinedLinearTreeNetwork(numList[0], numList[1], treeNodes);
    default:
        NS_LOG_ERROR("Unsupported network type");
        return P2PNetwork();
    }
}

P2PNetwork
CreateCombinedLinearTreeNetwork(uint32_t linearNumNodes,
                                uint32_t treeNumNodes,
                                NodeContainer& treeNodes)
{
    P2PNetwork net;

    P2PNetwork linearNet = CreateLinearNetwork(linearNumNodes);
    P2PNetwork treeNet = CreateTreeNetwork(treeNumNodes);
    treeNodes = treeNet.nodes;
    // need to crate a connection between last node in first network
    // and first node in second network
    // Merge node lists
    net.nodes.Add(linearNet.nodes);
    net.nodes.Add(treeNet.nodes);

    // Merge neighbors
    net.nodeNeighbors = linearNet.nodeNeighbors;
    net.nodeNeighbors.insert(net.nodeNeighbors.end(),
                             treeNet.nodeNeighbors.begin(),
                             treeNet.nodeNeighbors.end());

    // base ipv4 for the new connection
    std::ostringstream baseIPForBridgeConnection;
    baseIPForBridgeConnection << LINEAR_BASE_IPV4 << std::to_string(linearNumNodes)
                              << BASE_IPV4_ENDING;

    // Create a connection between the last node of linear and first node of tree
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NodeContainer bridgeNodes;
    bridgeNodes.Add(linearNet.nodes.Get(linearNumNodes - 1));
    bridgeNodes.Add(treeNet.nodes.Get(0));

    NetDeviceContainer bridgeDevices = p2p.Install(bridgeNodes);

    Ipv4AddressHelper address;
    address.SetBase(baseIPForBridgeConnection.str().c_str(), "255.255.255.0");
    Ipv4InterfaceContainer bridgeInterface = address.Assign(bridgeDevices);

    // Add neighbor connection
    net.nodeNeighbors[linearNumNodes - 1].push_back(bridgeInterface.GetAddress(1));
    net.nodeNeighbors[linearNumNodes].push_back(bridgeInterface.GetAddress(0));

    P2PUtil::PrintNetworkInfo(net);
    return net;
}

// Ptr<Node> linearLastNode = linearNet.nodes.Get(linearNumNodes - 1);
// Ptr<Node> treeFirstNode = treeNet.nodes.Get(0);

// // base ipv4 for the new connection
// std::ostringstream baseIPForBridgeConnection;
// baseIPForBridgeConnection << LINEAR_BASE_IPV4 << std::to_string(linearNumNodes)
//                           << BASE_IPV4_ENDING;

// // create a bridge between linear and tree topologies
// NodeContainer nodeBridge;
// nodeBridge.Add(linearLastNode);
// nodeBridge.Add(treeFirstNode);

// // Install internet stack
// InternetStackHelper internet;
// internet.Install(nodeBridge);

// // Point-to-Point helper for connections
// PointToPointHelper p2p;
// p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
// p2p.SetChannelAttribute("Delay", StringValue("2ms"));

// NetDeviceContainer device = p2p.Install(nodeBridge);
// Ipv4AddressHelper address;
// address.SetBase(baseIPForBridgeConnection.str().c_str(), NETWORK_MASK);
// Ipv4InterfaceContainer interface = address.Assign(device);

// //
// linearNet.nodeNeighbors[linearNumNodes - 1].push_back(interface.GetAddress(1));
// treeNet.nodeNeighbors[0].push_back(interface.GetAddress(0));

// net.nodes.Add(linearNet.nodes);
// net.nodes.Add(nodeBridge);
// net.nodes.Add(treeNet.nodes);

// net.nodeNeighbors.reserve(linearNet.nodeNeighbors.size() + treeNet.nodeNeighbors.size());
// net.nodeNeighbors.insert(net.nodeNeighbors.end(),
//                          linearNet.nodeNeighbors.begin(),
//                          linearNet.nodeNeighbors.end());
// net.nodeNeighbors.insert(net.nodeNeighbors.end(),
//                          treeNet.nodeNeighbors.begin(),
//                          treeNet.nodeNeighbors.end());

// NS_LOG_INFO(net.nodes.GetN());
// NS_LOG_INFO(net.nodeNeighbors.size());
// P2PUtil::PrintNetworkInfo(net);
// return net;

P2PNetwork
CreateLinearNetwork(uint32_t numNodes)
{
    P2PNetwork net;

    if (numNodes < 2)
    {
        throw std::runtime_error("A network requires at least two nodes.");
    }

    // Create nodes
    NodeContainer nodes;
    nodes.Create(numNodes);

    // Install internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    // Point-to-Point helper for connections
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // IPv4 address helper
    Ipv4AddressHelper address;
    uint32_t ipCounter = 1;

    // Vector to store node neighbors
    std::vector<std::vector<Ipv4Address>> nodeNeighbors(numNodes);

    // Create point-to-point links between consecutive nodes
    for (uint32_t i = 0; i < numNodes - 1; i++)
    {
        NodeContainer nodePair;
        nodePair.Add(nodes.Get(i));
        nodePair.Add(nodes.Get(i + 1));

        // Install devices
        NetDeviceContainer devices = p2p.Install(nodePair);

        // Assign IPv4 addresses
        std::ostringstream baseIP;
        baseIP << LINEAR_BASE_IPV4 << ipCounter << BASE_IPV4_ENDING;
        address.SetBase(baseIP.str().c_str(), NETWORK_MASK);
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        // Store neighbor relationships
        nodeNeighbors[i].push_back(interfaces.GetAddress(1));
        nodeNeighbors[i + 1].push_back(interfaces.GetAddress(0));

        ipCounter++;
    }

    // Assign to P2PNetwork struct
    net.nodes = nodes;
    net.nodeNeighbors = nodeNeighbors;

    P2PUtil::PrintNetworkInfo(net);
    return net;
}

P2PNetwork
CreateTreeNetwork(uint32_t numNodes)
{
    if (numNodes < 2)
    {
        throw std::runtime_error("A network requires at least two nodes.");
    }

    // network
    P2PNetwork net;

    // setting up wired point-to-point connections between each
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // creating the peers
    NodeContainer allNodes;
    allNodes.Create(numNodes);
    // [[node01, node02], [node13, node14], [node25, node26], ....]
    std::vector<std::vector<NodeContainer>> nodeCon;

    // install internet stack
    InternetStackHelper internet;
    internet.Install(allNodes);

    // list of node neighbours based on index
    std::vector<std::vector<Ipv4Address>> nodeNeighbors(numNodes);

    // ipv4 address counter. each netdevice will have it's own address
    uint32_t ipCounter = 1;
    Ipv4AddressHelper address;

    // make node connections, devices and chanells
    for (uint32_t i = 0; i < numNodes; i++)
    {
        uint32_t left = i * 2 + 1;
        uint32_t right = i * 2 + 2;
        std::vector<NodeContainer> curNode;

        if (left < numNodes)
        {
            // make nodes for index i
            NodeContainer nodePair;
            nodePair.Add(allNodes.Get(i));
            nodePair.Add(allNodes.Get(left));
            // make net device
            NetDeviceContainer device = p2p.Install(nodePair);

            // set IPv4 address
            std::ostringstream baseIP;
            baseIP << TREE_LEFT_BASE_IPV4 << ipCounter << BASE_IPV4_ENDING;
            address.SetBase(baseIP.str().c_str(), NETWORK_MASK);
            Ipv4InterfaceContainer interface = address.Assign(device);

            // assign node neihbour
            nodeNeighbors[i].push_back(interface.GetAddress(1));
            nodeNeighbors[left].push_back(interface.GetAddress(0));
            // push node pair
            curNode.push_back(nodePair);
        }

        if (right < numNodes)
        {
            NodeContainer nodePair;
            nodePair.Add(allNodes.Get(i));
            nodePair.Add(allNodes.Get(right));
            NetDeviceContainer device = p2p.Install(nodePair);

            // Assign a unique subnet in the format 10.x.1.0
            std::ostringstream baseIP;
            baseIP << TREE_RIGHT_BASE_IPV4 << ipCounter << BASE_IPV4_ENDING;
            address.SetBase(baseIP.str().c_str(), NETWORK_MASK);
            Ipv4InterfaceContainer interface = address.Assign(device);

            nodeNeighbors[i].push_back(interface.GetAddress(1));
            nodeNeighbors[right].push_back(interface.GetAddress(0));

            curNode.push_back(nodePair);
        }

        // curNode is [node01, node02]
        if (curNode.size() != 0)
        {
            nodeCon.push_back(curNode);
        }

        ipCounter++;
    }

    // Assign to P2PNetwork struct
    net.nodes = allNodes;
    net.nodeNeighbors = nodeNeighbors;

    P2PUtil::PrintNetworkInfo(net);
    return net;
}

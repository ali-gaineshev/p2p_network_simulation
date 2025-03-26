#include "p2p-network-builder.h"

#include "p2p-util.h"

#include "ns3/ipv4.h"
#include "ns3/log.h"

#define TREE_FIRST_OCTET_IPV4 "10."
#define LINEAR_FIRST_OCTET_IPV4 "20."
#define MESH_FIRST_OCTET_IPV4 "30."
#define D_REGULAR_FIRST_OCTET_IPV4 "40."

#define IPV4_LAST_OCTET ".0"
#define NETWORK_MASK "255.255.255.0"

NS_LOG_COMPONENT_DEFINE("NetworkBuilder");

Ipv4Address
generateIpv4Base(uint32_t ipCounter, std::string ipv4FirstOctet)
{
    uint32_t X = (ipCounter / 255) + 1; // Cycle X from 1 to 255
    uint32_t Y = (ipCounter % 255) + 1; // Cycle Y from 1 to 255

    std::ostringstream baseIP; // ipv4

    baseIP << ipv4FirstOctet << X << "." << Y << IPV4_LAST_OCTET;
    return baseIP.str().c_str();
}

// ASSUMPTIONS:
// from testing whenever you create a node, ns3 has an internal counter for node Id
// that means we can create structures seprately and then join them

P2PNetwork
CreateP2PNetwork(NetworkType networkType, uint32_t nodeNum, std::string fileName)
{
    switch (networkType)
    {
    case LINEAR:
        return CreateLinearNetwork(nodeNum);
    case TREE:
        return CreateTreeNetwork(nodeNum);
    case MESH:
        throw std::runtime_error("not implemented");
        // return CreateMeshNetwork(nodeNum, 5, 0.10);
    case REGULAR:
        return CreateRegularGraph(fileName);
    default:
        throw std::runtime_error("not implemented");
    }

}

P2PNetwork
CreateRegularGraph(std::string fileName)
{
    P2PNetwork net;
    // reading file with data
    std::vector<std::vector<int>> adjList = P2PUtil::readGraphFromFile(fileName);
    int numNodes = adjList.size();
    int numNeighbours = adjList[0].size();

    // Create nodes
    NodeContainer nodes;
    nodes.Create(numNodes);
    net.nodes = nodes;

    // Install internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    // Point-to-Point helper for connections
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // IPv4 address helper
    Ipv4AddressHelper address;
    uint32_t ipCounter = 0;

    // Vector to store node neighbors
    std::vector<std::vector<Ipv4Address>> nodeNeighbors(numNodes);
    std::vector<std::vector<int>> visitedNeighbours(numNodes);
    // Create point-to-point links between  nodes
    for (uint32_t i = 0; i < numNodes - 1; i++)
    {
        std::vector<int> visited = visitedNeighbours[i];
        for (int neighbourIndex : adjList[i])
        {
            bool isVisited = false;
            for (int visitedIndex : visited)
            {
                if (visitedIndex == neighbourIndex)
                {
                    isVisited = true;
                    break;
                }
            }
            if (isVisited)
            {
                continue;
            }
            NodeContainer nodePair;
            nodePair.Add(nodes.Get(i));
            nodePair.Add(nodes.Get(neighbourIndex));

            // Install devices
            NetDeviceContainer devices = p2p.Install(nodePair);

            // Assign IPv4 addresses
            Ipv4Address baseIP = generateIpv4Base(ipCounter, D_REGULAR_FIRST_OCTET_IPV4);
            address.SetBase(baseIP, NETWORK_MASK);
            Ipv4InterfaceContainer interfaces = address.Assign(devices);
            if (interfaces.GetAddress(1) == Ipv4Address::GetZero())
            {
                NS_LOG_INFO("EMPTTTTT");
            }
            if (interfaces.GetAddress(0) == Ipv4Address::GetZero())
            {
                NS_LOG_INFO("EMPTTTTT");
            }
            // Store neighbor relationships
            nodeNeighbors[i].push_back(interfaces.GetAddress(1));
            nodeNeighbors[neighbourIndex].push_back(interfaces.GetAddress(0));

            ipCounter++;

            visitedNeighbours[i].push_back(neighbourIndex);
            visitedNeighbours[neighbourIndex].push_back(i);
        }
    }

    // Assign to P2PNetwork struct
    net.nodes = nodes;
    net.nodeNeighbors = nodeNeighbors;

    P2PUtil::PrintNetworkInfo(net);
    return net;
}

// 0: 2 7 4 3 9
// 1: 6 3 5 8 4
// 2: 0 5 8 7 9
// 3: 0 1 4 6 8
// 4: 0 1 3 9 5
// 5: 1 2 4 6 9
// 6: 1 3 5 8 7
// 7: 0 2 6 9 8
// 8: 1 2 3 6 7
// 9: 0 2 4 5 7
P2PNetwork
CreateMeshNetwork(uint32_t numNodes, int maxConnections, double probability)
{
    P2PNetwork net;

    return net;
}

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
    uint32_t ipCounter = 0;

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
        Ipv4Address baseIP = generateIpv4Base(ipCounter, LINEAR_FIRST_OCTET_IPV4);
        address.SetBase(baseIP, NETWORK_MASK);
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

    // install internet stack
    InternetStackHelper internet;
    internet.Install(allNodes);

    // list of node neighbours based on index
    std::vector<std::vector<Ipv4Address>> nodeNeighbors(numNodes);

    // ipv4 address counter. each netdevice will have it's own address
    uint32_t ipCounter = 0;
    Ipv4AddressHelper address;

    // make node connections, devices and chanells
    for (uint32_t i = 0; i < numNodes; i++)
    {
        uint32_t left = i * 2 + 1;
        uint32_t right = i * 2 + 2;

        Ipv4Address baseIP;
        if (left < numNodes)
        {
            // make nodes for index i
            NodeContainer nodePair;
            nodePair.Add(allNodes.Get(i));
            nodePair.Add(allNodes.Get(left));
            // make net device
            NetDeviceContainer device = p2p.Install(nodePair);

            // set IPv4 address
            baseIP = generateIpv4Base(ipCounter, TREE_FIRST_OCTET_IPV4);
            address.SetBase(baseIP, NETWORK_MASK);
            Ipv4InterfaceContainer interface = address.Assign(device);

            // assign node neihbour
            nodeNeighbors[i].push_back(interface.GetAddress(1));
            nodeNeighbors[left].push_back(interface.GetAddress(0));

            ipCounter++;
        }

        if (right < numNodes)
        {
            NodeContainer nodePair;
            nodePair.Add(allNodes.Get(i));
            nodePair.Add(allNodes.Get(right));
            NetDeviceContainer device = p2p.Install(nodePair);

            // Assign a unique subnet in the format 10.x.1.0
            baseIP = generateIpv4Base(ipCounter, TREE_FIRST_OCTET_IPV4);
            address.SetBase(baseIP, NETWORK_MASK);
            Ipv4InterfaceContainer interface = address.Assign(device);

            nodeNeighbors[i].push_back(interface.GetAddress(1));
            nodeNeighbors[right].push_back(interface.GetAddress(0));

            ipCounter++;
        }
    }

    // Assign to P2PNetwork struct
    net.nodes = allNodes;
    net.nodeNeighbors = nodeNeighbors;

    // P2PUtil::PrintNetworkInfo(net);
    return net;
}

/*
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
    */

/*
P2PNetwork
AddNewNodeToExistingNet(P2PNetwork& net, int nodeIndex)
{
int currentNetSize = net.nodes.GetN();

if (nodeIndex < 0 || nodeIndex >= currentNetSize)
{
    throw std::runtime_error("Invalid params in network builder");
}

Ptr<Node> newNode = CreateObject<Node>();
net.nodes.Add(newNode);

// Install internet stack on the new node
InternetStackHelper internet;
internet.Install(newNode);

// Create a point-to-point connection between new node and the specified existing node
PointToPointHelper p2p;
p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
p2p.SetChannelAttribute("Delay", StringValue("2ms"));

NodeContainer nodePair;
nodePair.Add(net.nodes.Get(nodeIndex));
nodePair.Add(newNode);

NetDeviceContainer devices = p2p.Install(nodePair);
// Assign IPv4 address
Ipv4AddressHelper address;
std::ostringstream baseIP;
baseIP << NEW_LINK_BASE_IPV4 << currentNetSize + 1 << ".1.0"; // Unique subnet (hopefully)
address.SetBase(baseIP.str().c_str(), NETWORK_MASK);
Ipv4InterfaceContainer interfaces = address.Assign(devices);

net.nodeNeighbors.resize(net.nodes.GetN());
net.nodeNeighbors[nodeIndex].push_back(interfaces.GetAddress(1));
net.nodeNeighbors[currentNetSize].push_back(interfaces.GetAddress(0));

NS_LOG_INFO("Added new node to the network");
return net;
}
*/
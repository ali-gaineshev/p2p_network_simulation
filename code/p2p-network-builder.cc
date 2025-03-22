#include "p2p-network-builder.h"

#include "p2p-util.h"

#include "ns3/ipv4.h"
#include "ns3/log.h"

P2PNetwork CreateLinearNetwork(uint32_t);
P2PNetwork CreateTreeNetwork(uint32_t);

NS_LOG_COMPONENT_DEFINE("NetworkBuilder");

P2PNetwork
CreateP2PNetwork(NetworkType networkType, uint32_t numNodes)
{
    switch (networkType)
    {
    case LINEAR:
        return CreateLinearNetwork(numNodes);
    case TREE:
        return CreateTreeNetwork(numNodes);
    default:
        NS_LOG_ERROR("Unsupported network type");
        return P2PNetwork();
    }
}

P2PNetwork
CreateLinearNetwork(uint32_t numNodes)
{
    P2PNetwork net;
    return net;
}

P2PNetwork
CreateTreeNetwork(uint32_t numNodes)
{
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
            baseIP << "10." << ipCounter << ".1.0";
            address.SetBase(baseIP.str().c_str(), "255.255.255.0");
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
            baseIP << "20." << ipCounter << ".1.0";
            address.SetBase(baseIP.str().c_str(), "255.255.255.0");
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

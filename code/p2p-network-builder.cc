#include "p2p-network-builder.h"

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
            NS_LOG_INFO(baseIP.str());
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
            NS_LOG_INFO(baseIP.str());
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
}

void
PrintNodeInfo(NodeContainer nodes,
              std::vector<Ipv4Address>& nodeIps,
              std::vector<std::vector<Ipv4Address>>& nodeNeighbors,
              uint32_t idx)
{
    Ptr<Node> node = nodes.Get(idx);
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();

    NS_LOG_INFO("==================================");
    NS_LOG_INFO("Node " << idx);
    NS_LOG_INFO("\t Primary IP: " << nodeIps[idx]);
    NS_LOG_INFO("\t NetDevice Count: " << node->GetNDevices());
    NS_LOG_INFO("\t Neighbor List:");
    for (const auto& neighbor : nodeNeighbors[idx])
    {
        NS_LOG_INFO("\t\t --> " << neighbor);
    }

    NS_LOG_INFO("\t NetDevices and Interfaces:");

    // Iterate over all NetDevices on this node
    for (uint32_t devIdx = 0; devIdx < node->GetNDevices(); ++devIdx)
    {
        Ptr<NetDevice> device = node->GetDevice(devIdx);

        std::string devType = device->GetInstanceTypeId().GetName();

        NS_LOG_INFO("\t Device " << devIdx << ": " << devType);

        // Now look for IP interfaces connected to this NetDevice
        for (uint32_t ifaceIdx = 0; ifaceIdx < ipv4->GetNInterfaces(); ++ifaceIdx)
        {
            // Check if this interface is associated with the current NetDevice
            if (ipv4->GetNetDevice(ifaceIdx) == device)
            {
                NS_LOG_INFO("\t\t Ipv4Interface " << ifaceIdx);
                uint32_t numAddresses = ipv4->GetNAddresses(ifaceIdx);
                for (uint32_t addrIdx = 0; addrIdx < numAddresses; ++addrIdx)
                {
                    Ipv4InterfaceAddress ifaceAddr = ipv4->GetAddress(ifaceIdx, addrIdx);
                    NS_LOG_INFO("\t\t\t Address " << addrIdx << ": " << ifaceAddr.GetLocal());
                }
            }
        }
    }
}
// header for application (P2PApplication)

// header gaurds avoids multiple inclusions
#ifndef P2P_APPLICATION_H
#define P2P_APPLICATION_H

#include "p2p-packet.h"

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/core-module.h"
#include "ns3/event-id.h"
#include "ns3/socket.h"

#include <map>
#include <vector>

namespace ns3
{
// class declaration (inherits from ns3::Application)
class P2PApplication : public Application
{
  private:
    std::vector<Ptr<Socket>> m_sockets;
    std::vector<Ipv4Address> m_ipv4Addresses;
    std::vector<Ipv4Address> m_neighbours;
    std::map<uint32_t, Ipv4Address> m_queryCache;
    uint32_t messageIdCount;
    uint32_t m_port;
    uint32_t GenerateMessageId();
    void SendQueryResponse(uint32_t queryId, Ipv4Address requester);
    void ProcessQuery(std::string query, Ipv4Address sender);

  public:
    // public to construct/destruct
    P2PApplication();
    P2PApplication(uint16_t port);

    virtual ~P2PApplication();
    static TypeId GetTypeId();
    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SetPeers(std::vector<Ipv4Address> neighbours);
    void SetAddresses();
    void SendPacket(MessageType type,
                    Ipv4Address dest,
                    uint8_t ttl,
                    Ipv4Address curaddy,
                    uint32_t sinkn,
                    int neighbourIndex);
    void RecievePacket(Ptr<Socket> socket);
    void Flood(uint32_t sinknode);
    void FloodButNotAll(P2PPacket p2pPacket, int excludeIndex);
    void QueryHit(P2PPacket ppacket, int lastHopIndex);
    void ForwardQueryHit(P2PPacket ppacket, int lastHopIndex);
    void SendPing();
    void AddReverseRoute(Ipv4Address nextHop);
    int GetNeighbourIndexFromNeighbourIP(Ipv4Address senderIP);
    // DEBUG
    void LogNodeInfo();
    void PrintSocketBindings();
    void PrintAddresses();
};
} // namespace ns3

#endif

#ifndef P2P_APPLICATION_H
#define P2P_APPLICATION_H

#include "p2p-packet.h"

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/core-module.h"
#include "ns3/event-id.h"
#include "ns3/socket.h"

#include <map>
#include <vector>

namespace ns3
{

class P2PApplication : public Application
{
  private:
    std::vector<Ptr<Socket>> m_sockets;
    std::vector<Ipv4Address> m_ipv4Addresses;
    std::vector<Ipv4Address> m_neighbors;
    uint32_t m_messageIdCounter;
    uint16_t m_port;

    uint32_t GenerateMessageId();
    int GetNeighborIndex(Ipv4Address senderIP);

  public:
    P2PApplication();
    P2PApplication(uint16_t port);
    virtual ~P2PApplication();

    static TypeId GetTypeId();
    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SetNeighbors(std::vector<Ipv4Address> neighbors);
    void SetAddresses();
    void SendPacket(MessageType type,
                    Ipv4Address dest,
                    uint8_t ttl,
                    Ipv4Address curIP,
                    uint32_t sinkNode,
                    int neighborIndex);
    void ReceivePacket(Ptr<Socket> socket);
    void Flood(uint32_t sinkNode);
    void FloodExcept(P2PPacket p2pPacket, int excludeIndex);
    void ForwardQueryHit(P2PPacket ppacket, int lastHopIndex);

    void LogNodeInfo();
};

} // namespace ns3

#endif // P2P_APPLICATION_H
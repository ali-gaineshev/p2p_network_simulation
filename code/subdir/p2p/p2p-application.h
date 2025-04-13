// header for application (P2PApplication)

// header gaurds avoids multiple inclusions
#ifndef P2P_APPLICATION_H
#define P2P_APPLICATION_H

#include "p2p-packet.h"
#include "p2p-stats.h"

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/core-module.h"
#include "ns3/event-id.h"
#include "ns3/socket.h"

#include <map>
#include <vector>

enum SearchAlgorithm
{
    FLOOD,
    RANDOM_WALK,
    NORMALIZED_FLOOD
};

struct QUERY_CACHE
{
    bool initialized;
    uint32_t msgId;
    uint8_t ttl;
};

namespace ns3

{
// class declaration (inherits from ns3::Application)
class P2PApplication : public Application
{
  private:
    // application variables
    std::vector<Ptr<Socket>> m_sockets;
    std::vector<Ipv4Address> m_ipv4Addresses;
    std::vector<Ipv4Address> m_neighbours;
    QUERY_CACHE m_queryCache;

    // application functionality
    bool m_isDisabled = false;
    uint32_t m_ttlIncrease;
    // statistics
    bool m_queryHit = false;
    P2PStats stats;

    // retry events and variables needed
    EventId m_retryEvent;
    uint32_t m_currentRetry = 0;
    SearchAlgorithm m_currentSearchAlgorithm;
    uint8_t m_currentSinknode;
    uint8_t m_currentTtl;
    int m_currentWalkers;

    uint32_t messageIdCount;
    uint32_t m_port;

  public:
    // public to construct/destruct
    P2PApplication();
    P2PApplication(uint16_t port);

    virtual ~P2PApplication();
    static TypeId GetTypeId();
    virtual void StartApplication() override;
    virtual void StopApplication() override;

    // statistics
    bool IsSinkNode();
    bool IsSrcNode();
    bool IsDisabledNode();
    int GetQueryHits();
    std::vector<int> GetHopsForQueryHits();
    std::vector<double> GetSecondsForQueryHits();
    int GetSentRequests();
    int GetReceivedRequests();
    int GetForwardedQueryHits();
    int GetTriedRequests();
    int GetInitializedRequests();
    void SetSinkNode();
    void SetSrcNode();

    // Searching and Scheduling
    void ScheduleSearchWithRetry(SearchAlgorithm searchAlgorithm,
                                 uint32_t sinknode,
                                 uint32_t ttl,
                                 int walkers);

    void CheckAndRetrySearch();

    // RECEIVING
    void RecievePacket(Ptr<Socket> socket);

    // Sending from SRC
    void SendPacketFromSrc(uint32_t msgid,
                           MessageType type,
                           Ipv4Address dest,
                           uint8_t ttl,
                           Ipv4Address curaddy,
                           uint32_t sinkn,
                           int neighbourIndex);
    // FLOODING
    void InitialFlood(uint32_t sinknode, uint32_t ttl, uint32_t msgid);
    void FloodExceptSender(P2PPacket p2pPacket, int excludeIndex);
    // NORMALIZED FLOOD
    void InitialNormalizedFlood(uint32_t sinknode, uint32_t ttl, int howManyNodes, uint32_t msgid);
    void NormalizedFloodExceptSender(P2PPacket p2pPacket, int excludeIndex, int howManyNodes);
    // RANDOM WALKS
    void InitialRandomWalk(uint32_t sinknode, uint32_t ttl, int k, uint32_t msgid);
    void RandomWalkExceptSender(P2PPacket p2pPacket, int excludeIndex);

    // BACK TRACKING
    void ForwardQueryHit(P2PPacket ppacket, int lastHopIndex);

    // UTIL
    int GetNeighbourIndexFromNeighbourIP(Ipv4Address senderIP);
    bool IsCurrentNodeInPath(std::vector<ns3::Ipv4Address> path);
    bool DoesIPv4BelongToCurrentNode(Ipv4Address IPv4toCheck);
    // SETTERS
    void SetPeers(std::vector<Ipv4Address> neighbours);
    void SetAddresses();
    void SetDisableNode(bool m_isDisabled);
    void SetTTLIncrease(uint32_t ttlIncrease);

    // GETTERS
    const bool GetQueryHit() const;
    const std::vector<Ptr<Socket>>& GetSockets() const;
    const std::vector<Ipv4Address>& GetIpv4Addresses() const;
    const std::vector<Ipv4Address>& GetNeighbours() const;

    // DEBUG
    void LogNodeInfo();
    void LogSocketBindings();
    void LogAddresses();
};
} // namespace ns3

#endif
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
    // at index i
    //
    std::vector<Ptr<Socket>> m_sockets;
    std::vector<Ipv4Address> m_ipv4Addresses; // its node's  a
    std::vector<Ipv4Address> m_neighbours;    //
    std::map<uint32_t, Ipv4Address> m_queryCache;

    bool m_isDisabled = false;

    bool m_queryHit = false;
    EventId m_retryEvent;
    void RetryFlood(uint32_t sinknode);

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

    // RECEIVING
    void RecievePacket(Ptr<Socket> socket);

    // Sending from SRC
    void SendPacketFromSrc(MessageType type,
                           Ipv4Address dest,
                           uint8_t ttl,
                           Ipv4Address curaddy,
                           uint32_t sinkn,
                           int neighbourIndex);
    // FLOODING
    void InitialFlood(uint32_t sinknode);
    void FloodExceptSender(P2PPacket p2pPacket, int excludeIndex);
    // NORMALIZED FLOOD
    void InitialNormalizedFlood(uint32_t sinknode, int howManyNodes);
    void NormalizedFloodExceptSender(P2PPacket p2pPacket, int excludeIndex, int howManyNodes);
    // RANDOM WALKS
    void InitialRandomWalk(uint32_t sinknode, int k);
    void RandomWalkExceptSender(P2PPacket p2pPacket, int excludeIndex);

    // BACK TRACKING
    void ForwardQueryHit(P2PPacket ppacket, int lastHopIndex);

    // UTIL
    int GetNeighbourIndexFromNeighbourIP(Ipv4Address senderIP);
    // SETTERS
    void SetPeers(std::vector<Ipv4Address> neighbours);
    void SetAddresses();
    void SetDisableNode(bool m_isDisabled);

    // GETTERS
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
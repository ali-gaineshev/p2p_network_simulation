// custom packet header for p2p (gnutella like)

#ifndef P2P_PACKET_H
#define P2P_PACKET_H

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"

#include <vector>

namespace ns3
{

// basic types for now can edit later
enum MessageType
{
    PING,
    PONG,
    QUERY,
    QUERY_HIT,
    QUERY_RW,
    QUERY_NF
};

class P2PPacket : public Header
{
  private:
    uint32_t descriptorId;
    MessageType payloadDescriptor;
    Ipv4Address senderIp;
    uint8_t m_ttl;
    uint8_t m_messageType;
    uint8_t default_ttl = 5;
    uint8_t hops;
    Ipv4Address destIp;
    uint32_t sinkNode;
    std::vector<Ipv4Address> path; // store path instead of prev hop

  public:
    P2PPacket();
    P2PPacket(MessageType,
              uint32_t msgId,
              Ipv4Address source,
              Ipv4Address dest,
              uint8_t ttl,
              uint8_t hop,
              uint32_t sinknode,
              std::vector<Ipv4Address> pathHistory);

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;

    // serialization stuff
    virtual void Serialize(Buffer::Iterator start) const override;
    virtual uint32_t Deserialize(Buffer::Iterator start) override;
    virtual uint32_t GetSerializedSize() const override;

    // genera functions
    void AddToPath(Ipv4Address ip);
    void RemoveLastHop();
    void DecrementTtl();
    uint32_t GenerateMessageId();
    bool IsPathEmpty() const;

    void IncrementHops();
    uint32_t getPathSize() const;

    // get
    MessageType GetMessageType() const;
    uint32_t GetMessageId() const;
    Ipv4Address GetSenderIp() const;
    Ipv4Address GetDestIp() const;
    uint8_t GetTtl() const;
    uint8_t GetHops() const;
    uint32_t GetSinkNode() const;
    Ipv4Address GetLastHop() const;
    std::vector<Ipv4Address> GetPath() const;

    // set
    void SetMessageType(MessageType type);
    void SetMessagesId(uint32_t id);
    void SetSenderIp(Ipv4Address ip);
    void SetDestIp(Ipv4Address d);
    void SetTtl(uint8_t ttl);
    void SetHops(uint8_t hop);
    void SetSinkNode(uint32_t nodeId);

    // change for debuging stuff
    virtual void Print(std::ostream& os) const override;
    void PrintPath() const;
};
} // namespace ns3

#endif
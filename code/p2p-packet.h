// custom packet header for p2p (gnutella like)

#ifndef P2P_PACKET_H
#define P2P_PACKET_H

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"

namespace ns3 {

// basic types for now can edit later
enum MessageType {PING, PONG, QUERY, QUERY_HIT };

class P2PPacket : public Header {

private:
    uint32_t descriptorId;
    MessageType payloadDescriptor;
    Ipv4Address senderIp;
    uint8_t m_ttl;
    uint8_t m_messageType;
    uint8_t default_ttl = 5;
    uint8_t hops;
    Ipv4Address previousHop;
    Ipv4Address destIp;

public:
    P2PPacket();
    P2PPacket(MessageType, uint32_t msgId, Ipv4Address source, Ipv4Address dest, uint8_t ttl , uint8_t hop, Ipv4Address prevhop);

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const override;

    // serialization stuff 
    virtual void Serialize(Buffer::Iterator start) const override;
    virtual uint32_t Deserialize(Buffer::Iterator start) override;
    virtual uint32_t GetSerializedSize() const override;

    // get set
    MessageType GetMessageType() const;
    uint32_t GetMessageId() const;
    Ipv4Address GetSenderIp() const;
    Ipv4Address GetDestIp() const;
    uint8_t GetTtl() const;
    uint8_t GetHops() const;
    Ipv4Address GetPrevHop() const;
    void DecrementTtl();
    uint32_t GenerateMessageId();
    void SetMessageType(MessageType type);
    void SetMessagesId(uint32_t id);
    void SetSenderIp(Ipv4Address ip);
    void SetDestIp(Ipv4Address d);
    void SetPrevHop(Ipv4Address prev);
    void SetTtl(uint8_t ttl);
    void SetHops(uint8_t hop);

    // change for debuging stuff
    virtual void Print(std::ostream &os) const override;

};
}

#endif

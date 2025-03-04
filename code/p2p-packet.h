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
    uint32_t m_messageId;
    MessageType m_type;
    Ipv4Address m_senderIp;
    uint8_t m_ttl;
    uint8_t m_messageType;

public:
    P2PPacket();
    P2PPacket(MessageType, uint32_t msgId, Ipv4Address sender, uint8_t ttl);

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
    uint8_t GetTtl() const;
    void SetMessageType(MessageType type);
    void SetMessagesId(uint32_t id);
    void SetSenderIp(Ipv4Address ip);
    void SetTtl(uint8_t ttl);

    // change for debuging stuff
    virtual void Print(std::ostream &os) const override;

};
}

#endif

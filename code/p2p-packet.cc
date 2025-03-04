// packet class and behaviour

#include "p2p-packet.h"
#include "ns3/packet.h"
#include "ns3/header.h"
#include "ns3/log.h"
#include "ns3/address-utils.h"

// logging/debugging purposes
NS_LOG_COMPONENT_DEFINE("P2PPacket");


namespace ns3 {

    NS_OBJECT_ENSURE_REGISTERED(P2PPacket);
    P2PPacket packet;
    
    //ig you have to register packet header wiht the ns3 software
    //NS_OBJECT_ENSURE_REGISTERED(P2PPacket);


// defualy to 0 for build purposes also w parameters ill be changin this once it builds 
    P2PPacket::P2PPacket(): m_messageId(0), m_type(PING), m_senderIp(Ipv4Address("0.0.0.0")), m_ttl(0) { }
    P2PPacket::P2PPacket(MessageType type, uint32_t msgId, Ipv4Address sender, uint8_t ttl)
        : m_messageId(msgId), m_type(type), m_senderIp(sender), m_ttl(ttl) { }


    TypeId P2PPacket::GetTypeId() {
        static TypeId tid = TypeId("ns3::P2PPacket")
            .SetParent<Header>()
            .AddConstructor<P2PPacket>();
        return tid;

    }
    TypeId P2PPacket::GetInstanceTypeId() const {
        return GetTypeId();
    }

    // serialization stuff
    // (m_messageType = Ping pong query ..)
    void P2PPacket::Serialize(Buffer::Iterator start) const {
        start.WriteU8(static_cast<uint8_t>(m_type));
        start.WriteU32(m_messageId);
        WriteTo(start, m_senderIp);
        start.WriteU8(m_ttl);
    }
    uint32_t P2PPacket::Deserialize(Buffer::Iterator start) {
        m_type = static_cast<MessageType>(start.ReadU8());
        m_messageId = start.ReadU32();
        ReadFrom(start, m_senderIp);
        m_ttl = start.ReadU8();
        return GetSerializedSize();
    }
    uint32_t P2PPacket::GetSerializedSize() const {
        return 1 + 4 + 4 + 1;
    }

    void P2PPacket::Print(std::ostream &os) const {
        os << "P2PPacket: Type=" << m_type 
            << ", MessageId=" << m_messageId 
            << ", SenderIP=" << m_senderIp 
            << ", TTL=" << static_cast<uint32_t>(m_ttl);
}

    // Getter methods
    MessageType P2PPacket::GetMessageType() const { return m_type; }
    uint32_t P2PPacket::GetMessageId() const { return m_messageId; }
    Ipv4Address P2PPacket::GetSenderIp() const { return m_senderIp; }
    uint8_t P2PPacket::GetTtl() const { return m_ttl; }

    // Setter methods
    void P2PPacket::SetMessageType(MessageType type) { m_type = type; }
    void P2PPacket::SetMessagesId(uint32_t id) { m_messageId = id; }
    void P2PPacket::SetSenderIp(Ipv4Address ip) { m_senderIp = ip; }
    void P2PPacket::SetTtl(uint8_t ttl) { m_ttl = ttl; }

}
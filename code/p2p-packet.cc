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


    // MODIFIED THIS TO FIT THE ACTUAL PACKETS IN GNUTELLA https://rfc-gnutella.sourceforge.net/developer/stable/ also see the mt update
    P2PPacket::P2PPacket(): descriptorId(0), payloadDescriptor(PING), senderIp(Ipv4Address("0.0.0.0"))
    , destIp(Ipv4Address("0.0.0.0")), m_ttl(0), hops(0), sinkNode(-1) { } //uint32_t sinkNode(0)
    P2PPacket::P2PPacket(MessageType type, uint32_t msgId, Ipv4Address sender, Ipv4Address dest, uint8_t ttl, uint8_t hop, uint32_t sinknode, std::vector<Ipv4Address> pathHistory = {})
        : descriptorId(msgId), payloadDescriptor(type), senderIp(sender), destIp(dest), m_ttl(ttl), hops(hop), sinkNode(sinknode), path(std::move(pathHistory)) {}


    TypeId P2PPacket::GetTypeId() {
        static TypeId tid = TypeId("ns3::P2PPacket")
            .SetParent<Header>()
            .AddConstructor<P2PPacket>();
        return tid;

    }
    TypeId P2PPacket::GetInstanceTypeId() const {
        return GetTypeId();
    }

    // decrements the ttl of the package and increases the hop count 
    void P2PPacket::DecrementTtl() {
        if (m_ttl > 0) {
            m_ttl --;
            hops ++;
        }
    }

    // serialization stuff - making the contents of the package able to transfer 
    // (m_messageType = Ping pong query ..)
    void P2PPacket::Serialize(Buffer::Iterator start) const {
    start.WriteU8(static_cast<uint8_t>(payloadDescriptor));  // Write message type
    start.WriteU32(descriptorId);  // Write descriptor ID
    WriteTo(start, senderIp);  // Write sender IP
    WriteTo(start, destIp);  // Write destination IP
    start.WriteU32(sinkNode);  // Write sink node
    start.WriteU8(m_ttl);  // Write TTL
    start.WriteU8(hops);  // Write hops

    start.WriteU8(path.size());  // **Write path size (BEFORE writing addresses!)**
    for (const auto& ip : path) {
        WriteTo(start, ip);
    }
}
    
    // uint32_t P2PPacket::Deserialize(Buffer::Iterator start) {
    //     payloadDescriptor = static_cast<MessageType>(start.ReadU8());  // Read message type
    //     descriptorId = start.ReadU32();  // Read descriptor ID
    //     ReadFrom(start, senderIp);  // Read sender IP
    //     ReadFrom(start, destIp);  // Read destination IP
    //     sinkNode = start.ReadU32();
    //     m_ttl = start.ReadU8();  // Read TTL
    //     hops = start.ReadU8();  // Read hops
    //     uint8_t pathSize = start.ReadU8();
    //     path.clear();
    //     for (uint8_t i = 0; i < pathSize; i++) {
    //         Ipv4Address hop;
    //         ReadFrom(start, hop);
    //         path.push_back(hop);
    //     }
    //     return GetSerializedSize();
    // }
    uint32_t P2PPacket::Deserialize(Buffer::Iterator start) {
    payloadDescriptor = static_cast<MessageType>(start.ReadU8());  // Read message type
    descriptorId = start.ReadU32();  // Read descriptor ID
    ReadFrom(start, senderIp);  // Read sender IP
    ReadFrom(start, destIp);  // Read destination IP
    sinkNode = start.ReadU32();  // Read sink node
    m_ttl = start.ReadU8();  // Read TTL
    hops = start.ReadU8();  // Read hops

    uint8_t pathSize = start.ReadU8();  // **Read path size BEFORE reading addresses**
    if (pathSize > 20) { // Arbitrary max path length check
        NS_LOG_ERROR("Corrupt packet: pathSize=" << (int)pathSize);
        return 0;  // Drop packet
    }
    path.clear();
    for (uint8_t i = 0; i < pathSize; i++) {
        Ipv4Address hop;
        ReadFrom(start, hop);
        path.push_back(hop);
    }

    return GetSerializedSize();
}

    uint32_t P2PPacket::GetSerializedSize() const {
        return 1 + 4 + 4 + 4 + 4 + 1 + 1 + 1 + (path.size() * 4);// payload descriptor + descriptorid + senderip + destip + ttl + hops + pathsize + sinknode
    }
    // used for testing the packages contents
    void P2PPacket::Print(std::ostream &os) const {
        os << "P2PPacket: Type=" << payloadDescriptor
       << ", MessageId=" << descriptorId
       << ", SenderIP=" << senderIp
       << ", DestIP=" << destIp
       << ", SinkNode=" << sinkNode
       << ", TTL=" << static_cast<uint32_t>(m_ttl)
       << ", Hops=" << static_cast<uint32_t>(hops)
       << ", Path=[";
        for (const auto& hop : path) {
            os << hop << " ";
        }
        os << "]";
    }   

    void P2PPacket::AddToPath(Ipv4Address ip) { path.push_back(ip); }
    void P2PPacket::RemoveLastHop() { if (!path.empty()) path.pop_back(); }
    bool P2PPacket::IsPathEmpty() const { return path.empty(); }

    // Getter methods
    MessageType P2PPacket::GetMessageType() const { return payloadDescriptor; }
    uint32_t P2PPacket::GetMessageId() const { return descriptorId; }
    Ipv4Address P2PPacket::GetSenderIp() const { return senderIp; }
    Ipv4Address P2PPacket::GetDestIp() const { return destIp; }
    //Ipv4Address P2PPacket::GetPrevHop() const { return previousHop; }
    uint8_t P2PPacket::GetTtl() const { return m_ttl; }
    uint8_t P2PPacket::GetHops() const { return hops; }
    uint32_t P2PPacket::GetSinkNode() const { return sinkNode; }
    Ipv4Address P2PPacket::GetLastHop() const { return path.empty() ? Ipv4Address("0.0.0.0") : path.back(); }

    // Setter methods
    void P2PPacket::SetMessageType(MessageType type) { payloadDescriptor = type; }
    void P2PPacket::SetMessagesId(uint32_t id) { descriptorId = id; }
    void P2PPacket::SetSenderIp(Ipv4Address ip) { senderIp = ip; }
    void P2PPacket::SetDestIp(Ipv4Address d) { destIp = d; }
    //void P2PPacket::SetPrevHop(Ipv4Address prev) { previousHop = prev; }
    void P2PPacket::SetTtl(uint8_t ttl) { m_ttl = ttl; }
    void P2PPacket::SetHops(uint8_t hop) { hops = hop; }
    void P2PPacket::SetSinkNode(uint32_t nodeId) { sinkNode = nodeId; }

}
// header for application (P2PApplication)


// header gaurds avoids multiple inclusions 
#ifndef P2P_APPLICATION_H
#define P2P_APPLICATION_H

#include "ns3/core-module.h"
#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "p2p-packet.h"

#include <vector>
#include <map>

namespace ns3 {
// class declaration (inherits from ns3::Application)
class P2PApplication :  public Application {
public:
    // public to construct/destruct
    // TODO EDIT THESE AS METHODS AND FUCNTIONS ARE ADDED 
    
    P2PApplication();
    P2PApplication(uint16_t port);
    
    virtual ~P2PApplication();
    static TypeId GetTypeId();
    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SetPeers(std::vector<Ipv4Address> neighbours);
    void SendPacket(MessageType type, Ipv4Address dest, uint8_t ttl, Ipv4Address curaddy);
    void RecievePacket(Ptr<Socket> socket);
    void SendQuery(std::string filename);
    void SendPing();
    // void HandleQuery(Ptr<Socket> socket);


private:
    Ptr<Socket> m_socket;
    Ipv4Address m_myAddress;
    std::vector<Ipv4Address> m_neighbours;
    std::map<uint32_t, Ipv4Address> m_queryCache;
    //std::unordered_set<uint32_t> processedPackets;
    uint32_t messageIdCount;
    uint32_t m_port;
    uint32_t GenerateMessageId();
    void SendQueryResponse(uint32_t queryId, Ipv4Address requester);
    void ProcessQuery(std::string query, Ipv4Address sender);
};
}

#endif
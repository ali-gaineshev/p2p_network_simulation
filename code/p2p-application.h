// header for application (P2PApplication)


// header gaurds avoids multiple inclusions 
#ifndef P2P_APPLICATION_H
#define P2P_APPLICATION_H

#include "ns3/core-module.h"
#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/event-id.h"

#include <vector>
#include <map>

namespace ns3 {
// class declaration (inherits from ns3::Application)
class P2PApplication :  public Application {
public:
    // public to construct/destruct
    // TODO EDIT THESE AS METHODS AND FUCNTIONS ARE ADDED 
    P2PApplication();
    virtual ~P2PApplication();
    static TypeId GetTypeId();

    void SetPeers(std::vector<Ipv4Address> neighbours);
    void SendQuery(std::string filename);
    void HandleQuery(Ptr<Socket> socket);

protected:
    virtual void StartApplication() override;
    virtual void StopApplication() override;

private:
    Ptr<Socket> m_socket;
    Ipv4Address m_myAddress;
    std::vector<Ipv4Address> m_neighbours;
    std::map<uint32_t, Ipv4Address> m_queryCache;
    void SendQueryResponse(uint32_t queryId, Ipv4Address requester);
    void ProcessQuery(std::string query, Ipv4Address sender);
};
}

#endif
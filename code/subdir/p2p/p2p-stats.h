#ifndef P2P_STATS_H
#define P2P_STATS_H

#include <vector>

class P2PStats
{
  public:
    // Constructors and destructor
    P2PStats();
    ~P2PStats();

    // Setters
    void SetIsSinkNode(bool isSink);
    void SetIsSrcNode(bool isSrc);

    // Increment methods
    void IncrementReceivedRequests();
    void IncrementInitializedRequests();
    void IncrementSentRequests();
    void IncrementForwardedQueryHits();
    void IncrementTriedRequests();

    void IncrementQueryHits();
    void AddHopsForQueryHit(int hops);
    void AddSecondsForQueryHit(double seconds);

    // Getters
    bool GetIsSinkNode() const;
    bool GetIsSrcNode() const;
    std::vector<int> GetHopsForQueryHits() const;
    std::vector<double> GetSecondsForQueryHits() const;
    int GetQueryHits() const;
    int GetTriedRequests() const;
    int GetInitializedRequests() const;
    int GetReceivedRequests() const;
    int GetSentRequests() const;
    int GetForwardedQueryHits() const;

  private:
    // Member variables
    bool isSinkNode;
    bool isSrcNode;

    // sending
    int triedRequests;       // retries for src
    int initializedRequests; // started initial searches
    int sentRequests;        // sent requests (intermediate nodes)
    int forwardedQueryHits;  // forwarded query hits

    // receiving
    int receivedRequests; // any received requests

    // query hits
    int n_queryHits;                         // number of query hits
    std::vector<int> hopsForQueryHits;       // hops for query hits
    std::vector<double> secondsForQueryHits; // seconds for query hits
};

#endif // P2P_STATS_H
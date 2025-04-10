#include "p2p-stats.h"

// Constructor implementation
P2PStats::P2PStats()
    : isSinkNode(false),
      isSrcNode(false),
      receivedRequests(0),
      sentRequests(0),
      forwardedQueryHits(0),
      triedRequests(0),
      initializedRequests(0)
{
}

P2PStats::~P2PStats()
{
}

// Setter implementations
void
P2PStats::SetIsSinkNode(bool isSink)
{
    isSinkNode = isSink;
}

void
P2PStats::SetIsSrcNode(bool isSrc)
{
    isSrcNode = isSrc;
}

// Increment method implementations

void
P2PStats::AddHopsForQueryHit(int hops)
{
    hopsForQueryHits.push_back(hops);
}

void
P2PStats::AddSecondsForQueryHit(double seconds)
{
    secondsForQueryHits.push_back(seconds);
}

void
P2PStats::IncrementQueryHits()
{
    n_queryHits++;
}

void
P2PStats::IncrementTriedRequests()
{
    triedRequests++;
}

void
P2PStats::IncrementReceivedRequests()
{
    receivedRequests++;
}

void
P2PStats::IncrementSentRequests()
{
    sentRequests++;
}

void
P2PStats::IncrementForwardedQueryHits()
{
    forwardedQueryHits++;
}

void
P2PStats::IncrementInitializedRequests()
{
    initializedRequests++;
}

// Getter implementations
int
P2PStats::GetQueryHits() const
{
    return n_queryHits;
}

std::vector<int>
P2PStats::GetHopsForQueryHits() const
{
    return hopsForQueryHits;
}

std::vector<double>
P2PStats::GetSecondsForQueryHits() const
{
    return secondsForQueryHits;
}

int
P2PStats::GetTriedRequests() const
{
    return triedRequests;
}

int
P2PStats::GetInitializedRequests() const
{
    return initializedRequests;
}

int
P2PStats::GetReceivedRequests() const
{
    return receivedRequests;
}

int
P2PStats::GetSentRequests() const
{
    return sentRequests;
}

int
P2PStats::GetForwardedQueryHits() const
{
    return forwardedQueryHits;
}

bool
P2PStats::GetIsSinkNode() const
{
    return isSinkNode;
}

bool
P2PStats::GetIsSrcNode() const
{
    return isSrcNode;
}
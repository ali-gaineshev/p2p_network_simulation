// P2PApplication behaviour (where we will be basically building the gnutella like behaviour)

// need this to build
int main(int argc, char *argv[]) {
    return 0;
}
// PEER INTIALIZATION (each node registers itself upon sim start, store neighbour info in local vector)
//  start running as a p2p peer
//  maintiain a list of connected neighbours
//  assign each peer an id and ip address


// QUERY PROPAGATION 
//  A peer should be able to search for a file by sending a query message.
//  Queries can use Flooding, Random Walk, or Expanding Ring techniques.
//  Each query contains a unique ID to prevent loops.
//  Peers forward queries based on the selected technique.

// sendQuery() function
// QueryMessage -> queryId, sourcePeerID, TTL, requestFileName

// QUERY RESPONSE HANDLING
//  If a peer has the requested file, it should respond to the requester.
//  Response should include -> peerID, fileLocation (backwards along query path)

// HandleQuery() -> check if peer has file, if found then send queryHit, else forward toward requester


// QUERY CACHING
//  peers cache results to reduce redundant searches (maybe)

// cache table usign map 

// NEIGHBOUR DISCOVERY AND PEER CONNECTINOS
//  maintain dynamic list of neighbours
//  send keep-alive messages to detect dead peers 
//  randomly discover new peers, peers send heartbeat messages to keep cache alive 

//  ping()

//  pong()

// HANDLING PER JOIN AND LEAVES

//  peer join -> find neighbours and accept queries
// JoinNetwork() registers peer

//  peer leaves -> neighbours update connection list
//  LeaveNetwork() removes peer 

// LOGGING AND STATS
//  number of q sent and recieved
//  response time
//  traffic 


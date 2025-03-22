#ifndef P2P_UTIL_H
#define P2P_UTIL_H

#include "p2p-application.h"
#include "p2p-network-builder.h"

#include "ns3/animation-interface.h"

namespace ns3
{

class P2PApplication; // Forward declaration

class P2PUtil
{
  public:
    static void PositionTreeNodes(uint32_t nodeIndex,
                                  double x,
                                  double y,
                                  double xOffset,
                                  double yOffset,
                                  AnimationInterface& anim,
                                  NodeContainer& nodes);
    static void PrintNodeInfo(Ptr<P2PApplication> app);
    static void PrintSocketBindings(Ptr<P2PApplication> app);
    static void PrintAddresses(Ptr<P2PApplication> app);
    static void PrintNetworkInfo(const P2PNetwork& net);
};

} // namespace ns3

#endif // P2P_UTIL_H

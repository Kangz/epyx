/**
 * @file dht/peer.h
 */
#ifndef EPYX_DHT_PEER_H
#define EPYX_DHT_PEER_H

#include "../n2np/nodeid.h"
#include "id.h"
namespace Epyx
{
namespace DHT
{
    class Peer
    {
    public:
        Id id;
        N2NP::NodeId n2npId;
        time_t lastReceiveTime;

        Peer();
        Peer(const Id& id, const N2NP::NodeId& n2npId, time_t lastReceiveTime);
        Peer(const N2NP::NodeId& n2npId);

        void serialize(std::ostream& o);
        void unserialize(std::istream& i);
    };

}
}
#endif /*EPYX_DHT_PEER_H*/

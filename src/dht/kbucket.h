/**
 * @file dht/kbucket.h
 * @brief defines the KBucket : the DHT routing table
 */

#ifndef EPYX_DHT_KBUCKET_H
#define EPYX_DHT_KBUCKET_H

#include <list>
#include <vector>
#include <map>
#include "../n2np/nodeid.h"
#include "id.h"
#include "peer.h"
#include "../core/common.h"

namespace Epyx
{
namespace DHT
{
    class Bucket;

    /**
     * @class KBucket
     * @brief A KBucket is a kind of routing table for the DHT
     */
    class KBucket
    {
    public:
        /**
         * @brief Constructs the KBucket given the node's Id
         * @param self the Id of our node
         */
        KBucket(const Id& self);

        /**
         * @brief tells the KBucket we've seen someone (to update the routing table)
         * @param peerId the DHT Id of this peer
         * @param n2npId the N2NP Id of this peer
         */
        void seenPeer(const Id& peerId, const N2NP::NodeId& n2npId);

        /**
         * @brief finds the nodes closest to an Id
         * @param id the Id we wat to getto
         * @param nearest where the results will be stored (maybe make it a result instead of an argument)
         * @param n the number of peers we want to get
         */
        void findNearestNodes(const Id& id, std::vector<Peer> &nearest, int n);

        static const unsigned int NODES_PER_BUCKET = 20;
        static const unsigned int MAX_INACTIVE_PERIOD = 15000;

    private:
        Mutex lock;
        std::vector<Bucket> buckets;
        Id ownId;
    };

    //Used for the internal representation of the KBucket
    class Bucket
    {
        public:
            std::list<Peer*> peers;
    };
}
}

#endif /* EPYX_DHT_KBUCKET_H */

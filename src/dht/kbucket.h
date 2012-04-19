#ifndef EPYX_DHT_KBUCKET_H
#define EPYX_DHT_KBUCKET_H

#include <list>
#include <vector>
#include <map>
#include "../n2np/nodeid.h"
#include "id.h"
#include "peer.h"
#include "../core/common.h"

namespace Epyx{
namespace DHT {
    class Bucket;

    class KBucket {
        public:
            KBucket(Id* const self);
            void seenPeer(Id* const peerId, N2NP::NodeId& n2npId);
            void findNearestNodes(const Id& id, std::vector<Peer> &nearest, int n);

        private:
            Mutex lock;
            std::vector<Bucket> buckets;
            Id ownId;
    };

    class Bucket{
        public:
            std::list<Peer*> peers;
    };
}
}

#endif /* EPYX_DHT_KBUCKET_H */

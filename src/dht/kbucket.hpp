#ifndef EPYX_DHT_KBUCKET_H
#define EPYX_DHT_KBUCKET_H

#include <list>
#include <vector>
#include <map>
#include "id.h"

namespace Epyx{
namespace DHT {
    class Peer;

    class KBucket {
        public:
            KBucket(Id* const self);
            static int findBucket(Distance* const d);
            void add_peer(Id* const peerId);
            void find_nearest_nodes(Id* const id,std::multimap<Distance,Id> &nearest,int n);

        private:
            std::vector<Bucket> buckets;
            Id ownId;
    }

    class Peer{
        public:
            Id peerId;
            time_t lastReceiveTime;
    };


    class Bucket{
        public:
            std::list<Peer*> peers;
    };
}
}

#endif /* EPYX_DHT_KBUCKET_H */

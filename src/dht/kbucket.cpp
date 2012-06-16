/**
 * @file kbucket.cpp
 * @brief implements the KBucket
 */

#include "kbucket.h"
#include "../core/common.h"
#include <queue>

namespace Epyx
{
namespace DHT
{

    KBucket::KBucket(const Id& self) : ownId(self) {
        buckets.resize(Id::LENGTH);
    }

    void KBucket::seenPeer(const Id& peerId, const N2NP::NodeId& n2npId) {
        lock.lock();

        bool add =false;
        time_t now=time(NULL);

        Distance d(ownId, peerId);

        //Find the bucket the peer belongs to
        Bucket& bucket = buckets[d.firstActiveBit()];

        std::list<Peer*>::iterator peer_it;

        std::list<Peer*>& peers = bucket.peers;

        //Search for the peer in the bucket to update its lastReceiveTime
        for (peer_it = peers.begin(); peer_it != peers.end(); ++peer_it) {
            Peer* peer = *peer_it;
            if (peerId == peer->id) {
                Peer* temp = peer;
                peer_it = peers.erase(peer_it);
                temp->lastReceiveTime = now;
                temp->n2npId = n2npId;
                peers.push_back(temp);
                lock.unlock();
                return;
            }
        }

        //We want to add the peer to the bucket if it is not full or if the oldest peer in it is too old
        if (peers.size()<NODES_PER_BUCKET) {
            add=true;
        } else {
            Peer* peerFront = peers.front();
            if (now - peerFront->lastReceiveTime>MAX_INACTIVE_PERIOD) {
                peers.pop_front();
                add=true;
                delete peerFront;
            }
        }

        //Finally add the peer to the KBucket
        if (add) {
            peers.push_back(new Peer(peerId, n2npId, now));
        }

        lock.unlock();
    }

    struct FindNearestComparator {
        bool operator()(const std::pair<Distance, Peer> a, const std::pair<Distance, Peer> b) const {
            return a.first < b.first;
        }
    };


    void KBucket::findNearestNodes(const Id& id, std::vector<Peer> &nearest, int n) {
        lock.lock();

        std::priority_queue<std::pair<Distance,Peer>, std::vector<std::pair<Distance, Peer> >,
                FindNearestComparator> closest;

        //Insert EVERY node in the priority_queue with it's distance associated
        //This is very inefficient, we will change this
        //Optimization n°1 : Never store more than n nodes in the map
        //TODO opti2 : split kbuckets when they are full
        //TODO opti3: with split buckets restrain the search
        std::vector<Bucket>::iterator bucket;
        std::list<Peer*>::iterator peer;
        for ( bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ) {
            for ( peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++ ) {

                Distance dist(id, (*peer)->id);

                if (closest.size() >= (unsigned long)n) {
                    std::pair<Distance,Peer> farthest = closest.top();
                    if (dist < farthest.first) {
                        closest.pop();
                    }
                }

                if (closest.size() < (unsigned long)n) {
                    closest.push(std::pair<Distance,Peer>(dist,*(*peer)));
                }
            }
        }

        while (closest.size() > 0) {
            nearest.push_back(closest.top().second);
            closest.pop();
        }
        lock.unlock();
    }

}
}

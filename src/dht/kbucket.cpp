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
        std::lock_guard<std::mutex> guard(lock);

        bool add =false;
        time_t now=time(NULL);

        Distance d(ownId, peerId);

        //Find the bucket the peer belongs to
        Bucket& bucket = buckets[d.firstActiveBit()];

        std::list<Peer::SPtr>& peers = bucket.peers;

        //Search for the peer in the bucket to update its lastReceiveTime
        for (auto peer_it = peers.begin(); peer_it != peers.end(); ++peer_it) {
            Peer::SPtr peer = *peer_it;
            if (peerId == peer->id) {
                Peer::SPtr temp = peer;
                peer_it = peers.erase(peer_it);
                temp->lastReceiveTime = now;
                temp->n2npId = n2npId;
                peers.push_back(temp);
                return;
            }
        }

        //We want to add the peer to the bucket if it is not full or if the oldest peer in it is too old
        if (peers.size() < NODES_PER_BUCKET) {
            add=true;
        } else {
            Peer::SPtr peerFront = peers.front();
            if (now - peerFront->lastReceiveTime > (int)MAX_INACTIVE_PERIOD) {
                peers.pop_front();
                add=true;
            }
        }

        //Finally add the peer to the KBucket
        if (add) {
            peers.push_back(Peer::SPtr(new Peer(peerId, n2npId, now)));
        }

    }

    struct FindNearestComparator {
        bool operator()(const std::pair<Distance, Peer::SPtr> a, const std::pair<Distance, Peer::SPtr> b) const {
            return a.first < b.first;
        }
    };

    typedef std::pair<Distance, Peer::SPtr> SearchCandidate;

    void KBucket::findNearestNodes(const Id& id, std::vector<Peer::SPtr> &nearest, int n) {
        std::lock_guard<std::mutex> guard(lock);

        std::priority_queue<SearchCandidate, std::vector<SearchCandidate>,
                FindNearestComparator> closest;

        //Insert EVERY node in the priority_queue with it's distance associated
        //This is very inefficient, we will change this
        //Optimization n°1 : Never store more than n nodes in the map
        //TODO opti2 : split kbuckets when they are full
        //TODO opti3: with split buckets restrain the search
        for (auto bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ) {
            for (auto peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++ ) {

                Distance dist(id, (*peer)->id);

                if (closest.size() >= (unsigned long)n) {
                    SearchCandidate farthest = closest.top();
                    if (dist < farthest.first) {
                        closest.pop();
                    }
                }

                if (closest.size() < (unsigned long)n) {
                    closest.push(SearchCandidate(dist,Peer::SPtr(*peer)));
                }
            }
        }

        while (closest.size() > 0) {
            nearest.push_back(closest.top().second);
            closest.pop();
        }
    }

}
}

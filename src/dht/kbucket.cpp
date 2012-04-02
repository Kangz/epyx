#include "kbucket.h"
#include "../core/common.h"
#include <queue>

#define NODES_PER_BUCKET 20
#define MAX_INACTIVE_PERIOD 15000

namespace Epyx {
namespace DHT {
    KBucket::KBucket(Id* const self) : ownId(*self){
        buckets.resize(ID_LENGTH);
    }

    void KBucket::seenPeer(Id* const peerId){
        bool add =false;
        time_t now=time(NULL);

        Distance d(&ownId, peerId);

        //Find the bucket the peer belongs to
        Bucket& bucket = buckets[d.firstActiveBit()];

        std::list<Peer*>::iterator peer_it;

        //Search for the peer in the bucket to update its lastReceiveTime
        for(peer_it = bucket.peers.begin(); peer_it != bucket.peers.end(); peer_it++){
            Peer* peer = *peer_it;
            if(*peerId == peer->peerId){
                Peer* temp = peer;
                peer_it = bucket.peers.erase(peer_it);
                temp->lastReceiveTime = now;
                bucket.peers.push_back(temp);
                return;
            }
        }

        //We want to add the peer to the bucket if it is not full or if the oldest peer in it is too old
        if(bucket.peers.size()<NODES_PER_BUCKET){
            add=true;
        }else{
            Peer* peerFront = bucket.peers.front();
            if(now - peerFront->lastReceiveTime>MAX_INACTIVE_PERIOD){
                bucket.peers.pop_front();
                add=true;
                delete peerFront;
            }
        }

        //Finally add the peer to the KBucket
        if(add){
            Peer* newPeer = new Peer();
            newPeer->peerId=*peerId;
            newPeer->lastReceiveTime=now;
            bucket.peers.push_back(newPeer);
        }
    }

    void KBucket::findNearestNodes(Id* const id, std::multimap<Distance,Id> &nearest,int n){
        std::priority_queue<std::pair<Distance,Id> > closest;

        //Insert EVERY node in the priority_queue with it's distance associated
        //This is very inefficient, we will change this
        //Optimization n°1 : Never store more than n nodes in the map
        std::vector<Bucket>::iterator bucket;
        std::list<Peer*>::iterator peer;
        for( bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ){
            for( peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++ ){

                Distance dist(id, &((*peer)->peerId));

                if(closest.size() >= (unsigned long)n){
                    std::pair<Distance,Id> farthest = closest.top();
                    if(dist < farthest.first){
                        closest.pop();
                    }
                }

                if(closest.size() < (unsigned long)n){
                    closest.push(std::pair<Distance,Id>(dist,(*peer)->peerId));
                }
            }
        }

        while(closest.size() > 0){
            nearest.insert(closest.top());
            closest.pop();
        }
    }

}
}

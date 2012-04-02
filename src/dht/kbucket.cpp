#include "kbucket.h"

#define NODES_PER_BUCKET 20
#define  MAX_INACTIVE_PERIOD 15000

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


    //We need to have the farthest Id first to be able to remove it easily
    struct ReverseDistanceComparator{
        bool operator()(const Distance& d1, const Distance& d2){
            return ! (d1 < d2);
        }
    };

    void KBucket::findNearestNodes(Id* const id,std::multimap<Distance,Id> &nearest,int n){
        std::multimap<Distance,Id, ReverseDistanceComparator> closest;
        std::multimap<Distance,Id, ReverseDistanceComparator>::iterator mapit;

        std::vector<Bucket>::iterator bucket;
        std::list<Peer*>::iterator peer;

        //Insert EVERY node in the map with it's distance associated
        //This is very inefficient, we will change this
        //Optimization n°1 : Never store more than n nodes in the map
        for( bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ){
            for( peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++){

                Distance dist(id, &((*peer)->peerId));

                if(closest.size() >= (unsigned long)n-1){
                    std::pair<Distance,Id> farthest = *(closest.begin());
                    if(farthest.first < dist){
                        closest.erase(closest.begin());
                    }
                }

                if(closest.size() < (unsigned long)n-1){
                    closest.insert(std::pair<Distance,Id>(dist,(*peer)->peerId));
                }
            }
        }

        int i=0;
        //The map iterator will return the peers sorted by distance
        for(mapit=closest.begin(); mapit!=closest.end() && i<n; mapit++,i++){
            nearest.insert(*mapit);
        }
    }

}
}

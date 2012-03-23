#include "kbucket.h"

#define NODES_PER_BUCKET 20
#define  MAX_INACTIVE_PERIOD 15000

namespace Epyx {
namespace DHT {
    KBucket::KBucket(Id* const self) : ownId(*self){
        buckets.resize(ID_LENGTH);
    }

    void KBucket::addPeer(Id* const peerId){
        bool add =false;
        time_t now=time(NULL);

        Distance d(&ownId, peerId);

        //Find the bucket the peer belongs to
        Bucket& bucket = buckets[d.firstActiveBit()];

        std::list<Peer*>::iterator peer;

        //Search for the peer in the bucket to update its lastReceiveTime
        for(peer = bucket.peers.begin(); peer != bucket.peers.end(); peer++){
            Peer* p = *peer;
            if(*peerId == p->peerId){
                Peer* temp = p;
                peer = bucket.peers.erase(peer);
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
            }
            delete *peer;
        }

        //Finally add the peer to the KBucket
        if(add){
            Peer* newPeer = new Peer();
            newPeer->peerId=*peerId;
            newPeer->lastReceiveTime=now;
            bucket.peers.push_back(newPeer);
        }
    }


    void KBucket::findNearestNodes(Id* const id,std::multimap<Distance,Id> &nearest,int n){
        std::multimap<Distance,Id> closest;
        std::multimap<Distance,Id>::iterator mapit;

        Distance* dist;

        std::vector<Bucket>::iterator bucket;
        std::list<Peer*>::iterator peer;

        //Insert EVERY node in the map with it's distance associated
        //This is very inefficient, we will change this
        for( bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ){
            for( peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++){
                dist = new Distance(id, &((*peer)->peerId));
                closest.insert(std::pair<Distance,Id>(*dist,(*peer)->peerId));
                delete dist;
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

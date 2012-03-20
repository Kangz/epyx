#include "kbucket.h"

namespace Epyx {
namespace DHT {
    KBucket::Kbucket(Id* const self) : self(*ownId){
        buckets.resize(ID_LENGTH);
    }

    int KBucket::findBucket(Distance* const d){
        for(int i = 0; i < ID_LENGTH; i++) {
            int bit = ID_LENGTH - i - 1;
            int byte = i / 8;
            int bit_in_byte = 7 - (i % 8);

            unsigned char comp = (1 << bit_in_byte);

            if (comp & d->data[byte]) {
                return bit;
            }
        }
        return 0;
    }

    void KBucket::add_peer(Id* const peerId){
        bool add =false;
        time_t now=time(NULL);

        Distance* d = ownId.distanceTo(peerId);
        int bucket = findBucket(d);
        delete d;

        //Find the bucket the peer belongs to
        Bucket& bucket = buckets[bucket];

        std::list<Peer*>::iterator peer;

        //Search for the peer in the bucket to update its lastReceiveTime
        for(peer = bucket.peers.begin(); peer != bucket.peers.end(); peer++){
            if(*peerId == peer->peerId){
                Peer* temp = *peer;
                peer = buck.peers.erase(peer);
                temp.lastReceiveTime = now;
                buck.peers.push_back(temp);
                return;
            }
        }

        //We want to add the peer to the bucket if it is not full or if the oldest peer in it is too old
        if(buck.peers.size()<NODES_PER_BUCKET){
            add=true;
        }else{
            Peer* peerFront = buck.peers.front();
            if(now - peerFront->lastReceiveTime>MAX_INACTIVE_PERIODE){
                buck.peers.pop_front();
                add=true;
            }
            delete Peer;
        }

        //Finally add the peer to the KBucket
        if(add){
            Peer* newPeer = new Peer;
            newPeer->peerId=*peerId;
            newPeer->lastReceiveTime=now;
            buck.peers.push_back(newPeer);
        }
    }


    void KBucket::find_nearest_nodes(Id* const id,std::multimap<Distance,Id> &nearest,int n){
        std::multimap<Distance,Id> closest;
        std::multimap<Distance,Id>::iterator mapit;

        Distance* dist;

        std::vector<Bucket>::iterator bucket;
        std::list<Peer*>::iterator peer;

        //Insert EVERY node in the map with it's distance associated
        //This is very inefficient, we will change this
        for( bucket=buckets.begin(); bucket!=buckets.end(); bucket++ ){
            for( peer=bucket->peers.begin(); peer!=bucket->peers.end(); peer++){
                dist = id->distanceTo(peer->peerId);
                closest.insert(std::pair<Distance,Id>(*dist,pit->peerId));
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

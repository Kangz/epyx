#include "static-actors.h"
#include "node.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    PingActor::PingActor(InternalNode& n): n(n) {
    }

    void PingActor::treat(Peer::SPtr peer, Packet::SPtr pkt) {
        //We only need to send back a pong
        Packet answer;
        answer.method = M_PONG;
        this->n.send(answer, peer);
    }

    GetActor::GetActor(InternalNode& n): n(n) {
    }

    void GetActor::treat(Peer::SPtr peer, Packet::SPtr pkt) {
        Packet answer;
        answer.method = M_GOT;
        answer.connectionId = pkt->connectionId;
        answer.status = 1;

        //Try to find the value
        Value requested;
        if(this->n.storage.get(pkt->key, requested)){
            answer.value = requested.content;
            answer.status = 0;
        }
        this->n.send(answer, peer);
    }

    StoreActor::StoreActor(InternalNode& n): n(n) {
    }

    void StoreActor::treat(Peer::SPtr peer, Packet::SPtr pkt) {
        Packet answer;
        answer.method = M_STORED;
        answer.connectionId = pkt->connectionId;
        answer.status = 0;

        this->n.storage.set(pkt->key, pkt->value);

        this->n.send(answer, peer);
    }

    FindActor::FindActor(InternalNode& n): n(n) {
    }

    void FindActor::treat(Peer::SPtr peer, Packet::SPtr pkt) {
        Packet answer;
        answer.method = M_FOUND;
        answer.connectionId = pkt->connectionId;
        answer.status = 0;
        answer.foundPeers = new std::vector<Peer::SPtr>();

        //Ask the kbucket
        this->n.kbucket.findNearestNodes(pkt->idToFind, *answer.foundPeers, pkt->count);

        this->n.send(answer, peer);
    }

}
}

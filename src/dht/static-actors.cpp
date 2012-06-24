#include "static-actors.h"
#include "node.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    StaticActorData::StaticActorData(Peer& peer, Packet* pkt)
    :peer(peer), pkt(pkt) {
    }

    StaticActorData::StaticActorData(Peer& peer, Packet& pkt)
    :peer(peer), pkt(&pkt) {
    }

    void StaticActorData::freeData(){
        delete &peer;
        delete pkt;
    }

    PingActor::PingActor(InternalNode& n): n(n) {
    }

    void PingActor::treat(StaticActorData& msg) {
        //We only need to send back a pong
        Packet pkt;
        pkt.method = M_PONG;
        this->n.send(pkt, msg.peer);
        msg.freeData();
    }

    GetActor::GetActor(InternalNode& n): n(n) {
    }

    void GetActor::treat(StaticActorData& msg) {
        Packet answer;
        answer.method = M_GOT;
        answer.connectionId = msg.pkt->connectionId;
        answer.status = 1;

        //Try to find the value
        Value requested;
        if(this->n.storage.get(msg.pkt->key, requested)){
            answer.value = requested.content;
            answer.status = 0;
        }
        this->n.send(answer, msg.peer);
        msg.freeData();
    }

    StoreActor::StoreActor(InternalNode& n): n(n) {
    }

    void StoreActor::treat(StaticActorData& msg) {
        Packet answer;
        answer.method = M_STORED;
        answer.connectionId = msg.pkt->connectionId;
        answer.status = 0;

        this->n.storage.set(msg.pkt->key, msg.pkt->value);

        this->n.send(answer, msg.peer);
        msg.freeData();
    }

    FindActor::FindActor(InternalNode& n): n(n) {
    }

    void FindActor::treat(StaticActorData& msg) {
        Packet answer;
        answer.method = M_FOUND;
        answer.connectionId = msg.pkt->connectionId;
        answer.status = 0;
        answer.foundPeers = new std::vector<Peer>();

        //Ask the kbucket
        this->n.kbucket.findNearestNodes(msg.pkt->idToFind, *answer.foundPeers, msg.pkt->count);

        this->n.send(answer, msg.peer);
        msg.freeData();
    }

}
}

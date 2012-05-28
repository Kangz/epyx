#include "process-actors.h"
#include "internal-node.h"
#include "finder-actor.h"
#include "getter-actor.h"
#include "setter-actor.h"

namespace Epyx
{
namespace DHT
{

    ProcessActorData::ProcessActorData(Target& target, Packet* pkt)
    :target(target), pkt(pkt) {
    }

    ProcessActorData::ProcessActorData(Target& target, Packet& pkt)
    :target(target), pkt(&pkt) {
    }

    void ProcessActorData::freeData() {
        delete &target;
        delete pkt;
    }

    ProcessActor::ProcessActor(InternalNode& n, int timeout)
    :n(n) {
        processId = n.registerProcessActor(*this, timeout);
    }

    void ProcessActor::destroy() {
        n.unregisterProcessActor(processId);
    }

    SingularFindActor::SingularFindActor(InternalNode& n, ActorId<FinderActorData> p, Peer& peer, Id& requested)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), target(peer), parent(p){
        Packet pkt;
        pkt.method = M_FIND;
        pkt.connectionId = processId;
        pkt.count = 5; //TODO make it a cst
        pkt.idToFind = requested;

        this->n.send(pkt, *n.peerToTarget(peer));
    }

    void SingularFindActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_FOUND && msg.pkt->status == 0 && msg.pkt->count > 0) {
            parent.post(*(new FinderActorData(target, msg.pkt->foundPeers, true)));
            msg.pkt->foundPeers = NULL;
            kill();
        } else {
            timeout();
        }
    }

    void SingularFindActor::timeout() {
        parent.post(*(new FinderActorData(target, NULL, false)));
        kill();
    }

    SingularGetActor::SingularGetActor(InternalNode& n, ActorId<GetterActorData> p, Peer& peer, const std::string& key)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), parent(p) {
        Packet pkt;
        pkt.method = M_GET;
        pkt.connectionId = processId;
        pkt.key = key;

        this->n.send(pkt, *n.peerToTarget(peer));
    }

    void SingularGetActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_GOT && msg.pkt->status == 0) {
            parent.post(*(new GetterActorData(msg.pkt->value)));
            kill();
        } else {
            timeout();
        }
    }

    void SingularGetActor::timeout() {
        parent.post(*(new GetterActorData()));
        kill();
    }

    SingularSetActor::SingularSetActor(InternalNode& n, ActorId<SetterActorData> p, Peer& peer, const std::string& key, const std::string& value)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), parent(p) {
        Packet pkt;
        pkt.method = M_STORE;
        pkt.connectionId = processId;
        pkt.key = key;
        pkt.value = value;

        this->n.send(pkt, *n.peerToTarget(peer));
    }

    void SingularSetActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_GOT && msg.pkt->status == 0) {
            parent.post(*(new SetterActorData(true)));
            kill();
        } else {
            timeout();
        }
    }

    void SingularSetActor::timeout() {
        parent.post(*(new SetterActorData(false)));
        kill();
    }

}
}

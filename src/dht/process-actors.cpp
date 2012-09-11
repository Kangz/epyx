#include "process-actors.h"
#include "internal-node.h"
#include "finder-actor.h"
#include "getter-actor.h"
#include "setter-actor.h"
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    ProcessActorData::ProcessActorData(Peer& peer, Packet* pkt)
    :peer(peer), pkt(pkt) {
    }

    ProcessActorData::ProcessActorData(Peer& peer, Packet& pkt)
    :peer(peer), pkt(&pkt) {
    }

    void ProcessActorData::freeData() {
        delete &peer;
        delete pkt;
    }

    ProcessActor::ProcessActor(InternalNode& n, int timeout)
    :n(n) {
        processId = n.registerProcessActor(*this, timeout);
    }

    void ProcessActor::destroy() {
        n.unregisterProcessActor(processId);
        kill();
    }

    SingularFindActor::SingularFindActor(InternalNode& n, ActorId<FinderActor> p, Peer& peer, Id& requested)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), target(peer), parent(p) {
        //Send the FIND query
        Packet pkt;
        pkt.method = M_FIND;
        pkt.connectionId = processId;
        pkt.count = FIND_NB_NODE_REQUESTED;
        pkt.idToFind = requested;

        this->n.send(pkt, target);
    }

    void SingularFindActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_FOUND && msg.pkt->status == 0 && msg.pkt->count > 0) {
            parent.post(*(new FinderActorData(target, msg.pkt->foundPeers, true)));
            msg.pkt->foundPeers = NULL;
            destroy();
        } else {
            timeout();
        }
    }

    void SingularFindActor::timeout() {
        parent.post(*(new FinderActorData(target, NULL, false)));
        destroy();
    }

    SingularGetActor::SingularGetActor(InternalNode& n, ActorId<GetterActor> p, Peer& peer, const std::string& key)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), parent(p) {
        //Send the GET query
        Packet pkt;
        pkt.method = M_GET;
        pkt.connectionId = processId;
        pkt.key = key;

        this->n.send(pkt, peer);
    }

    void SingularGetActor::treat(ProcessActorData& msg) {
        if(msg.pkt->method == M_GOT && msg.pkt->status == 0) {
            parent.post(*(new GetterActorData(msg.pkt->value)));
            destroy();
        } else {
            timeout();
        }
    }

    void SingularGetActor::timeout() {
        parent.post(*(new GetterActorData()));
        destroy();
    }

    SingularSetActor::SingularSetActor(InternalNode& n, ActorId<SetterActor> p, Peer& peer, const std::string& key, const std::string& value)
    :ProcessActor(n, SINGLE_REQUEST_TIMEOUT), parent(p) {
        //Send the SET query
        Packet pkt;
        pkt.method = M_STORE;
        pkt.connectionId = processId;
        pkt.key = key;
        pkt.value = value;

        this->n.send(pkt, peer);
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

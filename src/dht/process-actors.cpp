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

    void SingularFindActor::treat(Peer* peer, Packet* pkt) {
        if(pkt->method == M_FOUND && pkt->status == 0 && pkt->count > 0) {
            parent.post(EPYX_AQ("found"), target, pkt->foundPeers);
            pkt->foundPeers = NULL;
            destroy();
        } else {
            timeout();
        }

        delete peer;
        delete pkt;
    }

    void SingularFindActor::timeout() {
        parent.post(EPYX_AQ("not found"), target);
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

    void SingularGetActor::treat(Peer* peer, Packet* pkt) {
        if(pkt->method == M_GOT && pkt->status == 0) {
            parent.post(EPYX_AQ("get success"), pkt->value);
            destroy();
        } else {
            timeout();
        }
    }

    void SingularGetActor::timeout() {
        parent.post(EPYX_AQ("get failure"));
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

    void SingularSetActor::treat(Peer* peer, Packet* pkt) {
        if(pkt->method == M_GOT && pkt->status == 0) {
            parent.post(EPYX_AQ("set success"));
            kill();
        } else {
            timeout();
        }
    }

    void SingularSetActor::timeout() {
        parent.post(EPYX_AQ("set failure"));
        kill();
    }

}
}

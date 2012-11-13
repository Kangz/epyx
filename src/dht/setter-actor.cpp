#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "finder-actor.h"
#include "setter-actor.h"
#include <algorithm>
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    void SetCallback::onError() {
    }

    SetCallback::~SetCallback() {
    }

    SetterSearchCallback::SetterSearchCallback(ActorId<SetterActor> parent)
    :parent(parent){
    }

    void SetterSearchCallback::onFound(ClosestQueue& result) {
        std::vector<Peer::SPtr>* res = new std::vector<Peer::SPtr>();
        for(auto it = result.begin(); it != result.end(); it ++)  {
            res->push_back((*it).second);
        }
        parent.post(EPYX_AQ("find success"), res);
    }

    void SetterSearchCallback::onError() {
        parent.post(EPYX_AQ("find failure"));
    }

    SetterActor::SetterActor(InternalNode& n, const std::string& key, const std::string& value, SetCallback* cb)
    :ProcessActor(n), callback(cb), key(key), value(value), nErrors(0), found(false) {
    }

    void SetterActor::start() {
        Id id;
        idForString(id, key);
        //Send the FIND process
        n.findClosest(new SetterSearchCallback(Actor::getId(this)), SET_REDUNDANCY, id);
    }

    void SetterActor::treat(EPYX_AQA("find success"), std::vector<Peer::SPtr>* peers) {
        for(auto it = peers->begin(); it != peers->end(); it ++) {
            ask(**it);
        }
        pendingRequests = peers->size();

        found = true;
        delete peers;
    }

    void SetterActor::treat(EPYX_AQA("find failure")) {
        timeout();
    }

    void SetterActor::onNewAnswer(Peer::SPtr peer, Packet::UPtr pkt) {
        if (pkt->method != M_STORED || pkt->status != 0) {
            nErrors ++;
        }
        onSetReceive();
    }

    void SetterActor::onAnswerTimeout(long id) {
        nErrors ++;
        onSetReceive();
    }

    void SetterActor::onSetReceive() {
        pendingRequests --;
        if(pendingRequests == 0) {
            if(nErrors < SET_ERROR_THRESHOLD) {
                callback->onSet();
                delete callback;
                kill();
            } else {
                timeout();
            }
        }
    }

    void SetterActor::ask(Peer& p) {
        Packet pkt;
        pkt.method = M_STORE;
        pkt.key = key;
        pkt.value = value;

        //HACK!!!
        Peer::SPtr peer(new Peer(p));

        this->sendQuery(peer, pkt, SINGLE_REQUEST_TIMEOUT);
    }

    void SetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}

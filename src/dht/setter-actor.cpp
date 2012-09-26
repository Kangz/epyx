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
        std::vector<Peer>* res = new std::vector<Peer>();
        ClosestQueue::iterator it;
        for(it = result.begin(); it != result.end(); it ++)  {
            res->push_back((*it).second);
        }
        parent.post(EPYX_AQ("find success"), res);
    }

    void SetterSearchCallback::onError() {
        parent.post(EPYX_AQ("find failure"));
    }

    SetterActor::SetterActor(InternalNode& n, const std::string& key, const std::string& value, SetCallback* cb)
    :n(n), callback(cb), key(key), value(value), nErrors(0), found(false) {
    }

    void SetterActor::start() {
        Id id;
        idForString(id, key);
        //Send the FIND process
        n.findClosest(new SetterSearchCallback(Actor::getId(this)), SET_REDUNDANCY, id);
    }

    void SetterActor::treat(EPYX_AQA("find success"), std::vector<Peer>* peers) {
        for(auto it = peers->begin(); it != peers->end(); it ++) {
            ask(*it);
        }
        pendingRequests = peers->size();

        found = true;
        delete peers;
    }

    void SetterActor::treat(EPYX_AQA("find failure")) {
        timeout();
    }

    void SetterActor::treat(EPYX_AQA("set success")) {
        onSetReceive();
    }

    void SetterActor::treat(EPYX_AQA("set failure")) {
        nErrors ++;
        onSetReceive();
    }

    void SetterActor::onSetReceive() {
        pendingRequests --;
        if(pendingRequests == 0) {
            if(nErrors > SET_ERROR_THRESHOLD) {
                callback->onSet();
                delete callback;
                kill();
            } else {
                timeout();
            }
        }
    }

    void SetterActor::ask(Peer& p) {
        new SingularSetActor(n, Actor::getId(this), p, key, value);
    }

    void SetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}

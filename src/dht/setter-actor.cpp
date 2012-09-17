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

    SetterActorData::SetterActorData()
    :peersToAsk(NULL), found(false), succeded(false) {
    }

    SetterActorData::SetterActorData(std::vector<Peer>* peers)
    :peersToAsk(peers), found(true), succeded(false) {
    }

    SetterActorData::SetterActorData(bool success)
    :peersToAsk(NULL), found(false), succeded(success) {
    }

    SetterActorData::~SetterActorData(){
        if(peersToAsk != NULL) {
            delete peersToAsk;
        }
    }

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
        parent.post(new SetterActorData(res));
    }

    void SetterSearchCallback::onError() {
        parent.post(new SetterActorData());
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

    void SetterActor::treat(SetterActorData* msg) {
        //While we wait for the FIND process
        if(! found) {
            if (! msg->found) {
                timeout();
            } else {
                std::vector<Peer>::iterator it;
                for(it = msg->peersToAsk->begin(); it != msg->peersToAsk->end(); it ++) {
                    ask(*it);
                }
                pendingRequests = msg->peersToAsk->size();
            }
            found = true;
            return;
        }

        if(!msg->succeded) {
            nErrors ++;
        }

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

        delete msg;
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

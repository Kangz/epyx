#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "finder-actor.h"
#include "setter-actor.h"
#include <algorithm>

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

    #define SET_REDUNDANCY 10

    SetterSearchCallback::SetterSearchCallback(ActorId<SetterActorData> parent)
    :parent(parent){
    }

    void SetterSearchCallback::onFound(ClosestQueue& result) {
        std::vector<Peer>* res = new std::vector<Peer>();
        ClosestQueue::iterator it;
        for(it = result.begin(); it != result.end(); it ++)  {
            res->push_back((*it).second);
        }
        parent.post(*(new SetterActorData(res)));
    }

    void SetterSearchCallback::onError() {
        parent.post(*(new SetterActorData()));
    }

    SetterActor::SetterActor(InternalNode& n, const std::string& key, const std::string& value, SetCallback* cb)
    :n(n), callback(cb), key(key), value(value), nErrors(0), found(false) {
    }

    void SetterActor::start() {
        Id id;
        idForString(id, key);
        n.findClosest(new SetterSearchCallback(getId()), SET_REDUNDANCY, id);
    }

    void SetterActor::treat(SetterActorData& msg) {
        if(! found) {
            if (! msg.found) {
                timeout();
            } else {
                std::vector<Peer>::iterator it;
                for(it = msg.peersToAsk->begin(); it != msg.peersToAsk->end(); it ++) {
                    ask(*it);
                }
                pendingRequests = msg.peersToAsk->size();
            }
            return;
        }

        if(!msg.succeded) {
            nErrors ++;
        }

        pendingRequests --;
        if(pendingRequests == 0) {
            if(nErrors <= 3) {
                callback->onSet();
                delete callback;
                kill();
            } else {
                timeout();
            }
        }
    }

    void SetterActor::ask(Peer& p) {
        new SingularSetActor(n, getId(), p, key, value);
    }

    void SetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}

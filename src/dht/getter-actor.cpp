#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "finder-actor.h"
#include <algorithm>
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    GetterActorData::GetterActorData()
    :peersToAsk(NULL), found(false), answered(false) {
    }

    GetterActorData::GetterActorData(std::vector<Peer>* peers)
    :peersToAsk(peers), found(true), answered(false) {
    }

    GetterActorData::GetterActorData(const std::string& result)
    :peersToAsk(NULL), found(false), answered(true), result(result) {
    }

    GetterActorData::~GetterActorData() {
        if(peersToAsk != NULL) {
            delete peersToAsk;
        }
    }

    void GetCallback::onError() {
    }

    GetCallback::~GetCallback() {
    }

    GetterSearchCallback::GetterSearchCallback(ActorId<GetterActor> parent)
    :parent(parent) {
    }

    void GetterSearchCallback::onFound(ClosestQueue& result) {
        std::vector<Peer>* res = new std::vector<Peer>();
        ClosestQueue::iterator it;
        for(it = result.begin(); it != result.end(); it ++)  {
            res->push_back((*it).second);
        }
        parent.post(*(new GetterActorData(res)));
    }

    void GetterSearchCallback::onError() {
        parent.post(*(new GetterActorData()));
    }

    GetterActor::GetterActor(InternalNode& n, const std::string& key, GetCallback* cb)
    :n(n), callback(cb), key(key), found(false) {
    }

    void GetterActor::start() {
        Id id;
        idForString(id, key);
        //Start the FIND process
        n.findClosest(new GetterSearchCallback(Actor::getId(this)), GET_REDUNDANCY, id);
    }

    void GetterActor::treat(GetterActorData& msg) {
        //Branch used when we wait for the FIND process
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
            found = true;
            return;
        }

        //For now we stop at the first answer
        if(msg.answered) {
            callback->onGot(msg.result);
            delete callback;
            kill();
            return;
        }

        pendingRequests --;
        if(pendingRequests == 0) {
            timeout();
        }

    }

    void GetterActor::ask(Peer& p) {
        new SingularGetActor(n, Actor::getId(this), p, key);
    }

    void GetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}

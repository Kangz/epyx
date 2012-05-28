#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "finder-actor.h"
#include <algorithm>

namespace Epyx
{
namespace DHT
{

    GetterActorData::GetterActorData()
    :peersToAsk(NULL), found(false), answered(false){
    }

    GetterActorData::GetterActorData(std::vector<Peer>* peers)
    :peersToAsk(peers), found(true), answered(false){
    }

    GetterActorData::GetterActorData(const std::string& result)
    :peersToAsk(NULL), found(false), answered(true), result(result){
    }

    GetterActorData::~GetterActorData(){
        if(peersToAsk != NULL) {
            delete peersToAsk;
        }
    }

    void GetCallback::onError() {
    }

    GetCallback::~GetCallback() {
    }

    #define GET_REDUNDANCY 10

    GetterSearchCallback::GetterSearchCallback(ActorId<GetterActorData> parent)
    :parent(parent){
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
        n.findClosest(new GetterSearchCallback(getId()), GET_REDUNDANCY, id);
    }

    void GetterActor::treat(GetterActorData& msg) {
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
        new SingularGetActor(n, getId(), p, key);
    }

    void GetterActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }
}
}

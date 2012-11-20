#include "replicater-actor.h"
#include "node.h"
#include "internal-node.h"
#include "consts.h"
#include "value.h"

namespace Epyx
{
namespace DHT
{

    ReplicaterActor::ReplicaterActor(InternalNode& n): n(n), storage(n.storage) {
    }

    void ReplicaterActor::treat(EPYX_AQA("new key"), std::string key) {
        getId(this).timeout(REPLICATION_TIMEOUT, EPYX_AQ("replicate"), key);
        log::debug << "Replicater: starting to manage \"" << key << "\"" << log::endl;
    }

    void ReplicaterActor::timeout(EPYX_AQA("replicate"), std::string key) {
        Value v;
        log::debug << "Replicater: replicating \"" << key << "\"" << log::endl;
        if(storage.get(key, v)){
            n.setValue(new ReplicaterSetCallback(n, getId(this), key), key, v.content);
        }
    }

    ReplicaterSetCallback::ReplicaterSetCallback(InternalNode& n, ActorId<ReplicaterActor> replicater, std::string key)
    :replicater(replicater), n(n), isHandlingKey(false), key(key) {
    }

    void ReplicaterSetCallback::onSet(){
        if(isHandlingKey){
            replicater.timeout(REPLICATION_TIMEOUT, EPYX_AQ("replicate"), key);
        }
    }

    void ReplicaterSetCallback::onFoundPeers(std::shared_ptr<std::vector<Peer::SPtr>> peers) {
        for(auto it = peers->begin(); it != peers->end(); it++){
            if((*it)->id == n.id){
                isHandlingKey = true;
            }
        }
    }
}
}

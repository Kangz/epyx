/**
 * @file actor-manager.cpp
 * @brief Implementation of the actor framework (non-templated part)
 */

#include "actor-manager.h"

#include "common.h"

namespace Epyx
{

    ActorManager::ActorManager(int num_workers, const std::string& name):
        wp(num_workers, name, this), actorCount(0){
    }

    ActorManager::~ActorManager(){
        //TODO: find out why it was not called
        wp.stop();
        //TODO finish and delete everything
    }


    void ActorManager::kill(ActorId_base a){
        actorsLock.lock();

        std::map<int, Actor_base*>::iterator it;
        it = actors.find(a.id);

        // If we find the actor we wait until it finishes what it is doing
        // and we destroy it. The workers will not access it as they lock
        // actor before unlocking actorsLock
        if(it != actors.end()){
            Actor_base* actor = (*it).second;
            actor->lock();
            actor->unlock();
            delete actor;
            actors.erase(it);
        }

        actorsLock.unlock();
    }

    void ActorManager::post(int id, void* msg){
        wp.post(new std::pair<int, void*>(id, msg));
    }

    void ActorManager::ActorWorkers::treat(std::pair<int, void*>* msg){
        ActorManager* m = this->manager;

        Actor_base* a = NULL;

        m->actorsLock.lock();

        std::map<int, Actor_base*>::iterator it;
        it = m->actors.find(msg->first);
        if(it != m->actors.end()){
            Actor_base* actor = (*it).second;

            //KEEP IT IN THIS ORDER, see kill() for more info
            actor->lock();
            m->actorsLock.unlock();
            actor->_internal_treat(msg->second);
            actor->unlock();
        }else{
            m->actorsLock.unlock();
        }

    }

}


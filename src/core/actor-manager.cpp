/**
 * @file actor-manager.cpp
 * @brief Implementation of the actor framework (non-templated part)
 */

#include "actor-manager.h"

#include "common.h"

namespace Epyx
{

    ActorManager::ActorManager(int num_workers, const std::string& name) :
    wp(num_workers, name, this), actorCount(0), timeouts(this, name){
        timeouts.start();
    }

    ActorManager::~ActorManager() {
        //TODO: find out why it was not called
        wp.stop();
        //TODO finish and delete everything
    }

    void ActorManager::kill(ActorId_base a) {
        actorsLock.lock();

        std::map<int, Actor_base*>::iterator it;
        it = actors.find(a.id);

        //If we find the actor we only set the alive flag and send it
        //a message, it will be killed the next time it is processed
        if (it != actors.end()) {
            Actor_base* actor = (*it).second;
            actor->alive = false;
            actorsLock.unlock();

            this->post(a.id, NULL);
        } else {
            actorsLock.unlock();
        }
    }

    void ActorManager::post(int id, void* msg) {
        wp.post(new std::pair<int, void*>(id, msg));
    }

    ActorManager::ActorWorkers::ActorWorkers(int num_workers, const std::string& name,
        ActorManager* m)
    :WorkerPool<std::pair<int, void*> >(num_workers, true, name),
    manager(m) {
    }

    //pair->first is the id of the actor and ->second the message
    //if actor does not exist, do noting
    //if he is dead remove it and do nothing
    //if message = null the actor will call timeout()
    //else he will call treat(message)
    void ActorManager::ActorWorkers::treat(std::pair<int, void*>* msg){
        ActorManager* m = this->manager;

        Actor_base* a = NULL;

        m->actorsLock.lock();

        std::map<int, Actor_base*>::iterator it;
        it = m->actors.find(msg->first);
        if (it != m->actors.end()) {
            Actor_base* actor = (*it).second;

            //KEEP IT IN THIS ORDER, see kill() for more info
            //there won't be deadlocks as we always acquire
            //actor before unlocking actorsLock
            //
            actor->lock();
            if (actor->alive) {
                m->actorsLock.unlock();
                actor->_internal_treat(msg->second);
                actor->unlock();
            } else {
                actor->unlock();
                delete actor;
                m->actors.erase(it);
                m->actorsLock.unlock();
            }
        }else{
            m->actorsLock.unlock();
        }
    }

    bool ActorManager::ActorTimeoutComparator::operator()(const std::pair<Timeout, int> a, const std::pair<Timeout, int> b){
        return ! (a.first < b.first);
    }

    ActorManager::TimeoutLauncher::TimeoutLauncher(ActorManager* m, const std::string& name): Thread(name + "Timeouts", 0), manager(m) {
    }

    ActorManager::TimeoutLauncher::~TimeoutLauncher(){
        incoming.close();
        this->wait();
    }

    void ActorManager::TimeoutLauncher::addTimeout(Timeout t, int id){
        incoming.push(new std::pair<Timeout, int>(t, id));
    }

    void ActorManager::TimeoutLauncher::run(){
        const int epsilon = 2;

        std::pair<Timeout, int>* to_add = NULL;
        do{
            //Wait for a new entry or until something times out
            if(timeouts.empty()){
                to_add = incoming.pop();
            }else{
                to_add = incoming.pop(timeouts.top().first.remainingMsec() + epsilon);
            }

            if(to_add != NULL){
                timeouts.push(*to_add);
                delete to_add;
            }

            while(!timeouts.empty() && timeouts.top().first.hasExpired()){
                manager->post(timeouts.top().second, NULL);
                timeouts.pop();
            }

        }while(incoming.isOpened());
    }

}


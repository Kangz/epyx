#ifndef EPYX_CORE_ACTOR_MANAGER_H
#define EPYX_CORE_ACTOR_MANAGER_H

#include "actor.h"
#include "actor-base.h"
#include "mutex.h"
#include "worker-pool.h"
#include <string>
#include <map>

namespace Epyx
{

    struct ActorId_base;
    template<typename T> struct ActorId;
    template<typename T> class Actor;


    class ActorManager
    {
    public:
        ActorManager(int num_workers, const std::string& name);
        ~ActorManager();

        void has(ActorId_base id);

        template<typename T> ActorId<T> add(Actor<T>* a);
        template<typename T> ActorId<T> add(Actor<T>& a);

        //template<typename T> ActorId<T> register(Actor<T> a, int timeout);

        void kill(ActorId_base id);

        void post(int id, void* msg);

    private:
        class ActorWorkers: public WorkerPool<std::pair<int, void*> >
        {
        public:
            ActorWorkers(int num_workers, const std::string& name, ActorManager* m):
                WorkerPool<std::pair<int, void*> >(num_workers, true, name){
                    this->manager = m;
            }

            virtual void treat(std::pair<int, void*>* msg);

        private:
            ActorManager* manager;
        };

        ActorWorkers wp;
        std::map<int, Actor_base*> actors;
        Mutex actorsLock;
        int actorCount;
    };


    template<typename T> ActorId<T> ActorManager::add(Actor<T>* a){
        EPYX_ASSERT(a != NULL);
        return this->add(*a);
    }

    template<typename T> ActorId<T> ActorManager::add(Actor<T>& a){
        ActorId<T> res(this, actorCount);

        a.setId(res);

        actorsLock.lock();
        actors[actorCount++] = &a;
        actorsLock.unlock();

        return res;
    }



    struct ActorId_base{
        int id;
        ActorManager* manager;
    };

    template<typename T> struct ActorId: public ActorId_base{
        ActorId(){
            manager = NULL;
            id = -1;
        }
        ActorId(ActorManager* m, int i){
            manager = m;
            id = i;
        }
        void post(T& msg){
            manager->post(id, static_cast<void*>(&msg));
        }
        void kill(){
            manager->kill(*this);
        }
    };
}
#endif //EPYX_CORE_ACTOR_MANAGER_H

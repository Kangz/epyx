/**
 * @file actor-manager-detail.h
 * @brief Implementation of the actor framework (templated part)
 */

#ifndef EPYX_CORE_ACTOR_MANAGER_DETAIL_H
#define EPYX_CORE_ACTOR_MANAGER_DETAIL_H

namespace Epyx
{

    template<typename T> ActorId<T> ActorManager::add(Actor<T>* a) {
        EPYX_ASSERT(a != NULL);
        return this->add(*a);
    }

    template<typename T> ActorId<T> ActorManager::add(Actor<T>& a) {
        unsigned int i = actorCount.getIncrement();
        ActorId<T> res(this, i);
        a.setId(res);

        actorsLock.lock();
        actors[i] = &a;
        actorsLock.unlock();

        return res;
    }

    template<typename T> ActorId<T> ActorManager::add(Actor<T>* a, Timeout t) {
        EPYX_ASSERT(a != NULL);
        return this->add(*a, t);
    }

    template<typename T> ActorId<T> ActorManager::add(Actor<T>& a, Timeout t) {
        ActorId<T> res = this->add(a);
        this->timeouts.addTimeout(t, res.id);
        return res;
    }


    template<typename T> ActorId<T>::ActorId() {
        manager = NULL;
        id = -1;
    }

    template<typename T> ActorId<T>::ActorId(ActorManager* m, int i) {
        manager = m;
        id = i;
    }

    template<typename T> void ActorId<T>::post(T& msg) {
        manager->post(id, static_cast<void*> (&msg));
    }

    template<typename T> void ActorId<T>::kill() {
        manager->kill(*this);
    }

}

#endif /* EPYX_CORE_ACTOR_MANAGER_DETAIL_H */

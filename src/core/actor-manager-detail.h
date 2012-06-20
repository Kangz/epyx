/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
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

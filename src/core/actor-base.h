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
 * @file actor-base.h
 * @brief Definition of the base class of the actors, do not use directly
 */
#ifndef EPYX_CORE_ACTOR_BASE_H
#define EPYX_CORE_ACTOR_BASE_H

#include "mutex.h"

namespace Epyx
{
    class ActorManager;

    /**
     * @class Actor_base
     * @private
     */
    class Actor_base
    {
    public:
        Actor_base();
        virtual ~Actor_base();
        virtual void _internal_treat(void* msg) = 0;
        virtual void timeout();

        /**
         * @brief locks this actor
         *
         * DO NOT USE INSIDE treat() OR timeout()
         */
        void lock();

        /**
         * @brief unlocks this actor
         */
        void unlock();

        bool alive;

    private:
        Mutex mutex;

    };
}
#endif //EPYX_CORE_ACTOR_BASE_H

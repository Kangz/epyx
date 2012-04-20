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

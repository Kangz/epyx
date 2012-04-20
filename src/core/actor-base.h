#ifndef EPYX_CORE_ACTOR_BASE_H
#define EPYX_CORE_ACTOR_BASE_H

#include "mutex.h"

namespace Epyx
{
    class ActorManager;

    class Actor_base
    {
    public:
        Actor_base();
        virtual ~Actor_base();
        virtual void _internal_treat(void* msg) = 0;
        virtual void timeout();
        void lock();
        void unlock();

    private:
        Mutex mutex;

    };
}
#endif //EPYX_CORE_ACTOR_BASE_H

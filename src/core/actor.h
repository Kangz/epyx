#ifndef EPYX_CORE_ACTOR_H
#define EPYX_CORE_ACTOR_H

#include "actor-base.h"
#include "actor-manager.h"

namespace Epyx
{
    template<typename T>struct ActorId;

    template<typename T>class Actor : public Actor_base
    {
    public:
        virtual void _internal_treat(void* msg);
        void setId(ActorId<T> id);

    protected:
        virtual void treat(T& msg) = 0;

    private:
        ActorId<T> self;
    };


    template<typename T> void Actor<T>::_internal_treat(void* msg){
        T* message = static_cast<T*>(msg);
        this->treat(*message);
        delete message;
    }

    template<typename T> void Actor<T>::setId(ActorId<T> id){
        self = id;
    }
}
#endif //EPYX_CORE_ACTOR_H

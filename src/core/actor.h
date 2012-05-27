/**
 * @file actor.h
 * @brief Definition of the actor class that will need to be extended to mae your own actors
 */

#ifndef EPYX_CORE_ACTOR_H
#define EPYX_CORE_ACTOR_H

#include "actor-base.h"
#include "actor-manager.h"

#include "common.h"
namespace Epyx
{
    template<typename T>struct ActorId;

    /**
     * @class Actor
     *
     * @brief A way to implement you own actors
     *
     * Basically an actor can do only 2 things: receiving messages and timing out.
     * To do this overrride the two methods treat and timeout.
     *
     * @tparam T the base type of the messages the actor can receive
     */
    template<typename T>class Actor : public Actor_base
    {
    public:
        virtual void _internal_treat(void* msg);
        void setId(ActorId<T> id);
        ActorId<T> getId();

    protected:
        /**
         * @brief The method used to process incoming messages
         * @param msg the message
         */
        virtual void treat(T& msg) = 0;

        /**
         * @brief the method called when the actor times out
         */
        virtual void timeout();


        /**
         * @brief kills the actor (it won't timeout or receive other messages)
         */
        void kill();

    private:
        ActorId<T> self;

    };


    template<typename T> void Actor<T>::_internal_treat(void* msg) {
        if (msg == NULL) {
            this->timeout();
        } else {
            T* message = static_cast<T*> (msg);
            this->treat(*message);
            delete message;
        }
    }

    template<typename T> void Actor<T>::setId(ActorId<T> id) {
        self = id;
    }

    template<typename T> ActorId<T> Actor<T>::getId() {
        return self;
    }

    template<typename T> void Actor<T>::timeout() {}

    template<typename T> void Actor<T>::kill() {
        this->alive = false;
        this->self.manager->post(this->self.id, NULL);//Send a message to make sure the actor is destroyed
    }
}
#endif //EPYX_CORE_ACTOR_H

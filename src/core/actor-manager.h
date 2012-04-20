/**
 * @file actor-manager.h
 * @brief Definition of the actor framework
 */

#ifndef EPYX_CORE_ACTOR_MANAGER_H
#define EPYX_CORE_ACTOR_MANAGER_H

#include "actor.h"
#include "actor-base.h"
#include "mutex.h"
#include "worker-pool.h"
#include "atom/counter.h"
#include "timeout.h"
#include <string>
#include <map>
#include <vector>

namespace Epyx
{

    struct ActorId_base;
    template<typename T> struct ActorId;
    template<typename T> class Actor;
    /**
     * @class ActorManager
     *
     * @brief The framework that will run our actors
     *
     * The ActorManager takes care of running the actors, it's basic use
     * is to add an actor into it and then use the returned ActorId to
     * send messages or kill the actor. Actors can also be added with
     * a timeout.
     */
    class ActorManager
    {
    public:
        /**
         * @brief the ActorManager constructor
         * @param num_workers the number of threads to run the actors on
         * @param name the base name used in the logging system
         */
        ActorManager(int num_workers, const std::string& name);

        /**
         * @brief the ActorManager destructor
         */
        ~ActorManager();

        /**
         * @brief check is an Actor is still alive
         *
         * Note: the Actor may die just after the call, this method
         * only provide a hint.
         */
        void has(ActorId_base id);

        /**
         * @brief Adds an Actor (pointer no-timeout version)
         * @param a the actor to be added
         * @return An ActorId for this Actor
         *
         * Note: After this call you should only use the ActorId to
         * give commands to your actor.
         */
        template<typename T> ActorId<T> add(Actor<T>* a);

        /**
         * @brief Adds an Actor (reference no-timeout version)
         * @param a the actor to be added
         * @return An ActorId for this Actor
         *
         * Note: After this call you should only use the ActorId to
         * give commands to your actor.
         */
        template<typename T> ActorId<T> add(Actor<T>& a);

        /**
         * @brief Adds an Actor (pointer timeout version)
         * @param a the actor to be added
         * @param t int the time before the timeout in msec
         * @return An ActorId for this Actor
         *
         * Note: After this call you should only use the ActorId to
         * give commands to your actor.
         */
        template<typename T> ActorId<T> add(Actor<T>* a, Timeout t);

        /**
         * @brief Adds an Actor (reference timeout version)
         * @param a the actor to be added
         * @param t int the time before the timeout in msec
         * @return An ActorId for this Actor
         *
         * Note: After this call you should only use the ActorId to
         * give commands to your actor.
         */
        template<typename T> ActorId<T> add(Actor<T>& a, Timeout t);

        /**
         * @brief Kills an Actor contained in this manager, it won't receive anmore messges
         * @param id the ActorId of the Actor to be killed
         */
        void kill(ActorId_base id);

        void post(int id, void* msg);

    private:
        class ActorWorkers : public WorkerPool<std::pair<int, void*> >
        {
        public:

            ActorWorkers(int num_workers, const std::string& name, ActorManager* m);
            virtual void treat(std::pair<int, void*>* msg);

        private:
            ActorManager* manager;
        };

        ActorWorkers wp;
        std::map<int, Actor_base*> actors;
        Mutex actorsLock;
        atom::Counter actorCount;

        //The thread used to fire timeouts
        struct ActorTimeoutComparator
        {
            bool operator()(const std::pair<Timeout, int> a, const std::pair<Timeout, int> b);
        };

        class TimeoutLauncher: public Thread
        {
        public:
            TimeoutLauncher(ActorManager* m, const std::string& name);
            ~TimeoutLauncher();
            void addTimeout(Timeout t, int id);

        protected:
            virtual void run();

        private:
            ActorManager* manager;
            std::priority_queue<std::pair<Timeout, int>, std::vector<std::pair<Timeout, int> >, ActorTimeoutComparator > timeouts;
            BlockingQueue<std::pair<Timeout, int> > incoming;
        };

        TimeoutLauncher timeouts;
    };
    /**
     * @struct ActorId_base
     * @brief the base class of the ActorId, do not use directly
     */
    struct ActorId_base
    {
        int id;
        ActorManager* manager;
    };
    /**
     * @struct ActorId
     * @brief Identifies and sends commands to an Actor
     * @tparam T the template parameter of the corresponding actor
     */
    template<typename T> struct ActorId : public ActorId_base
    {
        /**
         * @brief ActorId's default constructor
         */
        ActorId();

        /**
         * @brief ActorId's other constructor
         */
        ActorId(ActorManager* m, int i);

        /**
         * @brief sends a message to its Actor
         * @param msg the message
         */
        void post(T & msg);

        /**
         * @brief kills its Actor
         */
        void kill();
    };

}
#include "actor-manager-detail.h"

#endif /* EPYX_CORE_ACTOR_MANAGER_H */

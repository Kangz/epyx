/**
 * @file thread.h
 * @brief Thread management astraction layer
 */
#ifndef EPYX_THREAD_H
#define EPYX_THREAD_H

#include <string>
#include <pthread.h>
#include "tls-pointer.h"

namespace Epyx
{
    namespace detail
    {
        struct ThreadInfo
        {
            std::string name;
        };
        /**
         * @private thread_infos
         */
        extern TLSPointer<ThreadInfo>* thread_infos;
        /**
         * @private thread_initialized
         */
        extern bool thread_initialized;
    }
    /**
     * @class Thread
     *
     * @brief Thread management system
     *
     * To define a thread, you should inherit from this abstract class.
     */
    class Thread
    {
    public:

        /**
         * @brief Thread constructor
         */
        Thread();

        /**
         * @brief Give a name in the constructor
         * @param name given name
         * @param id Thread ID
         */
        Thread(const std::string& name, int id = -1);

        /**
         * @brief Basic destructor, which terminates the thread
         */
        virtual ~Thread();

        /**
         * @brief Set name-id if thread was created without one
         * @param name given name
         * @param id Thread ID
         */
        void setName(const std::string& name, int id = -1);

        /**
         * @brief Start the thread
         */
        bool start();

        /**
         * @brief Wait until the end
         */
        void wait();

        /**
         * @brief Terminate a thread
         */
        void term();

        /**
         * @brief Get the name of this thread
         * @return this->name
         */
        const std::string& getThisName();

        /**
         * @brief Get the name of the running thread
         * @return [current thread]->name, not this->name
         */
        static std::string getName();

        /**
         * @brief Intialise the thread system
         * @param name Name of the main thread
         * @param id ID of the main thread
         */
        static void init(const std::string& name = "Main", int id = -1);
        /**
         * @brief Tell wether the thread system is initialised
         * @return true if Thread::init() was called
         */
        static bool isInitialized();

    protected:
        /**
         * @brief Thread entry point
         */
        virtual void run() = 0;

    private:
        // Disable copy construction and assignment.
        Thread(const Thread&);
        const Thread& operator=(const Thread&);

        /**
         * @brief Thread entry point
         * @param Thread object
         * @return NULL
         */
        static void* _thread_start(void*);

        //And this is the ofstream of each stream
        pthread_t thread;
        detail::ThreadInfo* info;
    };
}

#endif /* EPYX_THREAD_H */

/**
 * @file mutex.h
 * @brief mutex definition.
 */
#ifndef EPYX_MUTEX_H
#define EPYX_MUTEX_H

#include <pthread.h>
#include <errno.h>

namespace Epyx
{
    /**
     * @class Mutex
     *
     * @brief A mutual exclusion lock "Mutex"
     *
     * This class is simply a wrapper around pthreads' mutex object to ease its use.
     */
    class Mutex
    {
    public:
        /**
         * @brief The Mutex constructor
         */
        Mutex();

        /**
         * @brief The Mutex destructor
         */
        ~Mutex();

        /**
         * @brief locks the Mutex
         */
        void lock();

        /**
         * @brief unlocks the previously locked Mutex
         */
        void unlock();

        /**
         * @brief tries to lock the Mutex
         * @return true if the Mutex was successfully locked, false otherwise
         */
        bool tryLock();

        /**
         * @brief an accessor to pthreads' internal mutex representation
         * @return this representation
         */
        pthread_mutex_t* getInternal();

    private:
        pthread_mutex_t mutex;

        // Disable copy construction and assignment.
        Mutex(const Mutex&);
        const Mutex& operator=(const Mutex&);
    };
}

#include "mutex-detail.h"

#endif /* EPYX_MUTEX_H */

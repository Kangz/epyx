/**
 * Mutex management astraction layer
 */
#ifndef EPYX_MUTEX_H
#define EPYX_MUTEX_H

#include <pthread.h>
#include <errno.h>
#include "assert.h"
#include "exception.h"

namespace Epyx
{
    class Mutex
    {
    public:
        Mutex();
        ~Mutex();
        void lock();
        void unlock();
        bool tryLock();
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

/**
 * Mutex management astraction layer
 */
#ifndef EPYX_MUTEX_H
#define EPYX_MUTEX_H

#include <pthread.h>
#include "exception.h"

namespace Epyx
{
    class Mutex
    {
    private:
        pthread_mutex_t mutex;

        // Disable copy construction and assignment.
        Mutex(const Mutex&);
        const Mutex& operator=(const Mutex&);

    public:
        inline Mutex()
        {
            int status = pthread_mutex_init(&(this->mutex), NULL);
            if (status)
                throw FailException("Mutex", "pthread_mutex init error");
        }

        inline ~Mutex()
        {
            int status = pthread_mutex_destroy(&(this->mutex));
            if (status)
                throw FailException("Mutex", "pthread_mutex destroy error");
        }

        inline void lock()
        {
            int status = pthread_mutex_lock(&(this->mutex));
            if (status)
                throw FailException("Mutex", "pthread_mutex lock error");
        }

        inline void unlock()
        {
            int status = pthread_mutex_unlock(&(this->mutex));
            if (status)
                throw FailException("Mutex", "pthread_mutex unlock error");
        }

        inline pthread_mutex_t* getInternal()
        {
            return &mutex;
        }
    };
}

#endif /* EPYX_THREAD_H */

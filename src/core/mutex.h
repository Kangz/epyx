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
    private:
        pthread_mutex_t mutex;

        // Disable copy construction and assignment.
        Mutex(const Mutex&);
        const Mutex& operator=(const Mutex&);

    public:
        inline Mutex()
        {
            int mutex_init_status = pthread_mutex_init(&(this->mutex), NULL);
            EPYX_ASSERT_NO_LOG(mutex_init_status == 0);
        }

        inline ~Mutex()
        {
            int mutex_destroy_status = pthread_mutex_destroy(&(this->mutex));
            EPYX_ASSERT_NO_LOG(mutex_destroy_status == 0);
        }

        inline void lock()
        {
            int mutex_lock_status = pthread_mutex_lock(&(this->mutex));
            EPYX_ASSERT_NO_LOG(mutex_lock_status == 0);
        }

        inline void unlock()
        {
            int mutex_unlock_status = pthread_mutex_unlock(&(this->mutex));
            EPYX_ASSERT_NO_LOG(mutex_unlock_status == 0);
        }

        //Returns true if it successfully locked the mutex)
        inline bool tryLock()
        {
            int mutex_trylock_status = pthread_mutex_trylock(&(this->mutex));
            EPYX_ASSERT_NO_LOG(mutex_trylock_status == 0 || mutex_trylock_status == EBUSY);
            return mutex_trylock_status != EBUSY;
        }

        inline pthread_mutex_t* getInternal()
        {
            return &mutex;
        }
    };
}

#endif /* EPYX_THREAD_H */

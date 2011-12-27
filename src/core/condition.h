/**
 * Thread condition management astraction layer
 * It is not exposing the full pthread capabilities on purpose to make it simpler
 * even if it might create small overheads
 */
#ifndef EPYX_CONDITION_H
#define EPYX_CONDITION_H

#include <pthread.h>
#include "mutex.h"

//TODO: add documentation

namespace Epyx {
    class Condition {
    private:
        Mutex mutex;
        pthread_cond_t cond;

        // Disable copy construction and assignment.
        Condition(const Condition&);
        const Condition& operator=(const Condition&);

    public:
        Condition();
        ~Condition();

        void lock();
        void unlock();
        void wait();

        //Wait until the condition is fired or the timeout is reached
        //Returns true if the timeout was fired false otherwise
        bool timedWait(int seconds, int nanoseconds);

        //This is the preferred version with a parameter in milliseconds
        bool timedWait(int ms);

        void notify();
        void notifyAll();
    };
}

#endif /* EPYX_CONDITION_H */

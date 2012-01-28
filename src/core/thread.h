/**
 * Thread management astraction layer
 */
#ifndef EPYX_THREAD_H
#define EPYX_THREAD_H

#include <pthread.h>
#include "runnable.h"

namespace Epyx {

    class Thread {
    private:
        static void* _thread_start(void*);

        Runnable* rn;
        pthread_t thread;

    public:
        Thread(Runnable* rn_);

        /**
         * Start the thread
         */
        bool run();

        /**
         * Wait until the end
         */
        void wait();

        /**
         * Terminate a thread
         */
        void term();
    };
}

#endif /* EPYX_THREAD_H */

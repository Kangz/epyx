/**
 * Thread management astraction layer
 */
#ifndef EPYX_THREAD_H
#define EPYX_THREAD_H

#include <string>
#include <pthread.h>
#include "runnable.h"
#include "tls-pointer.h"

namespace Epyx {

    namespace detail {
        struct ThreadInfo {
            std::string name;
            int id;
        };
        extern TLSPointer<ThreadInfo>* thread_infos;
    }

    class Thread {
    private:
        static void* _thread_start(void*);

        //And this is the ofstream of each stream
        Runnable* rn;
        pthread_t thread;
        detail::ThreadInfo* info;

    public:
        Thread(Runnable* rn_, std::string name, int id = -1);

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

        static std::string getName();
        static int getId();

        static void init(std::string name = "Main", int id = -1);
    };
}

#endif /* EPYX_THREAD_H */

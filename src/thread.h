/**
 * Thread management system
 */

#include <pthread.h>

namespace Epyx
{
    typedef void (ThreadFn)(void*);
    
    class Thread
    {
    private:
        pthread_t thread;
    public:
        ThreadFn *fn;
        void *arg;

        Thread(ThreadFn fn_, void *arg_);
        
        /**
         * Start the thread
         */
        bool run();

        /**
         * Wait until the end
         */
        void wait();
    };
}

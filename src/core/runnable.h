/**
 * Runnable object for threads
 */
#ifndef EPYX_RUNNABLE_H
#define EPYX_RUNNABLE_H

namespace Epyx {

    class Runnable {
    public:
        virtual void run() = 0;
    };

}

#endif /* EPYX_RUNNABLE_H */

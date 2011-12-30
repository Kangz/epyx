/**
 */
#ifndef EPYX_BLOCKING_QUEUE_H
#define EPYX_BLOCKING_QUEUE_H

#include <queue>
#include "condition.h"

//TODO: add documentation

namespace Epyx {
    template<typename T> class BlockingQueue {
    private:
        Condition cond;
        std::queue<T> fifo;

        // Disable copy construction and assignment.
        BlockingQueue(const BlockingQueue&);
        const BlockingQueue& operator=(const BlockingQueue&);

    public:
        BlockingQueue();
        ~BlockingQueue();

        void push(T& e);
        bool tryPush(T& e);
        T* pop();
        T* pop(int msec);        
        T* tryPop();

        //Do not implement the following methods due to a lack of atomic operations
        //int length();
        //bool isEmpty();
    };
}

#endif /* EPYX_BLOCKING_QUEUE_H */

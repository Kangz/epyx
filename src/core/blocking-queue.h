/**
 * Blocking queue implementation
 * TODO: add documentation
 */
#ifndef EPYX_BLOCKING_QUEUE_H
#define EPYX_BLOCKING_QUEUE_H

#include <queue>
#include <iostream>
#include "assert.h"
#include "condition.h"

namespace Epyx {
    template<typename T> class BlockingQueue {
    public:
        BlockingQueue();
        ~BlockingQueue();

        void close();
        bool isOpened();

        bool push(T* e);
        bool tryPush(T* e);
        T* pop();
        T* pop(int msec);
        T* tryPop();

        std::deque<T*>* flush();
        std::deque<T*>* flush(int msec);
        std::deque<T*>* tryFlush();

        //Do not implement the following methods due to a lack of atomic operations
        //TODO: do it
        //int length();
        //bool isEmpty();

    private:
        Condition cond;
        std::deque<T*> fifo;
        bool opened;

        // Disable copy construction and assignment.
        BlockingQueue(const BlockingQueue&);
        const BlockingQueue& operator=(const BlockingQueue&);

    };
}

#include "blocking-queue-detail.h"

#endif /* EPYX_BLOCKING_QUEUE_H */

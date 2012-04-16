/**
 * @file blocking-queue.h
 * @brief a blocking queue definition.
 */
#ifndef EPYX_BLOCKING_QUEUE_H
#define EPYX_BLOCKING_QUEUE_H

#include <queue>
#include <iostream>
#include "assert.h"
#include "condition.h"

namespace Epyx
{
    /**
     * @class BlockingQueue
     *
     * @brief A thread-safe fifo container for pointers
     *
     * Access to the blocking queue can be synchronous or asynchronous which makes it good
     * for passing messages from thread to threads. It stores pointers to the template parameter.
     * The thread popping a pointer is responsible for deleting it however when the queue is
     * destroyed it will delete all the objects it still has in memory. Thus the pushing thread
     * should not keep a reference to an object "stored" in the queue.
     *
     * @tparam T the base type of the pointers contained in the BlockingQueue
     */

    template<typename T> class BlockingQueue
    {
    public:

        /**
         * @brief The BlockingQueue constructor
         */
        BlockingQueue();

        /**
         * @brief The BlockingQueue destructor
         *
         * It will automatically delete all the remaining elements stored in the queue.
         */
        ~BlockingQueue();

        /**
         * @brief Closes the queue prior to deletion
         *
         * This is to be used to notify the threads waiting on the queue
         * as it makes all the pending pop() return NULL.
         */
        void close();

        /**
         * @brief Returns false if the queue was closed
         * @return returns true if close() has been called, false otherwise
         */
        bool isOpened();

        /**
         * @brief Pushes an element on the queue, the synchronous way
         * @param e a pointer to the element to push
         * @return false if the queue is closed, true otherwise
         */
        bool push(T* e);

        /**
         * @brief Pushes an element on the queue, the best-effort way
         *
         * This is not a really asynchronous call, it will try to lock the queue
         * and then push the element.
         *
         * @param e a pointer to the element to push
         * @return true if the queue is not closed and the element was pushed, false otherwise
         */
        bool tryPush(T* e);

        /**
         * @brief Pops an element, the synchronous way
         * @return An element or NULL if the queue gets closed
         */
        T* pop();

        /**
         * @brief Pops an element, the timed way
         *
         * This version of pop waits for an element or the specified time before returning.
         *
         * @param msec the time to wait in milliseconds
         * @return An element or NULL if the queue gets closed or the time runs out
         */
        T* pop(int msec);

        /**
         * @brief Pops an element, the best-effort way
         *
         * This is not a really asynchronous call, it will try to lock the queue
         * and then pop an element.
         *
         * @return An element or NULL if the queue was closed or if it could not get the lock
         */
        T* tryPop();

        /**
         * @brief Returns a copy of the queue and empty it, the synchronous way
         * @return A copy of the content of the queue or NULL if the queue gets closed
         */
        std::deque<T*>* flush();

        /**
         * @brief Returns a copy of the queue and empty it, the timed way
         *
         * This version of flush waits for the lock or the specified time before returning.
         *
         * @param msec the time to wait in milliseconds
         * @return A copy of the content of the queue or NULL if the queue gets closed or the time runs out
         */
        std::deque<T*>* flush(int msec);

        /**
         * @brief Returns a copy of the queue and empty it, the best-effort way
         *
         * This is not a really asynchronous call, it will try to lock the queue
         * and then flush it.
         *
         * @return A copy of the content of the queue or NULL if the queue was closed or if it could not get the lock
         */
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

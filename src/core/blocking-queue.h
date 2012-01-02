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

    template<typename T> BlockingQueue<T>::BlockingQueue() {}
    template<typename T> BlockingQueue<T>::~BlockingQueue() {} //Who frees the elements ?

    template<typename T> void BlockingQueue<T>::push(T& e) {
        cond.lock();
        fifo.push(e);
        cond.notify();
        cond.unlock();
    }

    template<typename T> bool BlockingQueue<T>::tryPush(T& e) {
        if(cond.tryLock()){
            fifo.push(e);
            cond.notify();
            cond.unlock();
            return true;
        }
        return false;
    }

    template<typename T> T* BlockingQueue<T>::pop(){
        cond.lock();
        while(fifo.empty()){
            cond.wait();
        }
        T& result = fifo.front();
        fifo.pop();
        return &result;
    }

    template<typename T> T* BlockingQueue<T>::pop(int msec){
        cond.lock();
        while(fifo.empty()){
            cond.timedWait(msec);
        }
        T& result = fifo.front();
        fifo.pop();
        return &result;
    }

    template<typename T> T* BlockingQueue<T>::tryPop(){
        if(cond.tryLock()){
            T& result = fifo.front();
            fifo.pop();
            cond.unlock();
            return &result;
        }
        return NULL;
    }
}

#endif /* EPYX_BLOCKING_QUEUE_H */

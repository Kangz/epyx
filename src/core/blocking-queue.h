/**
 * Blocking queue implementation
 * TODO: add documentation
 */
#ifndef EPYX_BLOCKING_QUEUE_H
#define EPYX_BLOCKING_QUEUE_H

#include "assert.h"
#include "condition.h"
#include <queue>
#include <iostream>

namespace Epyx {
    template<typename T> class BlockingQueue {
    private:
        Condition cond;
        std::deque<T> fifo;
        bool opened;

        // Disable copy construction and assignment.
        BlockingQueue(const BlockingQueue&);
        const BlockingQueue& operator=(const BlockingQueue&);

    public:
        BlockingQueue();
        ~BlockingQueue();

        void close();
        bool isOpened();

        bool push(const T& e);
        bool tryPush(const T& e);
        T* pop();
        T* pop(int msec);
        T* tryPop();

        std::deque<T>* flush();
        std::deque<T>* flush(int msec);
        std::deque<T>* tryFlush();


        //Do not implement the following methods due to a lack of atomic operations
        //TODO: do it
        //int length();
        //bool isEmpty();

    };

    template<typename T> BlockingQueue<T>::BlockingQueue()
        :opened(true)
    {
    }

    template<typename T> BlockingQueue<T>::~BlockingQueue()
    {
        // FIXME: Who frees the elements ?
    }

    template<typename T> void BlockingQueue<T>::close()
    {
        opened = false;
        // Unlock codition
        cond.notify();
    }

    template<typename T> bool BlockingQueue<T>::isOpened()
    {
        return opened;
    }

    template<typename T> bool BlockingQueue<T>::push(const T& e) {
        if(!opened)
            return false;
        cond.lock();
        fifo.push_back(e);
        cond.notify();
        cond.unlock();
        return true;
    }

    template<typename T> bool BlockingQueue<T>::tryPush(const T& e) {
        if(opened && cond.tryLock()){
            fifo.push_back(e);
            cond.notify();
            cond.unlock();
            return true;
        }
        return false;
    }

    template<typename T> T* BlockingQueue<T>::pop(){
        cond.lock();
        while(opened && fifo.empty()){
            cond.wait();
        }
        // Continue to pop even if queue is closed
        if(fifo.empty()){
            cond.unlock();
            return NULL;
        }
        T* result = new T(fifo.front());
        fifo.pop_front();
        cond.unlock();
        return result;
    }

    template<typename T> T* BlockingQueue<T>::pop(int msec){
        cond.lock();
        if(opened && fifo.empty()){
            cond.timedWait(msec);
        }
        if(fifo.empty()){
            cond.unlock();
            return NULL;
        }
        T* result = new T(fifo.front());
        fifo.pop_front();
        cond.unlock();
        return result;
    }

    template<typename T> T* BlockingQueue<T>::tryPop(){
        if(opened && cond.tryLock()){
            if(fifo.empty()){
                cond.unlock();
                return NULL;
            }
            T* result = new T(fifo.front());
            fifo.pop_front();
            cond.unlock();
            return result;
        }
        return NULL;
    }

    template<typename T> std::deque<T>* BlockingQueue<T>::flush(){
        cond.lock();
        while(opened && fifo.empty()){
            cond.wait();
        }
        if(fifo.empty()){
            cond.unlock();
            return NULL;
        }
        std::deque<T>* result = new std::deque<T>(fifo);
        fifo.clear();
        cond.unlock();
        return result;
    }

    template<typename T> std::deque<T>* BlockingQueue<T>::flush(int msec){
        cond.lock();
        if(opened && fifo.empty()){
            cond.timedWait(msec);
        }
        if(fifo.empty()){
            cond.unlock();
            return NULL;
        }
        std::deque<T>* result = new std::deque<T>(fifo);
        fifo.clear();
        cond.unlock();
        return result;
    }

    template<typename T> std::deque<T>* BlockingQueue<T>::tryFlush(){
        if(opened && cond.tryLock()){
            if(fifo.empty()){
                cond.unlock();
                return NULL;
            }
            std::deque<T>* result = new std::deque<T>(fifo);
            fifo.clear();
            cond.unlock();
            return result;
        }
        return NULL;
    }


}

#endif /* EPYX_BLOCKING_QUEUE_H */

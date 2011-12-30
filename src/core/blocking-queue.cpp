#include "blocking-queue.h"

namespace Epyx {
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
            cond.wait(msec);
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

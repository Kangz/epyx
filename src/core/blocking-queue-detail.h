/**
 * @file blocking-queue-detail.h
 * @brief a blocking queue implementation
 *
 * You should never include this file directly
 */
#ifndef EPYX_BLOCKING_QUEUE_DETAIL_H
#define EPYX_BLOCKING_QUEUE_DETAIL_H

namespace Epyx
{

    template<typename T> BlockingQueue<T>::BlockingQueue()
    : opened(true) {
    }

    template<typename T> BlockingQueue<T>::~BlockingQueue() {
        // FIXME: Who frees the elements ?
    }

    template<typename T> void BlockingQueue<T>::close() {
        this->opened = false;
        // Unlock condition
        cond.lock();
        cond.notifyAll();
        cond.unlock();
    }

    template<typename T> bool BlockingQueue<T>::isOpened() {
        return this->opened;
    }

    template<typename T> bool BlockingQueue<T>::push(T* e) {
        if (!opened)
            return false;
        cond.lock();
        fifo.push_back(e);
        cond.notify();
        cond.unlock();
        return true;
    }

    template<typename T> bool BlockingQueue<T>::tryPush(T* e) {
        if (opened && cond.tryLock()) {
            fifo.push_back(e);
            cond.notify();
            cond.unlock();
            return true;
        }
        return false;
    }

    template<typename T> T* BlockingQueue<T>::pop() {
        cond.lock();
        while (opened && fifo.empty()) {
            cond.wait();
        }
        // Continue to pop even if queue is closed
        if (fifo.empty()) {
            cond.unlock();
            return NULL;
        }
        T* result = fifo.front();
        fifo.pop_front();
        cond.unlock();
        return result;
    }

    template<typename T> T* BlockingQueue<T>::pop(int msec) {
        cond.lock();
        if (opened && fifo.empty()) {
            cond.timedWait(msec);
        }
        if (fifo.empty()) {
            cond.unlock();
            return NULL;
        }
        T* result = fifo.front();
        fifo.pop_front();
        cond.unlock();
        return result;
    }

    template<typename T> T* BlockingQueue<T>::tryPop() {
        if (opened && cond.tryLock()) {
            if (fifo.empty()) {
                cond.unlock();
                return NULL;
            }
            T* result = fifo.front();
            fifo.pop_front();
            cond.unlock();
            return result;
        }
        return NULL;
    }

    template<typename T> std::deque<T*>* BlockingQueue<T>::flush() {
        cond.lock();
        while (opened && fifo.empty()) {
            cond.wait();
        }
        if (fifo.empty()) {
            cond.unlock();
            return NULL;
        }
        std::deque<T*>* result = new std::deque<T*>(fifo);
        fifo.clear();
        cond.unlock();
        return result;
    }

    template<typename T> std::deque<T*>* BlockingQueue<T>::flush(int msec) {
        cond.lock();
        if (opened && fifo.empty()) {
            cond.timedWait(msec);
        }
        if (fifo.empty()) {
            cond.unlock();
            return NULL;
        }
        std::deque<T*>* result = new std::deque<T*>(fifo);
        fifo.clear();
        cond.unlock();
        return result;
    }

    template<typename T> std::deque<T*>* BlockingQueue<T>::tryFlush() {
        if (opened && cond.tryLock()) {
            if (fifo.empty()) {
                cond.unlock();
                return NULL;
            }
            std::deque<T*>* result = new std::deque<T*>(fifo);
            fifo.clear();
            cond.unlock();
            return result;
        }
        return NULL;
    }
}

#endif /* EPYX_BLOCKING_QUEUE_DETAIL_H */

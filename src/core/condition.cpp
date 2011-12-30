#include "condition.h"
#include <errno.h>
#include <sys/time.h>
#include "exception.h"

namespace Epyx {
    Condition::Condition() {
        //The mutex is automatically initialized

        int status = pthread_cond_init(&(this->cond), NULL);
        if (status) throw FailException("Condition", "pthread_cond init error");
    }

    Condition::~Condition() {
        //The mutex is automatically destroyed
        int status = pthread_cond_destroy(&(this->cond));
        if (status) throw FailException("Condition", "pthread_cond_destroy error");
    }

    void Condition::lock() {
        this->mutex.lock();
    }

    bool Condition::tryLock() {
        return this->mutex.tryLock();
    }

    void Condition::unlock() {
        this->mutex.unlock();
    }

    void Condition::wait() {
        int status = pthread_cond_wait(&this->cond, this->mutex.getInternal());
        if (status) throw FailException("Condition", "pthread_cond_wait error");
    }

    bool Condition::timedWait(int seconds, int nanoseconds) {
        timespec limit;
        timeval current;
        const unsigned int billion = 1000000000;

        //Find the absolute limit time
        int status = gettimeofday(&current, NULL);
        if (status) throw FailException("Condition", "gettimeofday error");
        limit.tv_sec = current.tv_sec + seconds;
        limit.tv_nsec = current.tv_usec * 1000 + nanoseconds;
        limit.tv_sec += limit.tv_nsec / billion;
        limit.tv_nsec %= billion;

        status = pthread_cond_timedwait(&this->cond, this->mutex.getInternal(), &limit);
        if (status && status != ETIMEDOUT) throw FailException("Condition", "pthread_cond_timedwait error");

        return status == ETIMEDOUT;
    }

    bool Condition::timedWait(int ms) {
        return this->timedWait(ms/1000, (ms%1000)*1000000);
    }

    void Condition::notify() {
        int status = pthread_cond_signal(&(this->cond));
        if (status) throw FailException("Condition", "pthread_cond_signal error");
    }

    void Condition::notifyAll() {
        int status = pthread_cond_broadcast(&(this->cond));
        if (status) throw FailException("Condition", "pthread_cond_broadcast error");
    }

    Mutex& Condition::getMutex() {
        return this->mutex;
    }

}

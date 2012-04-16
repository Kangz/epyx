#include "condition.h"
#include "assert.h"
#include "log.h"
#include <errno.h>
#include <sys/time.h>

namespace Epyx
{

    Condition::Condition() {
        //The mutex is automatically initialized
        int cond_init_status = pthread_cond_init(&(this->cond), NULL);
        EPYX_ASSERT(cond_init_status == 0);
    }

    Condition::~Condition() {
        //The mutex is automatically destroyed
        int cond_destroy_status = pthread_cond_destroy(&(this->cond));
        EPYX_ASSERT(cond_destroy_status == 0);
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
        int cond_wait_status = pthread_cond_wait(&this->cond, this->mutex.getInternal());
        EPYX_ASSERT(cond_wait_status == 0);
    }

    bool Condition::timedWait(int seconds, int nanoseconds) {
        timespec limit;
        timeval current;
        const unsigned int billion = 1000000000;

        //Find the absolute limit time
        int gettimeofday_status = gettimeofday(&current, NULL);
        EPYX_ASSERT(gettimeofday_status == 0);
        limit.tv_sec = current.tv_sec + seconds;
        limit.tv_nsec = current.tv_usec * 1000 + nanoseconds;
        limit.tv_sec += limit.tv_nsec / billion;
        limit.tv_nsec %= billion;

        int cond_timedwait_status = pthread_cond_timedwait(&this->cond, this->mutex.getInternal(), &limit);
        EPYX_ASSERT(cond_timedwait_status == 0 || cond_timedwait_status == ETIMEDOUT);

        return cond_timedwait_status == ETIMEDOUT;
    }

    bool Condition::timedWait(int ms) {
        return this->timedWait(ms / 1000, (ms % 1000)*1000000);
    }

    void Condition::notify() {
        int cond_signal_status = pthread_cond_signal(&(this->cond));
        EPYX_ASSERT(cond_signal_status == 0);
    }

    void Condition::notifyAll() {
        int cond_broadcast_status = pthread_cond_broadcast(&(this->cond));
        EPYX_ASSERT(cond_broadcast_status == 0);
    }

    Mutex& Condition::getMutex() {
        return this->mutex;
    }

}

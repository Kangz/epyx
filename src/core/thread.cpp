#include "thread.h"
#include "exception.h"

namespace Epyx
{
    Thread::Thread(Runnable* rn_): rn(rn_), thread(0){}

    bool Thread::run()
    {
        int status = pthread_create(&(this->thread), NULL, Thread::_thread_start, this);
        if (status)
            throw FailException("Thread", "pthread create error");
        if (!this->thread)
            throw FailException("Thread", "pthread succeeded in creating no thread");
        return true;
    }

    void Thread::wait()
    {
        if (this->thread == 0)
            throw FailException("Thread", "no thread to be waited for");
        int status = pthread_join(this->thread, NULL);
        if (status)
            throw FailException("Thread", "pthread join error");
    }

    void Thread::term()
    {
        int status = pthread_cancel(this->thread);
        if (status)
            throw FailException("Thread", "pthread cancel error");
        this->thread = 0;
    }

    void* Thread::_thread_start(void *arg)
    {
        Epyx::Thread *self = (Epyx::Thread*) arg;
        self->rn->run();
        return NULL;
    }

}

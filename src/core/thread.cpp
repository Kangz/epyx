#include "thread.h"
#include "exception.h"

extern "C"
{
    static void *thread_main (void *arg)
    {
        Epyx::Thread *self = (Epyx::Thread*) arg;
        self->fn (self->arg);
        return NULL;
    }
}

namespace Epyx
{
    Thread::Thread(ThreadFn fn_, void *arg_)
    {
        this->fn = fn_;
        this->arg = arg_;
        this->thread = 0;
    }

    bool Thread::run()
    {
        int status = pthread_create(&(this->thread), NULL, thread_main, this);
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
}

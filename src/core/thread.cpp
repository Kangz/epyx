#include "log.h"
#include "thread.h"
#include "exception.h"

namespace Epyx
{
    TLSPointer<detail::ThreadInfo>* detail::thread_infos = NULL;
    bool detail::thread_initialized = false;

    Thread::Thread(Runnable* rn_, std::string name, int id): rn(rn_), thread(0), info(NULL){
        //Set up the thread info dor this thread
        info = new detail::ThreadInfo();
        info->name = name;
        info->id = id;
    }

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

    std::string Thread::getName()
    {
        return detail::thread_infos->get()->name;
    }

    int Thread::getId()
    {
        return detail::thread_infos->get()->id;
    }


    void Thread::init(std::string name, int id)
    {
        detail::thread_infos = new TLSPointer<detail::ThreadInfo>();

        //Add the main thread's info
        detail::ThreadInfo* main_ti = new detail::ThreadInfo();
        main_ti->name = name;
        main_ti->id = id;
        detail::thread_infos->reset(main_ti);

        detail::thread_initialized = true;
    }

    bool Thread::isInitialized()
    {
        return detail::thread_initialized;
    }

    void* Thread::_thread_start(void *arg)
    {
        Epyx::Thread *self = (Epyx::Thread*) arg;

        detail::thread_infos->reset(self->info);
        try {
            self->rn->run();
        } catch (Exception e) {
            log::fatal << e << log::endl;
        }
        return NULL;
    }
}

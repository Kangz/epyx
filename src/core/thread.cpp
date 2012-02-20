#include "thread.h"
#include "assert.h"
#include "log.h"
#include "exception.h"

namespace Epyx
{
    TLSPointer<detail::ThreadInfo>* detail::thread_infos = NULL;
    bool detail::thread_initialized = false;

    Thread::Thread(Runnable* rn, std::string name, int id)
        :rn(rn), thread(0), info(NULL)
    {
        //Set up the thread info dor this thread
        info = new detail::ThreadInfo();
        info->name = name;
        info->id = id;
    }

    bool Thread::run()
    {
        int thread_create_status = pthread_create(&(this->thread), NULL,
                                                  Thread::_thread_start, this);
        EPYX_ASSERT(thread_create_status == 0);
        EPYX_ASSERT(this->thread != 0);
        return true;
    }

    void Thread::wait()
    {
        EPYX_ASSERT(this->thread != 0);
        int thread_join_status = pthread_join(this->thread, NULL);
        EPYX_ASSERT(thread_join_status == 0);
    }

    void Thread::term()
    {
        int thread_cancel_status = pthread_cancel(this->thread);
        EPYX_ASSERT(thread_cancel_status == 0);
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
        try {
            Epyx::Thread *self = (Epyx::Thread*) arg;
            EPYX_ASSERT(self != NULL);
            detail::thread_infos->reset(self->info);
            self->rn->run();
        } catch (Exception e) {
            log::fatal << "Thread exception !" << log::endl;
            log::fatal << e << log::endl;
        }
        return NULL;
    }
}

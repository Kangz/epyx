#include "thread.h"
#include "assert.h"
#include "log.h"
#include "exception.h"

namespace Epyx
{
    TLSPointer<detail::ThreadInfo>* detail::thread_infos = NULL;
    bool detail::thread_initialized = false;

    Thread::Thread()
    :thread(0), info(NULL) {
    }

    Thread::Thread(const std::string& name, int id)
    :thread(0), info(NULL) {
        this->setName(name, id);
    }

    Thread::~Thread() {
        if (this->thread != 0)
            this->term();
    }

    void Thread::setName(const std::string& name, int id) {
        //Set up oly once the thread info for this thread
        EPYX_ASSERT(info == NULL);
        info = new detail::ThreadInfo();
        // Copy name
        if (id != -1) {
            std::ostringstream str;
            str << name << " " << id;
            info->name = str.str();
        } else {
            info->name = name;
        }
    }

    const std::string& Thread::getThisName() {
        EPYX_ASSERT(info != NULL);
        return info->name;
    }

    bool Thread::start() {
        EPYX_ASSERT(info != NULL);
        int thread_create_status = pthread_create(&(this->thread), NULL,
            Thread::_thread_start, this);
        EPYX_ASSERT(thread_create_status == 0);
        EPYX_ASSERT(this->thread != 0);
        return true;
    }

    void Thread::wait() {
        EPYX_ASSERT(this->thread != 0);
        int thread_join_status = pthread_join(this->thread, NULL);
        EPYX_ASSERT(thread_join_status == 0);
    }

    void Thread::term() {
        if (this->thread != 0) {
            int thread_cancel_status = pthread_cancel(this->thread);
            // Thread may have disappeared
            EPYX_ASSERT(thread_cancel_status == 0 || thread_cancel_status == ESRCH);
            this->thread = 0;
        }
    }

    std::string Thread::getName() {
        return detail::thread_infos->get()->name;
    }

    void Thread::init(const std::string& name, int id) {
        detail::thread_infos = new TLSPointer<detail::ThreadInfo > ();

        //Add the main thread's info
        detail::ThreadInfo* main_ti = new detail::ThreadInfo();
        main_ti->name = name;
        detail::thread_infos->reset(main_ti);

        detail::thread_initialized = true;
    }

    bool Thread::isInitialized() {
        return detail::thread_initialized;
    }

    void* Thread::_thread_start(void *arg) {
        try {
            // Call Thread::run()
            Epyx::Thread *self = (Epyx::Thread*) arg;
            EPYX_ASSERT(self != NULL);
            detail::thread_infos->reset(self->info);
            self->run();
        } catch (Exception e) {
            log::fatal << "Thread exception !" << log::endl;
            log::fatal << e << log::endl;
        }
        return NULL;
    }
}

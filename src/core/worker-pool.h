#ifndef EPYX_WORKER_POOL_H
#define EPYX_WORKER_POOL_H

#include <list>
#include <string>
#include <sstream>
#include "common.h"

namespace Epyx {

    template<typename T>class WorkerPool{
    public:
        WorkerPool(int num_workers, std::string name);
        ~WorkerPool();

        void post(T& message);

        int getNumWorkers();

        void setNumWorkers(int n);

        virtual void treat(T& message) = 0;

    private:
        WorkerPool(const WorkerPool&);
        const WorkerPool& operator=(const WorkerPool&);

        //Everything related to names
        std::string name;
        std::string worker_name_prefix;
        int worker_name_counter;

        void addWorker();
        void removeWorker();
        void bookKeep();

        BlockingQueue<T> messages;

        //The Workers threads
        class Worker: public Runnable{
        public:
            Worker(WorkerPool& pool, int id);
            void start();
            void run();

        private:
            WorkerPool& pool;
            Thread thread;
            bool running;
        };
        int worker_count;
        Mutex worker_mutex;
        std::list<Worker*> workers;
        Mutex workers_to_destroy_mutex;
        std::list<Worker*> workers_to_destroy;
    };

}

#include "worker-pool-detail.h"

#endif /* EPYX_WORKER_POOL_H */

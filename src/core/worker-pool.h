/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file worker-pool.h
 * @brief a worker pool definition.
 */
#ifndef EPYX_WORKER_POOL_H
#define EPYX_WORKER_POOL_H

#include <boost/noncopyable.hpp>
#include <list>
#include <string>
#include <sstream>
#include "common.h"

namespace Epyx
{
    /**
     * @class WorkerPool
     *
     * @brief A mean to distribute work between several threads easily
     *
     * A WorkerPool allows you to distribute a work on several threads by
     * passing "messages" that are in facts function arguments. To use it
     * subclass the WorkerPool and implement the treat() function that will
     * process the message in a thread.
     *
     * @tparam T the base type of the messages passed to the workers
     */
    template<typename T>class WorkerPool : private boost::noncopyable
    {
    public:

        /**
         * @brief The WorkerPool constructor
         * @param num_workers the initial number of worker threads
         * @param deleteMessages tell to delete messages afer a post if set
         * @param name the prefix of the name of the worker threads in the logs
         */
        WorkerPool(int num_workers, bool deleteMessages, const std::string& name);

        /**
         * @brief The WorkerPool other constructor
         * @param deleteMessages tell to delete messages afer a post if set
         */
        WorkerPool(bool deleteMessages);

        /**
         * @brief The WorkerPool destructor
         *
         * @todo What will happen with the messages that are still to be processed ?
         */
        virtual ~WorkerPool();

        /**
         * @brief Stop WorkerPool (wait all children threads)
         */
        void stop();

        /**
         * @brief Adds a message to be processed
         * @param message the message
         */
        void post(T *message);

        /**
         * @brief set the name of the workers
         * Please call this functionto set a name BEFORE starting any worker
         */
        void setName(const std::string& name);

        /**
         * @brief gets the number of workers
         * @return the current number of workers
         */
        int getNumWorkers() const;

        /**
         * @brief sets the number of workers
         * @param n the number of workers needed
         */
        void setNumWorkers(int n);

    protected:
        /**
         * @brief this is the method to override
         * @param message the message to be processed
         */
        virtual void treat(T *message) = 0;

    private:
        void addWorker();
        void removeWorker();
        void bookKeep();

        BlockingQueue<T> messages;
        bool deleteMessages;

        // Worker threads
        class Worker : public Thread
        {
        public:
            Worker(WorkerPool<T>* pool, int id);
            void run();
            void tellStop();

        private:
            WorkerPool* pool;
            Mutex running_mutex;
            bool running;
        };

        // Everything related to names
        std::string name;
        int worker_name_counter;

        int worker_count;
        Mutex workers_mutex;
        std::list<Worker*> workers;
        Mutex workers_to_destroy_mutex;
        std::list<Worker*> workers_to_destroy;
    };

}

#include "worker-pool-detail.h"

#endif /* EPYX_WORKER_POOL_H */

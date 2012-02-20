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

    template<typename T> WorkerPool<T>::WorkerPool(int num_workers, std::string name)
        :name(name), worker_name_counter(0){
        //Build the prefix for the worker threads names
        std::ostringstream concatenator;
        concatenator << name << "Worker";
        this->worker_name_prefix = concatenator.str();

        for(int i=0; i<num_workers; i++){
            this->addWorker();
        }
    }

    template<typename T> WorkerPool<T>::~WorkerPool(){
        this->workers_mutex.lock();
        std::list<Worker*> backup(this->workers);
        this->workers_mutex.unlock();

        //Prevent the automatic destruction of the workers
        this->workers_to_destroy_mutex.lock();
        for(int i=0; i<backup.size(); i++){
            this->removeWorker();
        }
        this->workers_to_destroy_mutex.unlock();

        for(typename std::list<Worker*>::iterator it = backup.begin(); it != backup.end(); ++it){
            Worker* w = *it;
            w->thread.wait();
            delete w;
        }
    }


    template<typename T> void WorkerPool<T>::post(T& message){
        this->bookKeep();
        this->message->push(&message);
    }

    template<typename T> int WorkerPool<T>::getNumWorkers(){
        return this->worker_count;
    }

    template<typename T> void WorkerPool<T>::setNumWorkers(int n){
        //TODO PLEASE
    }

    template<typename T> void WorkerPool<T>::addWorker(){
        Worker* w = new Worker(this, this->worker_name_prefix);

        this->workers.push(w);
        w->start();

        this->worker_name_prefix ++;
        this->worker_count ++;
    }

    template<typename T> void WorkerPool<T>::removeWorker(){
        this->bookKeep();
        this->message->push(NULL);
    }

    template<typename T> void WorkerPool<T>::bookKeep(){
        this->workers_to_destroy_mutex.lock();
        if(workers_to_destroy.empty()){
            this->workers_to_destroy_mutex.unlock();
        }else{
            Worker* w = workers_to_destroy.front();
            workers_to_destroy.pop_front();
            this->workers_to_destroy_mutex.unlock();

            //We can wait here as it won't happen often and will be fairly quick
            //and we probably can't destroy a thread while it's running
            w->thread.wait();
            delete w;
        }
    }



    template<typename T> WorkerPool<T>::Worker::Worker(WorkerPool& pool, int id)
        :pool(pool), thread(this, pool->worker_name_prefix, id){
    }

    template<typename T> void WorkerPool<T>::Worker::start(){
        this->running = true;
        this->thread.run();
    }

    template<typename T> void WorkerPool<T>::Worker::run(){
        while(this->running){
            T* msg = this->pool.messages.pop();

            if(msg == NULL){
                this->running = false;
            }else{
                this->pool.treat(*msg);
            }

            delete msg;
        }
        pool.workers_mutex.lock();
        pool.workers.remove(this);
        pool.workers_mutex.unlock();

        pool.workers_to_destroy_mutex.lock();
        pool.workers_to_destroy.push_front(this);
        pool.workers_to_destroy_mutex.unlock();
    }

}

#endif /* EPYX_WORKER_POOL_H */

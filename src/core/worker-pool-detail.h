//You should never include this file directly

#ifndef EPYX_WORKER_POOL_DETAIL_H
#define EPYX_WORKER_POOL_DETAIL_H

namespace Epyx {

    template<typename T> WorkerPool<T>::WorkerPool(int num_workers, std::string name)
        :name(name), worker_name_counter(0), worker_count(0){
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
        for(int i=0; i<(int)backup.size(); i++){
            this->removeWorker();
        }
        this->workers_to_destroy_mutex.unlock();

        for(typename std::list<Worker*>::iterator it = backup.begin(); it != backup.end(); ++it){
            Worker* w = *it;
            w->wait();
            delete w;
        }
    }


    template<typename T> void WorkerPool<T>::post(T& message){
        this->bookKeep();
        this->messages.push(&message);
    }

    template<typename T> int WorkerPool<T>::getNumWorkers(){
        return this->worker_count;
    }

    template<typename T> void WorkerPool<T>::setNumWorkers(int n){
        if(this->worker_count > n){
            int to_remove = this->worker_count - n;
            for(int i=0; i < to_remove; i++){
                this->removeWorker();
            }
        }else{
            int to_add = n - this->worker_count;
            for(int i=0; i < to_add; i++){
                this->addWorker();
            }
        }
    }

    template<typename T> void WorkerPool<T>::addWorker(){
        Worker* w = new Worker(this, this->worker_name_counter);

        this->workers.push_front(w);
        w->start();

        this->worker_name_counter ++;
        this->worker_count ++;
    }

    template<typename T> void WorkerPool<T>::removeWorker(){
        this->workers_mutex.lock();
        Worker* w = workers.front();
        workers.pop_front();
        this->workers_mutex.unlock();

        w->running_mutex.lock();
        w->running = false;
        w->running_mutex.unlock();
        this->worker_count --;
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
            w->wait();
            delete w;
        }
    }



    template<typename T> WorkerPool<T>::Worker::Worker(WorkerPool<T>* pool, int id)
        :pool(pool), Thread(pool->name, id), running(true){
    }

    template<typename T> void WorkerPool<T>::Worker::run(){
        this->running_mutex.lock();
        while(this->running){
            this->running_mutex.unlock();

            T* msg = this->pool->messages.pop();

            if(msg == NULL){
                this->running = false;
            }else{
                this->pool->treat(*msg);
            }

            delete msg;

            this->running_mutex.lock();
        }
        this->running_mutex.unlock();

        pool->workers_to_destroy_mutex.lock();
        pool->workers_to_destroy.push_front(this);
        pool->workers_to_destroy_mutex.unlock();
    }

}

#endif /* EPYX_WORKER_POOL_DETAIL_H */

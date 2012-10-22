/**
 * This program tests Thread and Mutex classes
 */

#include <stdlib.h>
#include "api.h"
#include "core/actor-manager.h"
#include "core/actor.h"

using namespace Epyx;

Mutex *my_mutex;
int my_counter;

class MutexThread : public Thread {
protected:
    virtual void run(){
        int i;

        usleep(100000 * (1 + rand()%20));
        my_mutex->lock();
        i = ++my_counter;
        my_mutex->unlock();
        log::info << "Inc Cnt to " << i << log::endl;

        usleep(100000 * (1 + rand()%20));
        my_mutex->lock();
        i = --my_counter;
        my_mutex->unlock();
        log::info << "Dec Cnt to " << i << log::endl;
    }
};

void test_mutex() {
    const int threadNumber = 10;
    Thread *threads[threadNumber];
    my_mutex = 0;

    try {
        log::info << "Create Mutex" << log::endl;
        my_mutex = new Mutex();
        my_counter = 0;

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            threads[i] = new MutexThread();
            //threads[i]->setName("Mutex", i);
            threads[i]->start();
        }

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }


    } catch (Exception e) {
        std::cerr << e;
    }

    if (my_mutex)
        delete my_mutex;
}

Condition *cond_condition;

class ConditionThread : public Thread {
protected:
    virtual void run(){
        cond_condition->lock();
        cond_condition->wait();
        cond_condition->unlock();
        log::info << "Woken up" << log::endl;

        cond_condition->lock();
        cond_condition->wait();
        cond_condition->unlock();
        log::info << "Woken up AGAIN" << log::endl;
    }
};

class ImpatientConditionThread : public Thread {
protected:
    virtual void run(){
        cond_condition->lock();
        cond_condition->timedWait(500);
        cond_condition->unlock();
        log::info << "I timed out" << log::endl;

        cond_condition->lock();
        cond_condition->timedWait(500);
        cond_condition->unlock();
        log::info << "I'm really too impatient" << log::endl;
    }
};

void test_cond(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        log::info << "Create Condition" << log::endl;
        cond_condition = new Condition();

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            if(i == 0) {
                threads[i] = new ImpatientConditionThread();
                //threads[i]->setName("Impatient", i);
            } else {
                threads[i] = new ConditionThread();
                //threads[i]->setName("Condition", i);
            }
            threads[i]->start();
        }

        sleep(2);

        log::info<<"Waking "<< threadNumber - 2 <<" threads"<< log::endl;
        for(int i=0; i < threadNumber - 2; i++){
            cond_condition->lock();
            cond_condition->notify();
            cond_condition->unlock();
        }
        sleep(1);
        log::info<<"Waking every thread"<< log::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();
        sleep(1);
        log::info<<"Waking the remaining threads"<< log::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();

        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }

    } catch (Exception e) {
        std::cerr << e;
    }
}

class SpammingThread : public Thread {
protected:
    virtual void run(){
        for(int i=0; i<10000; i++){
            log::info<<"Spamming!!!§ " << i << " times " << log::endl;
        }
    }
};

void stress_test_logger(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            threads[i] = new SpammingThread();
            //threads[i]->setName("Spamming", i);
        }
        for (int i = 0; i < threadNumber; i++) {
            threads[i]->start();
        }
        for (int i = 0; i < threadNumber; i++) {
            log::info << "Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }

    } catch (Exception e) {
        std::cerr << e;
    }
}

class WaitingWorkerPool: public WorkerPool<int> {
public:
    WaitingWorkerPool():WorkerPool<int>(true){}

    virtual void treat(int *t){
        sleep(*t);
        log::info << "Worker finished" << log::endl;
        // t is deleted by the caller
    }
};

void test_worker_pool(){
    WaitingWorkerPool p;
    p.setName("WaitingWorker");

    log::info << "First wave of sleeps: 6 posts for 6 workers" << log::endl;
    p.setNumWorkers(6);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(2);

    log::info << "Second wave of sleeps: 6 sleeps and ask the pool have only 3 workers" << log::endl;
    p.setNumWorkers(3);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(3);

    log::info << "Second wave of sleeps (continued) : 6 sleeps notice how the workers";
    log::info <<" are destroyed only after they received a message" << log::endl;
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(3);

    log::info << "Third wave of sleeps : 6 posts after asking the pool to have 6 workers" << log::endl;
    p.setNumWorkers(6);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(2);
}

class SimpleActor: public Actor{
public:
    virtual void treat(int n){
        log::info << "SimpleActor received something (and is very happy) : " << n << log::endl;
    }
    ~SimpleActor(){
        log::info << "Why would someone kill SimpleActor ?" << log::endl;
    }
};

class TimeoutActor: public Actor{
public:
    virtual void treat(int n){
        log::info << "Timeout actor did not time out" << log::endl;
    }
    virtual void timeout(){
        log::info << "Argh! I timed out, I'll just this.kill()" << log::endl;
        this->kill();
    }
};

class QueryActor: public Actor{
public:
    virtual void treat(EPYX_ACTOR_QUERY_ARG("foo") q){
        log::info << "I was queried with FOO" << log::endl;
    }
    virtual void treat(EPYX_ACTOR_QUERY_ARG("bar") q){
        log::info << "I was queried with BAR" << log::endl;
    }
};

void test_actors(){
    ActorManager manager(5, "Actors");

    {
        log::info<<"Making a simple actor and send it a message and destroy it immediately"<<log::endl;
        auto simple = manager.add(new SimpleActor);
        simple.post(18);
        simple.kill();
    }
    sleep(1);
    log::info<<log::endl;
    {
        log::info<<"Making a simple actor and send it a message and destroy it after some time"<<log::endl;
        auto simple = manager.add(new SimpleActor);
        simple.post(18);
        usleep(10000);
        simple.kill();
    }
    sleep(1);
    log::info<<log::endl;
    {
        log::info<<"making a timeout actor and send it a message then wait it's timeout before sending it another message"<<log::endl;
        auto timeout = manager.add(new TimeoutActor, 1);
        timeout.post(18);
        usleep(10000);
        timeout.post(18);
    }
    sleep(1);
    log::info<<log::endl;
    {
        log::info<<"Testing the use of named structs within treat arguments"<<log::endl;
        auto query = manager.add(new QueryActor);
        query.post(EPYX_ACTOR_QUERY("foo"));
        usleep(10000);
        query.post(EPYX_ACTOR_QUERY("bar"));
        usleep(10000);
        query.kill();
    }
    log::info<<log::endl;
    sleep(1);
}

int main(){
    Epyx::API epyx(log::CONSOLE | log::LOGFILE, "Test.log");
    try {
        //test_mutex();
        //test_cond();
        //stress_test_logger();
        //test_worker_pool();
        test_actors();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}

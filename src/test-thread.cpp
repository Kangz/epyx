/**
 * This program tests Thread and Mutex classes
 */

#include <iostream>
#include <stdlib.h>
#include "core/thread.h"
#include "core/mutex.h"
#include "core/condition.h"


Epyx::Mutex *my_mutex;
int my_counter;

void mutex_thread(void *pThreadId) {
    int i;
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    usleep(100000 * (1 + rand()%20));
    my_mutex->lock();
    i = ++my_counter;
    my_mutex->unlock();
    std::cout << "[Thread " << threadId << "] Inc Cnt to " << i << '\n';

    usleep(100000 * (1 + rand()%20));
    my_mutex->lock();
    i = --my_counter;
    my_mutex->unlock();
    std::cout << "[Thread " << threadId << "] Dec Cnt to " << i << '\n';

    delete (int*)pThreadId;
    pThreadId = 0;
}

void test_mutex() {
    const int threadNumber = 10;
    Epyx::Thread *threads[threadNumber];
    my_mutex = 0;

    try {
        std::cout << "[Main] Create Mutex \n";
        my_mutex = new Epyx::Mutex();
        my_counter = 0;

        for (int i = 0; i < threadNumber; i++) {
            std::cout << "[Main] Spawn thread " << i << '\n';
            threads[i] = new Epyx::Thread(mutex_thread, new int(i));
            threads[i]->run();
        }

        for (int i = 0; i < threadNumber; i++) {
            std::cout << "[Main] Wait for thread " << i << '\n';
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }


    } catch (Epyx::Exception e) {
        std::cerr << e;
    }

    if (my_mutex)
        delete my_mutex;
}

Epyx::Condition *cond_condition;

void cond_thread(void *pThreadId) {
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    cond_condition->lock();
    cond_condition->wait();
    cond_condition->unlock();
    std::cout << "[Thread " << threadId << "] woken up" << '\n';


    cond_condition->lock();
    cond_condition->wait();
    cond_condition->unlock();
    std::cout << "[Thread " << threadId << "] woken up AGAIN" << '\n';

    delete (int*)pThreadId;
}

void cond_impatient_thread(void *pThreadId) {
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    cond_condition->lock();
    cond_condition->timedWait(500);
    cond_condition->unlock();
    std::cout << "[Thread " << threadId << "] I timed out" << '\n';

    cond_condition->lock();
    cond_condition->timedWait(500);
    cond_condition->unlock();
    std::cout << "[Thread " << threadId << "] I'm really too impatient" << '\n';

    delete (int*)pThreadId;
}

void test_cond(){
    const int threadNumber = 6;
    Epyx::Thread *threads[threadNumber];

    try {
        std::cout << "[Main] Create Condition \n";
        cond_condition = new Epyx::Condition();

        for (int i = 0; i < threadNumber; i++) {
            std::cout << "[Main] Spawn thread " << i << '\n';
            if(i == 0) {
                threads[i] = new Epyx::Thread(cond_impatient_thread, new int(i));
            } else {
                threads[i] = new Epyx::Thread(cond_thread, new int(i));
            }
            threads[i]->run();
        }

        sleep(2);

        std::cout<<"Waking "<< threadNumber - 2 <<" threads"<<std::endl;
        for(int i=0; i < threadNumber - 2; i++){
            cond_condition->lock();
            cond_condition->notify();
            cond_condition->unlock();
        }
        sleep(1);
        std::cout<<"Waking every thread"<<std::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();
        sleep(1);
        std::cout<<"Waking the remaining threads"<<std::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();

        for (int i = 0; i < threadNumber; i++) {
            std::cout << "[Main] Wait for thread " << i << '\n';
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }

    } catch (Epyx::Exception e) {
        std::cerr << e;
    }
}

int main(){
    test_mutex();
    test_cond();
    return 0;
}

/**
 * This program tests Thread and Mutex classes
 */

#include <iostream>
#include <stdlib.h>
#include "core/thread.h"
#include "core/mutex.h"
#include "core/condition.h"
#include "core/log.h"


using namespace Epyx;

Mutex *my_mutex;
int my_counter;

void mutex_thread(void *pThreadId) {
    int i;
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    usleep(100000 * (1 + rand()%20));
    my_mutex->lock();
    i = ++my_counter;
    my_mutex->unlock();
    log::info << "[Thread " << threadId << "] Inc Cnt to " << i << log::endl;

    usleep(100000 * (1 + rand()%20));
    my_mutex->lock();
    i = --my_counter;
    my_mutex->unlock();
    log::info << "[Thread " << threadId << "] Dec Cnt to " << i << log::endl;

    delete (int*)pThreadId;
    pThreadId = 0;
}

void test_mutex() {
    const int threadNumber = 10;
    Thread *threads[threadNumber];
    my_mutex = 0;

    try {
        log::info << "[Main] Create Mutex" << log::endl;
        my_mutex = new Mutex();
        my_counter = 0;

        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Spawn thread " << i << log::endl;
            threads[i] = new Thread(mutex_thread, new int(i));
            threads[i]->run();
        }

        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Wait for thread " << i << log::endl;
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

void cond_thread(void *pThreadId) {
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    cond_condition->lock();
    cond_condition->wait();
    cond_condition->unlock();
    log::info << "[Thread " << threadId << "] woken up" << log::endl;


    cond_condition->lock();
    cond_condition->wait();
    cond_condition->unlock();
    log::info << "[Thread " << threadId << "] woken up AGAIN" << log::endl;

    delete (int*)pThreadId;
}

void cond_impatient_thread(void *pThreadId) {
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    cond_condition->lock();
    cond_condition->timedWait(500);
    cond_condition->unlock();
    log::info << "[Thread " << threadId << "] I timed out" << log::endl;

    cond_condition->lock();
    cond_condition->timedWait(500);
    cond_condition->unlock();
    log::info << "[Thread " << threadId << "] I'm really too impatient" << log::endl;

    delete (int*)pThreadId;
}

void test_cond(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        log::info << "[Main] Create Condition" << log::endl;
        cond_condition = new Condition();

        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Spawn thread " << i << log::endl;
            if(i == 0) {
                threads[i] = new Thread(cond_impatient_thread, new int(i));
            } else {
                threads[i] = new Thread(cond_thread, new int(i));
            }
            threads[i]->run();
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

void spamming_thread(void *pThreadId) {
    int threadId = -1;

    if (pThreadId) threadId = *(int*)pThreadId;

    for(int i=0; i<100; i++){
        log::info<<"Thread " << threadId << " spamming!!!§ " << i << " times " << log::endl;
    }

    delete (int*)pThreadId;
}


void stress_test_logger(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Spawn thread " << i << log::endl;
            threads[i] = new Thread(spamming_thread, new int(i));
        }
        for (int i = 0; i < threadNumber; i++) {
            threads[i]->run();
        }
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

int main(){
    log::init(log::CONSOLE);
    std::cout << log::_buffers << std::endl;

    test_mutex();
    test_cond();
    stress_test_logger();
    return 0;
}

/**
 * This program tests Thread and Mutex classes
 */

#include <iostream>
#include <stdlib.h>
#include "core/thread.h"
#include "core/mutex.h"

Epyx::Mutex *my_mutex;
int my_counter;

void my_thread(void *pThreadId)
{
    int i;
    int threadId = -1;

    if (pThreadId)
        threadId = *(int*)pThreadId;

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

int main()
{
    const int threadNumber = 10;
    Epyx::Thread *threads[threadNumber];
    my_mutex = 0;

    try {
        std::cout << "[Main] Create Mutex \n";
        my_mutex = new Epyx::Mutex();
        my_counter = 0;

        for (int i = 0; i < threadNumber; i++) {
            std::cout << "[Main] Spawn thread " << i << '\n';
            threads[i] = new Epyx::Thread(my_thread, new int(i));
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
    return 0;
}

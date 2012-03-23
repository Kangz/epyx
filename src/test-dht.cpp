/**
 * This program tests Thread and Mutex classes
 */

#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <stdint.h>
#include "core/common.h"
#include "dht/id.h"
//#include "dht/kbucket.h"

using namespace Epyx;

using namespace Epyx::DHT;

void random_id(Id& id){
    uint8_t* dist = (uint8_t*) &id.data;
    for (int i = 0; i < ID_STORAGE_SIZE; i++) {
        *dist = rand()%256;
        dist ++;
    }
}

void test_id_distance(){
    srand ( time(NULL) );

    Id a, b;
    random_id(a);
    random_id(b);
    log::debug<<"     a: "<<a<<log::endl;
    log::debug<<"     b: "<<b<<log::endl;
    Distance d(&a, &b);
    log::debug<<"d(a,b): "<<d<<log::endl;


    log::debug<<"Modifying random bits"<<log::endl;

    for(int i=0; i<6; i++) {
        Id a;
        random_id(a);
        Id b = a;
        int bit = rand()%ID_LENGTH;
        b.data[bit/8] ^= 1 << (7 - (bit%8));
        log::debug<<"     a: "<<a<<log::endl;
        log::debug<<"     b: "<<b<<log::endl;
        Distance d(&a, &b);
        log::debug<<"d(a,b): "<<d<<log::endl;
    }
}

int main(){
    Thread::init();
    log::init(log::CONSOLE | log::LOGFILE, "Test.log");

    test_id_distance();

    log::flushAndQuit();
    return 0;
}

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
#include "dht/kbucket.h"

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
    Id a, b;
    random_id(a);
    random_id(b);
    log::info<<"     a: "<<a<<log::endl;
    log::info<<"     b: "<<b<<log::endl;
    Distance d(&a, &b);
    log::info<<"d(a,b): "<<d<<log::endl;


    log::info<<"Modifying random bits"<<log::endl;

    for(int i=0; i<6; i++) {
        Id a;
        random_id(a);
        Id b = a;
        int bit = rand()%ID_LENGTH;
        b.data[bit/8] ^= 1 << (7 - (bit%8));
        log::info<<"     a: "<<a<<log::endl;
        log::info<<"     b: "<<b<<log::endl;
        Distance d(&a, &b);
        log::info<<"d(a,b): "<<d<<log::endl;
    }
}

void test_kbucket(){
    Id self;
    random_id(self);

    log::info<<"I am at Id: "<<self<<log::endl;

    KBucket kb(&self);

    log::info<<"Inserting 500.000 nodes in the routing table"<<log::endl;

    for(int i=500000; i-->0;){
        Id a;
        random_id(a);

        kb.seenPeer(&a);
    }

    log::info<<"Making 10.000 lookups in the routing table"<<log::endl;

    for(int i=10000; i-->0;){
        Id a;
        random_id(a);

        std::multimap<Distance,Id> nearest;

        kb.findNearestNodes(&a, nearest, 20);
    }

    log::info<<"Done making 10.000 lookups in the routing table"<<log::endl;

    Id a;
    random_id(a);
    std::multimap<Distance,Id> nearest;
    kb.findNearestNodes(&a, nearest, 20);

    std::multimap<Distance, Id>::iterator it = nearest.begin();

    log::info<<"Searching for a: "<<a<<log::endl;
    for(int i=0; i<20; it++, i++){
        Distance d = (*it).first;
        Id id = (*it).second;
        log::info<<"      id: "<<id<<log::endl;
        log::info<<"d(id, a): "<<d<<log::endl;
    }
}

int main(){
    Thread::init();
    log::init(log::CONSOLE | log::LOGFILE, "Test.log");
    srand ( time(NULL) );

    //test_id_distance();
    test_kbucket();

    log::flushAndQuit();
    return 0;
}

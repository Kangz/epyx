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
#include "dht/dht-packet.h"
#include "n2np/n2np-nodeid.h"

using namespace Epyx;

using namespace Epyx::DHT;

void random_id(Id& id){
    uint8_t* dist = (uint8_t*) &id.data;
    for (int i = 0; i < ID_STORAGE_SIZE; i++) {
        *dist = rand()%256;
        dist ++;
    }
}

void random_id(std::string& s){
    std::stringstream ss;
    Id id;
    random_id(id);
    ss << id;
    s = ss.str();
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

    log::info << "Converting to string and back:" << log::endl;
    log::info << a << log::endl;

    std::stringstream o;
    o<<  a; //<-PAN
    Id c;
    o>>c;

    log::info << c << log::endl;

}

void test_kbucket(){
    Id self;
    random_id(self);
    N2npNodeId n2npId;

    log::info<<"I am at Id: "<<self<<log::endl;

    KBucket kb(&self);

    log::info<<"Inserting 500.000 nodes in the routing table"<<log::endl;

    for(int i=500000; i-->0;){
        Id a;
        random_id(a);

        kb.seenPeer(&a, n2npId);
    }

    log::info<<"Making 10.000 lookups in the routing table"<<log::endl;

    for(int i=10000; i-->0;){
        Id a;
        random_id(a);

        std::vector<Peer> nearest;

        kb.findNearestNodes(a, nearest, 20);
    }

    log::info<<"Done making 10.000 lookups in the routing table"<<log::endl;

    Id a;
    random_id(a);
    std::vector<Peer> nearest;
    kb.findNearestNodes(a, nearest, 20);

    std::vector<Peer>::iterator it = nearest.begin();

    log::info<<"Searching for a: "<<a<<log::endl;
    for(int i=0; i<20; it++, i++){
        Id id = (*it).id;
        Distance d(&a, &id);
        log::info<<"      id: "<<id<<log::endl;
        log::info<<"d(id, a): "<<d<<log::endl;
    }
}

void double_print(DHTPacket& pkt){
    GTTPacket* pkt1 = pkt.toGTTPacket();
    DHTPacket pkt2(*pkt1);
    GTTPacket* pkt3 = pkt2.toGTTPacket();
    log::info << "dht -> gtt\n" << *pkt1 << log::endl;
    log::info << "dht -> gtt -> dht -> gtt\n" << *pkt3 << log::endl;
}

void test_dhtpacket(){
    DHTPacket pkt;
    random_id(pkt.from);

    log::info<<"Testing PING"<<log::endl;
    pkt.method = M_PING;
    double_print(pkt);

    log::info<<"Testing PONG"<<log::endl;
    pkt.method = M_PONG;
    double_print(pkt);

    log::info<<"Testing STORE"<<log::endl;
    pkt.method = M_STORE;
    pkt.connectionId = 42;
    random_id(pkt.key);
    pkt.value = std::string(34, 'c');
    double_print(pkt);

    log::info<<"Testing STORED"<<log::endl;
    pkt.method = M_STORED;
    pkt.connectionId = 42;
    pkt.status = 0;
    double_print(pkt);

    log::info<<"Testing GET"<<log::endl;
    pkt.method = M_GET;
    pkt.connectionId = 42;
    random_id(pkt.key);
    double_print(pkt);

    log::info<<"Testing GOT"<<log::endl;
    pkt.method = M_GOT;
    pkt.value = std::string(34, 'c');
    log::info << pkt.value.size() << log::endl;
    pkt.connectionId = 42;
    double_print(pkt);

    log::info<<"Testing FIND"<<log::endl;
    pkt.method = M_FIND;
    pkt.connectionId = 42;
    pkt.count = 20;
    random_id(pkt.idToFind);
    double_print(pkt);

    log::info<<"Testing FOUND"<<log::endl;
    pkt.method = M_FOUND;
    pkt.connectionId = 42;
    pkt.status = 0;
    for(int i=0; i<20; i++){
        Peer temp;
        random_id(temp.id);
        pkt.foundPeers.push_back(temp);
    }
    random_id(pkt.idToFind);
    double_print(pkt);


}

int main(){
    Thread::init();
    log::init(log::CONSOLE | log::LOGFILE, "Test.log");
    srand ( time(NULL) );

    //test_id_distance();
    //test_kbucket();
    test_dhtpacket();

    log::flushAndQuit();
    return 0;
}

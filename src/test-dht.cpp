/**
 * A program to test the DHT
 */

#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <stdint.h>
#include "api.h"
#include "dht/node.h"
#include "dht/id.h"
#include "dht/kbucket.h"
#include "dht/packet.h"
#include "n2np/nodeid.h"
#include "n2np/relay.h"
#include "n2np/node.h"

using namespace Epyx;

using namespace Epyx::DHT;

void random_id(Id& id){
    uint8_t* dist = (uint8_t*) &id.data;
    for (int i = 0; i < Id::STORAGE_SIZE; i++) {
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
        int bit = rand()%Id::LENGTH;
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
    N2NP::NodeId n2npId;

    log::info<<"I am at Id: "<<self<<log::endl;

    KBucket kb(self);

    log::info<<"Inserting 500.000 nodes in the routing table"<<log::endl;

    for(int i=500000; i-->0;){
        Id a;
        random_id(a);

        kb.seenPeer(a, n2npId);
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

void double_print(Packet& pkt){
    GTTPacket* pkt1 = pkt.toGTTPacket();
    Packet pkt2(*pkt1);
    GTTPacket* pkt3 = pkt2.toGTTPacket();
    log::info << "dht -> gtt\n" << *pkt1 << log::endl;
    if(pkt.method == M_FOUND) log::info << pkt.value << log::endl;
    log::info << "dht -> gtt -> dht -> gtt\n" << *pkt3 << log::endl;
    if(pkt.method == M_FOUND) log::info << pkt2.value << log::endl;
    log::waitFlush();
}

void test_dhtpacket(){
    Packet pkt;
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
    pkt.key = "someKeyIWant/ToFind";
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
    pkt.key = "someKeyIWant/ToFind";
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
    pkt.foundPeers = new std::vector<Peer>();
    for(int i=0; i<20; i++){
        Peer temp;
        random_id(temp.id);
        pkt.foundPeers->push_back(temp);
    }
    random_id(pkt.idToFind);
    double_print(pkt);
}


class FakeDht : public Epyx::N2NP::Module
{
public:
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size){
        log::info<<"The fakeDHT received a message:"<<log::endl;
        std::string toPrint(data, size);
        log::info<<toPrint<<log::endl;
    }
};


void test_dht_n2np(){
     // Create Net Select for relay
    NetSelect *selectRelay = new NetSelect(10, "wRelay");
    selectRelay->setName("NetSelectRelay");
    selectRelay->start();

    // Create relay
    Address addr("127.0.0.1:4242");
    N2NP::Relay *relay = new N2NP::Relay(addr);
    selectRelay->add(new N2NP::RelayServer(new TCPServer(addr, 50), relay));
    log::info << "Start Relay " << relay->getId() << log::endl;

    // Create Net Select for nodes
    Epyx::NetSelect *selectNodes = new Epyx::NetSelect(10, "WNodes");
    selectNodes->setName("NetSelectNodes");
    selectNodes->start();

    // Create nodes
    Epyx::log::info << "Create nodes..." << Epyx::log::endl;
    Epyx::N2NP::Node node0(addr), node1(addr);
    selectNodes->add(&node0);
    selectNodes->add(&node1);

    // Wait for node IDs
    Epyx::log::info << "Waiting for nodes..." << Epyx::log::endl;
    Epyx::N2NP::NodeId nodeids[2];
    while (!node0.isReady() || !node1.isReady()){
        usleep(100);
    }

    DHT::Id id;
    random_id(id);
    DHT::Node dht(id, node0, "DHT");

    node0.addModule("DHT", &dht);

    FakeDht fakeDHT;
    node1.addModule("DHT", &fakeDHT);

    Id fakeId;
    random_id(fakeId);

    #define SendToDHT(method, message) \
    { \
        std::ostringstream o; \
        std::string msg = (message); \
        o << "DHT " << (method) << "\r\n"; \
        o << "from: " << fakeId << "\r\n"; \
        if(msg.length() != 0) o << (msg); \
        else o << "\r\n"; \
        std::string s = o.str(); \
        dht.fromN2NP(node0, node1.getId(), s.c_str(), s.length()); \
    }

    log::info << "Sending a PING to the DHT" << log::endl;
    SendToDHT("PING", "");

    SendToDHT("GET",
            "connectionid: 42\r\n"
            "key: someKeyIWant/ToFind\r\n"
            "\r\n");

    sleep(1);

    SendToDHT("STORE",
            "connectionid: 43\r\n"
            "key: someKeyIWant/ToFind\r\n"
            "content-length: 5\r\n"
            "\r\n"
            "12345");

    sleep(1);

    SendToDHT("GET",
            "connectionid: 44\r\n"
            "key: someKeyIWant/ToFind\r\n"
            "\r\n");

    sleep(1);

    SendToDHT("FIND",
            "connectionid: 45\r\n"
            "count: 20\r\n"
            "target: db3b:c14f:0000:0000:0000:0000:0000:0000:9063:a3cc:ff7f:0000:61ef:3d58:017f:0000\r\n"
            "\r\n")

    #undef SendToDHT

    sleep(1); //Wait for the processing of the messages
}

static const int NETWORK_SIZE = 100;

class MyFindCallback: public FindCallback {
    public:
        void onFound(std::vector<std::pair<Distance, Peer> >& result){
            log::info << "The DHT found the closest Ids with distance : " << log::endl;
            std::vector<std::pair<Distance, Peer> >::iterator it;
            for(it = result.begin(); it != result.end(); it ++){
                log::info << (*it).first << log::endl;
            }
        }
};

class MyGetCallback: public GetCallback {
    public:
        void onGot(const std::string& result) {
            log::info<<"I found a value !!! It is : " << result << log::endl;
        }
        void onError() {
            log::info << "I did not find the value :(" << log::endl;
        }
};

void test_dht_network(){
    // Create Net Select for relay
    NetSelect *selectRelay = new NetSelect(10, "wRelay");
    selectRelay->setName("NetSelectRelay");
    selectRelay->start();

    // Create relay
    Address addr("127.0.0.1:4242");
    N2NP::Relay *relay = new N2NP::Relay(addr);
    selectRelay->add(new N2NP::RelayServer(new TCPServer(addr, 50), relay));
    log::info << "Start Relay " << relay->getId() << log::endl;

    // Create Net Select for nodes
    Epyx::NetSelect *selectNodes = new Epyx::NetSelect(10, "WNodes");
    selectNodes->setName("NetSelectNodes");
    selectNodes->start();

    // Create nodes
    Epyx::log::info << "Create nodes..." << Epyx::log::endl;
    Epyx::N2NP::Node* n2npNodes[NETWORK_SIZE];
    for(int i=0; i<NETWORK_SIZE; i++){
        n2npNodes[i] = new Epyx::N2NP::Node(addr);
        selectNodes->add(n2npNodes[i]);
    }

    // Wait for node IDs
    Epyx::log::info << "Waiting for nodes..." << Epyx::log::endl;
    Epyx::N2NP::NodeId n2npIds[NETWORK_SIZE];
    for(int i=0; i<NETWORK_SIZE; i++) {
        while (!n2npNodes[i]->isReady()){
            usleep(100);
        }
    }

    //Create DHTs
    log::info << "Creating DHTs" << log::endl;
    DHT::Node* dhtNodes[NETWORK_SIZE];
    for(int i=0; i<NETWORK_SIZE; i++) {
        DHT::Id id;
        random_id(id);
        std::ostringstream o;
        o << "DHT";
        o << i;
        dhtNodes[i] = new DHT::Node(id, *n2npNodes[i], o.str());
        n2npNodes[i]->addModule("DHT", dhtNodes[i]);
    }

    //Make pings
    log::info << "Building the network" << log::endl;
    for(int i=0; i<NETWORK_SIZE; i++) {
        for(int j=i+1; j<NETWORK_SIZE; j++) {
            Peer p = dhtNodes[j]->getConnectionInfo();
            dhtNodes[i]->sendPing(p);
        }
    }

    log::info << "Done sending PING commands, waiting for the messages to be processed" << log::endl;

    sleep(10); //Wait for the processing of the messages

    log::info << "Launching the FIND query" << log::endl;

    Id idToFind;
    random_id(idToFind);

    dhtNodes[0]->findClosest(new MyFindCallback(), 5, idToFind);

    sleep(10);

    log::info << "Launching the GET query" << log::endl;

    dhtNodes[0]->getValue(new MyGetCallback(), "value1");

    sleep(10);
}


int main(){
    Epyx::API epyx(log::CONSOLE | log::LOGFILE, "Test.log");
    srand ( time(NULL) );
    try {
        //test_id_distance();
        //test_kbucket();
        //test_dhtpacket();
        //test_dht_n2np();
        test_dht_network();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}

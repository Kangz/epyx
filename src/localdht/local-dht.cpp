#include "local-dht.h"

#include <iostream>
namespace Epyx {

    LocalDHT::LocalDHT(LocalNode& n): node(n){
        node.registerRecv(N2npPacketType("DHT"), LocalDHT::receive_packet, this);
    }
    
    
    void LocalDHT::receive(LocalNode& node, const N2npPacket& pkt){
        std::cout << "[DHT associated to " << node << "] Recv from " << pkt.from << ": `"
            << pkt.data << "'\n";
    }
    
    bool LocalDHT::receive_packet(LocalNode& node, const N2npPacket& pkt, void* arg_){
        LocalDHT* dht = (LocalDHT*) arg_;
        dht->receive(node, pkt);
        return true;
    };

}


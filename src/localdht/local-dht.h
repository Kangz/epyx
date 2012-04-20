/**
 * Local Relay management
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_DHT_H
#define EPYX_LOCAL_DHT_H

#include "../n2np/n2np-packet.h"
#include "../localn2np/local-node.h"
#include "../core/thread.h"

namespace Epyx{

    class LocalDHT{

    public:
        LocalDHT(LocalNode& n);

    private:
        LocalNode& node;

        void receive(LocalNode& node, const N2npPacket& pkt);
        static bool receive_packet(LocalNode& node, const N2npPacket& pkt, void* arg_);
        
    };
}

#endif /* EPYX_LOCAL_DHT_H */

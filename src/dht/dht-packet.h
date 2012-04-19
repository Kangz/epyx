#ifndef EPYX_DHT_DHTPACKET_H
#define EPYX_DHT_DHTPACKET_H

#include <string>
#include <vector>

#include "id.h"
#include "peer.h"
#include "../parser/gttpacket.h"

namespace Epyx
{
namespace DHT
{
    enum DHTMethod
    {
        M_PING,
        M_PONG,

        M_GET,
        M_GOT,

        M_FIND,
        M_FOUND,

        M_STORE,
        M_STORED
    };

    class DHTPacket
    {
    public:
        void getValueFromGTT(GTTPacket& pkt);
        void setValueForGTT(GTTPacket* pkt);
        DHTPacket(GTTPacket& pkt);
        DHTPacket();
        ~DHTPacket();
        GTTPacket* toGTTPacket();

        DHTMethod method;
        Id from;
        int connectionId;
        int status;
        int count;
        std::vector<Peer> foundPeers;

        std::string value;
        std::string key;
        Id idToFind;
    };

}
}
#endif /*EPYX_DHT_DHTPACKET_H*/

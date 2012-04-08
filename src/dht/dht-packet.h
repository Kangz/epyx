#ifndef EPYX_DHT_DHTPACKET_H
#define EPYX_DHT_DHTPACKET_H

#include <string>
#include <list>

#include "id.h"
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
        DHTPacket(GTTPacket& pkt);
        DHTPacket();
        ~DHTPacket();
        GTTPacket* toGTTPacket();

        DHTMethod method;
        Id from;
        int connectionId;
        int status;
        int count;
        std::list<Id> foundIds;

        int valueSize;
        char* value;
        Id key;
        Id idToFind;
    };

}
}
#endif /*EPYX_DHT_DHTPACKET_H*/

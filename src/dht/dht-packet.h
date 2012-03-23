#ifndef EPYX_DHT_DHTPACKET_H
#define EPYX_DHT_DHTPACKET_H

#include <string>
#include <vector>

#include "id.h"
#include "../parser/gttpacket.h"

namespace Epyx
{
namespace DHT
{

    class DHTPacket
    {
    public:
        DHTPacket(const GTTPacket& pkt);
        ~DHTPacket();

        std::string method;
        int idConnection;
        int status;
        int n;
        vector<Id> idFound;
        string value;
        string key;
        Id idToFind;
    };

}
}
#endif /*EPYX_DHT_DHTPACKET_H*/

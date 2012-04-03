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
    int str2int(const std::string& str);
    std::string int2str(const int& n);

    class DHTPacket
    {
    public:
        DHTPacket(GTTPacket& pkt);
        ~DHTPacket();
        GTTPacket toGTTPacket();

        std::string method;
        int idConnection;
        int status;
        int n;
        std::vector<Id> idFound;
        std::string value;
        std::string key;
        Id idToFind;
    };

}
}
#endif /*EPYX_DHT_DHTPACKET_H*/

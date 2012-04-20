/**
 * @file packet.h
 * @brief the definition of the DHT Packet
 */
#ifndef EPYX_DHT_PACKET_H
#define EPYX_DHT_PACKET_H

#include <string>
#include <vector>

#include "id.h"
#include "peer.h"
#include "../parser/gttpacket.h"

namespace Epyx
{
namespace DHT
{
    /**
     * @enum DHTMethod
     * @brief the different methods a DHT Packet can have
     */
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

    /**
     * @class Packet
     * @brief represents a DHT packet, it can parse a GTTPacket or output one
     */
    class Packet
    {
    public:
        /**
         * @brief a Packet constructor, parses the given GTTPacket
         * @param pkt the GTTPacket to build the Packet from
         */
        Packet(GTTPacket& pkt);

        /**
         * @brief the default Packet constructor
         */
        Packet();

        /**
         * @brief a Packet destructor
         */
        ~Packet();

        /**
         * @brief outputs the GTTPacket corresponding to this Packet
         * @result the GTTPacket
         */
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

    private:
        void getValueFromGTT(GTTPacket& pkt);
        void setValueForGTT(GTTPacket* pkt);
    };

}
}
#endif /*EPYX_DHT_PACKET_H*/

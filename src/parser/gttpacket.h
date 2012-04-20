/**
 * @file gttpacket.h
 * @brief Packet of the Generic Text Template
 */

#ifndef EPYX_GTTPACKET_H
#define EPYX_GTTPACKET_H

#include <map>
#include <string>
#include <ostream>
#include "../net/socket.h"

namespace Epyx
{
    /**
     * @class GTTPacket
     *
     * @brief Packet for Generic Text Template
     */
    class GTTPacket
    {
    public:
        /**
         * @brief Constructor
         */
        GTTPacket();

        ~GTTPacket();

        /**
         * @brief Print a short packet desciption in an output stream (for debug)
         * @param os output stream
         * @param pkt
         */
        friend std::ostream& operator<<(std::ostream& os, const GTTPacket& pkt);

        /**
         * @brief Build the raw text query for this GTT Packet
         * @param newData newly-allocated buffer with ready-to-send data
         * @return size of *data
         *
         * Note: Please delete *newData with delete[]
         */
        unsigned long build(char **newData) const;

        /**
         * @brief Protocol name
         */
        std::string protocol;
        /**
         * @brief Method name
         */
        std::string method;
        /**
         * @brief Headers map
         */
        std::map<std::string, std::string> headers;
        /**
         * @brief Content size
         */
        unsigned long size;
        /**
         * @brief Content body
         */
        const char *body;
    };
}

#endif /* EPYX_GTTPACKET_H */

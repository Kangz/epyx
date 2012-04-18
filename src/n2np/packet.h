/**
 * @file packet.h
 * @brief N2NP Packet
 */

#ifndef EPYX_N2NP_PACKET_H
#define EPYX_N2NP_PACKET_H

#include "nodeid.h"
#include "packettype.h"

namespace Epyx
{
    namespace N2NP
    {
        /**
         * @class Packet
         * @brief Implement N2NP packet
         */
        class Packet
        {
        public:
            /**
             * @brief Constructor
             * @param type
             * @param size
             * @param data
             */
            Packet(const PacketType& type, unsigned int size,
                    const char *data);

            /**
             * @brief Packet deep copy
             * @param pkt
             */
            Packet(const Packet& pkt);
            /**
             * @brief Assignment with a deep copy
             * @param pkt
             * @return *this
             */
            const Packet& operator=(const Packet& pkt);

            /**
             * @brief Source node ID
             */
            NodeId from;
            /**
             * @brief Destination node ID
             */
            NodeId to;

            // TODO: Version

            /**
             * @brief Content type
             */
            PacketType type;
            /**
             * @brief Content size
             */
            unsigned int size;
            /**
             * @brief Content
             */
            const char *data;
        };
    }
}

#endif /* EPYX_N2NP_PACKET_H */

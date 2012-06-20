/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file n2np/packet.h
 * @brief N2NP Packet
 */

#ifndef EPYX_N2NP_PACKET_H
#define EPYX_N2NP_PACKET_H

#include "nodeid.h"
#include "../net/socket.h"
#include "../parser/gttpacket.h"

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
             * @param method
             * @param size
             * @param data
             */
            Packet(const std::string method, unsigned int size,
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
             * @brief Build a N2NP packet from a GTT Packet
             * @param pkt GTTPacket
             *
             * Note: data is copied, so you may destroy the GTT packet after
             * this constructor
             */
            Packet(const GTTPacket& pkt);

            ~Packet();

            /**
             * @brief Print a short packet desciption in an output stream
             * @param os output stream
             * @param pkt
             */
            friend std::ostream& operator<<(std::ostream& os, const Packet& pkt);

            /**
             * @brief Build the raw text query for this N2NP Packet
             * @param newData newly-allocated buffer with ready-to-send data
             * @return size of *data
             *
             * Note: Please delete *newData with delete[]
             */
            unsigned long build(char **newData) const;

            /**
             * @brief Send packet with a Socket
             * @param sock
             * @return true if it succeeded
             */
            bool send(Socket& sock) const;

            /**
             * @brief GTT Method
             */
            std::string method;

            /**
             * @brief Source node ID
             */
            NodeId from;
            /**
             * @brief Destination node ID
             */
            NodeId to;

            /**
             * @brief Protocol version
             * @todo a proper class
             */
            std::string version;

            /**
             * @brief Packet ID
             */
            unsigned long pktID;

            /**
             * @brief Content size
             */
            unsigned int size;
            /**
             * @brief Content
             */
            const char *data;

        private:
            /**
             * @brief Private function to get a GTT packet from a N2NP packet
             * @param gttpkt (out) GTT packet
             *
             * Use this function with care: as this->data = gttpkt.body, you need
             * to reset gttpkt.body to NULL before destroying it
             */
            void fillGTTPacket(GTTPacket &gttpkt) const;
        };
    }
}

#endif /* EPYX_N2NP_PACKET_H */

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
             * @param data
             */
            Packet(const std::string& method, const byte_str& data);

            /**
             * @brief Build a N2NP packet from a GTT Packet
             * @param pkt GTTPacket
             *
             * Note: data is copied, so you may destroy the GTT packet after
             * this constructor
             */
            Packet(const GTTPacket& pkt);

            /**
             * @brief Print a short packet desciption in an output stream
             * @param os output stream
             * @param pkt
             */
            friend std::ostream& operator<<(std::ostream& os, const Packet& pkt);

            /**
             * @brief Build the raw text query for this N2NP Packet
             * @return bytes string
             */
            byte_str build() const;

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
             * @brief Content
             */
            byte_str data;
        };
    }
}

#endif /* EPYX_N2NP_PACKET_H */

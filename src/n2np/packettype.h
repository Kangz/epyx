/**
 * @file packettype.h
 * @brief N2NP Packet Type
 */

#ifndef EPYX_N2NP_PACKET_TYPE_H
#define EPYX_N2NP_PACKET_TYPE_H

#include <ostream>
#include <string>

namespace Epyx
{
    namespace N2NP
    {
        /**
         * @brief Maximum length of a packet type
         */
        const size_t PacketTypeMaxLen = 50;
        /**
         * @class PacketType
         * @brief N2NP Packet Type
         *
         * This class helps to manage sub types. For example N2NP/ping and
         * N2NP/message belongs to the same master "N2NP" but not DHT/request
         */
        class PacketType
        {
        public:
            /**
             * @brief Empty constructor
             */
            PacketType();

            /**
             * @brief Constructor
             * @param type packet type
             */
            PacketType(const std::string& type);

            /**
             * @brief Print the packet type in an output stream
             * @param os output stream
             * @param pkttype
             */
            friend std::ostream& operator<<(std::ostream& os, const PacketType& pkttype);

            /**
             * @brief Return a std::string representation of the packet type
             * @return std::string
             */
            std::string toString() const;


            /**
             *  @brief Comparator, to be able to index types in a map
             */
            friend bool operator<(const PacketType& pkttype1, const PacketType& pkttype2);

        private:
            std::string type;
        };
    }
}

#endif /* EPYX_N2NP_PACKET_TYPE_H */

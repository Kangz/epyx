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
 * @file dht/node.h
 */

#ifndef EPYX_DHT_NODE_H
#define EPYX_DHT_NODE_H

#include "../core/common.h"
#include "../n2np/module.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    class Packet;

    /**
     * @class Node
     *
     * @brief The public interface of the DHT
     *
     * The node is basically doing nothing. All the complexity is hidden in the
     * InternalNode. Node is only proxying thing to InternalNode with a bit of
     * preprocessing (N2NP Packet -> DHT Packet) and hides the N2NP from the
     * InternalNode by offering a N2NP-agnostic send function.
     * It also has some logic for making Synchronous DHT calls.
     */
    class Node : public N2NP::Module
    {
    public:

        /**
         * @method the constructor of Node
         * @param id this Node DHT id
         * @param n2npSelf the N2NP Node associated with this Node
         * @param name the name to be used for the threads
         */
        Node(const Id& id, N2NP::Node& n2npSelf, const std::string& name);

        /**
         * @brief the Node destructor
         */
        ~Node();

        /**
         * @brief receives a packet from the N2NP
         * @param myself the N2np node that received the message (in case we forgot)
         * @param senderId the N2NP sender id
         * @param data the raw data
         */
        void fromN2NP(N2NP::Node& myself, N2NP::NodeId senderId, const byte_str& data);

        /**
         * @private send
         * @brief sends a N2NP message from a DHT Packet
         * @param pkt the Packet to send
         * @param dest the target of the message
         * @param myN2np self explaining
         */
        void send(Packet& pkt, Peer::SPtr dest, N2NP::Node& myN2np);

        /**
         * @brief sends a ping to a remote DHT
         * @param p the DHT to ping
         *
         * This is mainly used to boostrap our connection (as the remote DHT
         * will answer a PONG and get registered in our KBucket)
         */
        void sendPing(Peer::SPtr p);

        /**
         * @brief gets the DHT peer describing this DHT
         * @return itself, as a Peer
         */
        Peer::SPtr getConnectionInfo() const;

        /**
         * @brief Asks a FIND operation on the DHT
         * @param the user-defined callback to be used when the operation is completed
         * @param count the number of neighbors to find
         * @param idToFind the neighborhood to scan
         */
        void findClosest(FindCallback* cb, int count, const Id& idToFind);

        /**
         * @brief Asks a GET operation on the DHT
         * @param the user-defined callback to be used when the operation is completed
         * @param key what to fing
         */
        void getValue(GetCallback* cb, const std::string& key);

        /**
         * @brief Asks a SET operation on the DHT
         * @param the user-defined callback to be used when the operation is completed
         * @param key which key to set
         * @param value with which value
         */
        void setValue(SetCallback* cb, const std::string& key, const std::string& value);

        /**
         * @brief Asks a GET operation on the DHT, synchronous
         * @param key the key to find
         * @param result the storage for the result
         * @return true if it succeded, false otherwise
         */
        bool getValueSync(const std::string& key, std::string& result);

        /**
         * @brief Asks a SET operation on the DHT, synchronous
         * @param key the key to set
         * @param value the value to set
         * @return true if it succeded, false otherwise
         */
        bool setValueSync(const std::string& key, const std::string& value);

    private:
        Id id;
        GTTParser gttParser;
        InternalNode n;
    };

}
}

#endif //EPYX_DHT_NODE_H

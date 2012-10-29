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
 * @file dht/peer.h
 */
#ifndef EPYX_DHT_PEER_H
#define EPYX_DHT_PEER_H

#include "../n2np/nodeid.h"
#include "id.h"
namespace Epyx
{
namespace DHT
{
    class Peer
    {
    public:
        Id id;
        N2NP::NodeId n2npId;
        time_t lastReceiveTime;

        Peer();
        Peer(const Id& id, const N2NP::NodeId& n2npId, time_t lastReceiveTime);
        Peer(const Id& id, const N2NP::NodeId& n2npId);
        Peer(const N2NP::NodeId& n2npId);

        void serialize(std::ostream& o) const;
        void unserialize(std::istream& i);

        friend std::ostream& operator<<(std::ostream& os, const Peer& peer);
        friend std::istream& operator>>(std::istream& is, Peer& peer);
    };

}
}
#endif /*EPYX_DHT_PEER_H*/

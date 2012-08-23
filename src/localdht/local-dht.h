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
 * Local Relay management
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_DHT_H
#define EPYX_LOCAL_DHT_H

#include <boost/noncopyable.hpp>
#include "../n2np/packet.h"
#include "../localn2np/local-node.h"
#include "../core/thread.h"

namespace Epyx{

    class LocalDHT : private boost::noncopyable
    {

    public:
        LocalDHT(LocalNode& n);

    private:
        LocalNode& node;

        void receive(LocalNode& node, const N2npPacket& pkt);
        static bool receive_packet(LocalNode& node, const N2npPacket& pkt, void* arg_);
        
    };
}

#endif /* EPYX_LOCAL_DHT_H */

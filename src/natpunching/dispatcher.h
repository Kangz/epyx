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
#ifndef EPYX_DIRECTCONNECTION_DISPATCHER_H
#define EPYX_DIRECTCONNECTION_DISPATCHER_H

#include "../n2np/module.h"

namespace Epyx {
    namespace DirectConnection {
        
        class Dispatcher : public N2NP::Module
        {
            public:
                void fromN2NP(N2NP::Node &node, N2NP::NodeId from, const byte_str& data);
        };
        
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_DISPATCHER_H

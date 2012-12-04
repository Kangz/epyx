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
#ifndef EPYX_DIRECTCONNECTION_MODULE_H
#define EPYX_DIRECTCONNECTION_MODULE_H

#include "openconnection.h"
#include "../n2np/nodeid.h"
#include "../net/netselect.h"
#include <map>
#include <memory>
#include <stack>

namespace Epyx
{
    namespace DirectConnection
    {
        class Module
        {
        public:
            // Will Call DirectConnection::OpenConnection class which opens an independant Thread
            // Use to be the Client
            static std::shared_ptr<OpenConnection> openDirectConnection(const std::shared_ptr<N2NP::Node>& node,
                    const N2NP::NodeId& remoteHost);
            // Use to be the server
            static std::shared_ptr<OpenConnection> openDirectConnectionServer(const std::shared_ptr<N2NP::Node>& node,
                    const N2NP::NodeId& remoteHost);
        };
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_MODULE_H

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
 * @file openconnect.h
 * @brief Open a connection through NAT punching
 */
#ifndef OPENCONNECT_H
#define OPENCONNECT_H

#include "../../net/socket.h"
#include "../../net/netselect.h"
#include "../../net/uri.h"
#include "../../core/log.h"
#include "igd.h"

#include <string>
#include <map>

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Natpunch
         * @brief UPnP Nat Punghing implementation
         */
        class Natpunch
        {
        public:
            /**
             * @brief constructor
             */
            Natpunch();

            /**
             * @brief creating a port mapping behind a NAT
             * @param localPort
             * @param remotePort
             * @return Address of the NAT router
             */
            const SockAddress openMapPort(unsigned short localPort, unsigned short remotePort = 0);

            bool delMapPort(const SockAddress& addr, protocol proto = Epyx::UPNP::TCP);

            /**
             * Print a list of mappings
             */
            void printMapList();
        private:
            void testConnection();
            void closeMapPort();

            std::unique_ptr<IGD> igd;
        };
    }
}

#endif // OPENCONNECT_H

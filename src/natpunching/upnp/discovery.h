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
 * @file discovery.h
 * @brief UPnP discovery implementation
 */
#ifndef EPYX_UPNP_DISCOVERY_H
#define EPYX_UPNP_DISCOVERY_H

#include "../../net/netquery.h"
#include "../../net/uri.h"
#include "../../parser/httpparser.h"
#include "igd.h"

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Discovery
         * @brief UPnP discovery implementation with NetSelectSocket
         */
        class Discovery : public NetQuery<URI>
        {
        public:
            /**
             * @brief Constructor
             */
            Discovery();
            
            /**
             * @brief Launch a discovery request to discover IGD
             * @param timeout
             * @return a pointer to an IGD object, empty if nothing were found
             */
            static std::unique_ptr<IGD> discoverIGD(const Timeout& timeout);

            /**
             * @brief Send a search request through a UDP socket
             */
            bool query();

        protected:
            std::unique_ptr<URI> eat(const byte_str& data);

        private:
            HTTPParser htpars;
        };
    }
}

#endif /* EPYX_UPNP_DISCOVERY_H */

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
 * @file igd.h
 * @brief Internet Gateway Device Protocol implementation
 */

#ifndef EPYX_IGD_H
#define EPYX_IGD_H

#include <map>
#include <string>
#include <list>
#include "../../net/uri.h"
#include "../../../include/tinyxml/tinyxml.h"
#include "command.h"

namespace Epyx
{
    namespace UPNP
    {
        typedef struct portMap
        {
            bool enabled;
            SockAddress destination;
            unsigned short nat_port;
            std::string protocol;
            std::string description;
        } portMap;
        typedef enum prot
        {
            TCP,
            UDP
        } protocol;
        /**
         * @class IGD
         * @brief Internet Gateway Device Protocol implementation
         */
        class IGD
        {
        public:
            /**
             * @brief constructor
             * @param rootDescURI URI to rootDesc.xml
             */
            IGD(const URI& rootDescURI);

            /**
             * @brief Load services list
             * @return true if successful, false otherwise
             */
            bool getServices();

            /**
             * @brief Get external IP address
             * @return string
             */
            IpAddress getExtIPAdress() const;

            /**
             * @brief Get list of port map
             * @return
             */
            std::list<portMap> getListPortMap() const;
            
            /**
             * @brief Pick a random port which is free in port mapping
             * @return 
             */
            unsigned short pickRandomFreePort(protocol proto) const;

            /**
             * Returns Address class with which we can join the user from
             * the internet. For compatibility reasons the remote Port must
             * be the same as the local port
             */
            const SockAddress addPortMap(unsigned short port, protocol proto) const;
            const SockAddress addPortMap(unsigned short loc_port, protocol proto, unsigned short ext_port) const;

            /**
             * @brief Gets the local IP adress which communicates with the IGD.
             * @return
             */
            IpAddress getLocalAdress() const;

            /**
             * @brief Delete a port map
             * @param addr
             * @param proto TCP or UDP
             *
             * Addr must be the IP TCP/UDP coordinates for access to user machine
             * through IGD NAT Device. (Ex the return value of addPortMap)
             */
            bool delPortMap(const SockAddress& addr, protocol proto) const;

            /**
             * @brief Get list of services
             */
            std::map<std::string, std::string> getServiceList() const;

        private:
            /**
             * @brief URI to rootDesc.xml file
             */
            URI rootDescURI;

            /**
             * @brief IGD address
             */
            SockAddress address;

            /**
             * @brief IGD services
             *
             * First : Name of service.
             * Second : Control Path.
             * Let's Assume all commands of a service are always supported...
             */
            std::map<std::string, std::string> services;

            /**
             * @brief Retrieve urn:schemas-upnp-org:service:WANIPConnection
             * @param command Filled Command object
             * @return true on success
             */
            bool initWanIPConnCommand(Command *command) const;
        };
    }
}

#endif // EPYX_IGD_H

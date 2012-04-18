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

namespace Epyx
{
    namespace UPNP
    {
        /*typedef struct serviceInfo{
                std::string def_path;
                std::string ctl_path;
        }serviceInfo;
         */ // I think the struct might be useless. we'll see
        typedef struct portMap
        {
            bool enabled;
            Epyx::Address destination;
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
            std::string getExtIPAdress();

            /**
             * @brief Get list of port map
             * @return
             */
            std::list<portMap> getListPortMap();

            /**
             * Returns Address class with which we can join the user from
             * the internet. For compatibility reasons the remote Port must
             * be the same as the local port
             */
            const Address addPortMap(unsigned short port, protocol proto);
            const Address addPortMap(unsigned short loc_port, protocol proto, unsigned short ext_port);

            /**
             * @brief Gets the local IP adress which communicates with the IGD.
             * @return
             */
            std::string getLocalAdress();

            /**
             * @brief Delete a port map
             * @param addr
             * @param proto TCP or UDP
             *
             * Addr must be the IP TCP/UDP coordinates for access to user machine
             * through IGD NAT Device. (Ex the return value of addPortMap)
             */
            bool delPortMap(const Address& addr, protocol proto);

            /**
             * @brief Get list of services
             */
            std::map<std::string, std::string> getServiceList();

        private:
            /**
             * @brief URI to rootDesc.xml file
             */
            URI rootDescURI;

            /**
             * @brief IGD address
             */
            Address address;

            /**
             * @brief IGD services
             *
             * First : Name of service.
             * Second : Control Path.
             * Let's Assume all commands of a service are always supported...
             */
            std::map<std::string, std::string> services;
        };
    }
}

#endif // EPYX_IGD_H

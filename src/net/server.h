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
 * @file server.h
 * @brief Basic Internet server class
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include "address.h"

namespace Epyx
{
    /**
     * @class Server
     * @brief Server interface
     */
    class Server
    {
    public:
        /**
         * @brief Create a new generic unbinded server
         * @param addr
         */
        Server(const Address& addr);
        ~Server();

        /**
         * @brief Stop the server (shutdown + close)
         */
        void close();

        /**
         * @brief Tell wether server is binded or not
         * @return true if server is binded
         */
        bool isBinded() const;

        /**
         * @brief get server address
         * @return aserver Address object
         */
        const Address& getAddress() const;


        /**
         * @brief Return internal file descriptor
         */
        int getFd() const;

    protected:
        /**
         * @brief Server binded address
         */
        Address address;
        /**
         * @brief Server socket file descriptor
         */
        int sockfd;

        /**
         * @brief Common code for TCP and UDP bind
         * @param socktype: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
         * @return true on success
         */
        bool _internal_bind(int socktype);

    private:
        // Disable copy
        Server(const Server&);
        const Server& operator=(const Server&);
    };

}
#endif // EPYX_SERVER_H

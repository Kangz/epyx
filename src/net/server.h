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
         * @param port
         */
        Server(unsigned short port);
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
         * @brief Server listening port
         */
        unsigned short port;
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

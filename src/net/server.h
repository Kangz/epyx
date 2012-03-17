/**
 * Basic Internet server class
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include "address.h"

namespace Epyx
{
    class Server
    {
    public:
        Server(unsigned short port);
        ~Server();

        inline void setPort(unsigned short port_)
        {
            this->port = port_;
        }
        const Address& getAddress();

        /**
         * Start the server (socket + bind + listen)
         */
        virtual bool bind() = 0;

        /**
         * Stop the server (shutdown + close)
         */
        void close();

        /**
         * @brief Return internal file descriptor
         */
        inline int getFd()
        {
            return sockfd;
        }

    protected:
        Address address;
        unsigned short port;
        int sockfd;
        // Another thread tells the server to shut down
        bool running;

        /**
         * Common code for TCP and UDP bind
         * @param socktype: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
         * @return true on success
         */
        bool _internal_bind(int socktype);
    };

}
#endif // EPYX_SERVER_H

/**
 * Addresses and Ports management
 */
#ifndef EPYX_ADDRESS_H
#define EPYX_ADDRESS_H

#include <iostream>
#include <arpa/inet.h>

namespace Epyx
{
    class Address
    {
    private:
        unsigned int port;
        char ipStr[INET6_ADDRSTRLEN + 1];
        int ipVersion;//4 or 6

    public:
        Address();
        Address(const char *ip, unsigned int port_, int ipVersion_);
        Address(const struct sockaddr *saddr);
        friend std::ostream& operator<<(std::ostream& os, const Address& addr);
        friend std::ostream& operator<<(std::ostream& os, const Address *addr);

        friend bool operator==(const Address& addr1, const Address& addr2);

        friend inline bool operator!=(const Address& addr1, const Address& addr2)
        {
            return !(addr1 == addr2);
        }
    };
}

#endif /* EPYX_ADDRESS_H */

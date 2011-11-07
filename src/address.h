/**
 * Addresses and Ports management
 */
#include <iostream>
#include <arpa/inet.h>

namespace Epyx
{
    class Address
    {
    private:
        unsigned int port;
        char ipStr[INET6_ADDRSTRLEN];
        int ipVersion;//4 or 6

    public:
        Address(const struct sockaddr *saddr);
        friend std::ostream& operator<<(std::ostream& os, Address& addr);
    };
}

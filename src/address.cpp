#include "address.h"
#include "exception.h"

namespace Epyx
{
    Address::Address(const struct sockaddr *saddr)
    {
        if (!saddr)
            throw FailException("Address", "sockaddr is NULL");
        if (saddr->sa_family == AF_INET) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)saddr;
            this->port = ntohs(ipv4->sin_port);
            inet_ntop(AF_INET, &(ipv4->sin_addr), this->ipStr,
                      sizeof (this->ipStr));
            this->ipVersion = 4;
        } else if (saddr->sa_family == AF_INET6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)saddr;
            this->port = ntohs(ipv6->sin6_port);
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), this->ipStr,
                      sizeof (this->ipStr));
            this->ipVersion = 6;
        } else
            throw FailException("Address", "You have just invented a new IP \
                version without giving me informations about how to handle it");
    }

    std::ostream& operator<<(std::ostream& os, Address& addr)
    {
        if (addr.ipVersion == 6)
            os << '[' << addr.ipStr << ']';
        else
            os << addr.ipStr;

        if (addr.port)
            os << ':' << addr.port;
    }
}

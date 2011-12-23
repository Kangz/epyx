#include "address.h"
#include "exception.h"
#include <sstream>
#include <string.h>

namespace Epyx
{
    Address::Address()
        :port(0), ipVersion(0)
    {
        strcpy(this->ipStr, "*");
    }

    Address::Address(const char *ip, unsigned int port_, int ipVersion_)
        :port(port_), ipVersion(ipVersion_)
    {
        strncpy(this->ipStr, ip, INET6_ADDRSTRLEN);
        this->ipStr[INET6_ADDRSTRLEN] = 0;
    }

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
        } else {
            std::ostringstream out;
            out << "You have just invented a new IP version without giving me" \
                << " information about how to handle it\n" \
                << "The version is: " << saddr->sa_family << "\n" \
                << "IPv4 is: " << AF_INET << "\n" \
                << "IPv6 is: " << AF_INET6;
            throw FailException("Address", out.str().c_str());
        }
    }

    std::ostream& operator<<(std::ostream& os, const Address& addr)
    {
        if (addr.ipVersion == 6)
            os << '[' << addr.ipStr << ']';
        else
            os << addr.ipStr;

        if (addr.port)
            os << ':' << addr.port;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Address *addr)
    {
        if (!addr)
            return os << "(null)";
        else
            return os << (*addr);
    }

    bool operator==(const Address& addr1, const Address& addr2)
    {
        return (addr1.port == addr2.port)
            && (addr1.ipVersion == addr2.ipVersion)
            && !strncmp(addr1.ipStr, addr2.ipStr, INET6_ADDRSTRLEN);
    }
}

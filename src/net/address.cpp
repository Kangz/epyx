#include "address.h"
#include "../core/common.h"
#include <sstream>
#include <string.h>

namespace Epyx
{
    Address::Address()
        :port(0), ipVersion(0)
    {
        strcpy(this->ipStr, "*");
    }

    Address::Address(const char *ip, unsigned short port_, int ipVersion_)
        :port(port_), ipVersion(ipVersion_)
    {
        strncpy(this->ipStr, ip, INET6_ADDRSTRLEN);
        this->ipStr[INET6_ADDRSTRLEN] = 0;
    }

    Address::Address(const Address& addr)
        :port(addr.port), ipVersion(addr.ipVersion)
    {
        strncpy(this->ipStr, addr.ipStr, INET6_ADDRSTRLEN);
        this->ipStr[INET6_ADDRSTRLEN] = 0;
    }

    Address::Address(const struct sockaddr *saddr)
    {
        EPYX_ASSERT(saddr != NULL);

        if (saddr->sa_family == AF_INET) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)saddr;
            this->port = ntohs(ipv4->sin_port);
            inet_ntop(AF_INET, &(ipv4->sin_addr), this->ipStr,
                      sizeof(this->ipStr));
            this->ipVersion = 4;
        } else if (saddr->sa_family == AF_INET6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)saddr;
            this->port = ntohs(ipv6->sin6_port);
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), this->ipStr,
                      sizeof(this->ipStr));
            this->ipVersion = 6;
        } else {
            Epyx::log::fatal << "You have just invented a new IP version" \
                << "without giving me information about how to handle it\n" \
                << "The version is: " << saddr->sa_family << "\n" \
                << "IPv4 is: " << AF_INET << "\n" \
                << "IPv6 is: " << AF_INET6 \
                << Epyx::log::endl;
            throw FailException("Address", "Bad IP Version");
        }
    }

    const char* Address::ip(){
        return this->ipStr;
    }

    unsigned short Address::getPort(){
        return this->port;
    }

    void Address::getSockAddr(struct sockaddr *saddr)
    {
        EPYX_ASSERT(saddr != NULL);
        EPYX_ASSERT(this->ipVersion > 0);
        if (this->ipVersion == 4) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)saddr;
            ipv4->sin_family = AF_INET;
            inet_pton(AF_INET, this->ipStr, &(ipv4->sin_addr));
            ipv4->sin_port = htons(this->port);
            memset(&(ipv4->sin_zero), 0, sizeof(ipv4->sin_zero));
        } else if (this->ipVersion == 6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)saddr;
            ipv6->sin6_family = AF_INET6;
            ipv6->sin6_flowinfo = 0;
            ipv6->sin6_scope_id = 0;
            inet_pton(AF_INET6, this->ipStr, &(ipv6->sin6_addr));
            ipv6->sin6_port = htons(this->port);
        } else {
            throw FailException("Address", "Bad IP Version");
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

    std::string Address::toString()
    {
        std::ostringstream stream;
        stream << *this;
        return stream.str();
    }


    bool operator==(const Address& addr1, const Address& addr2)
    {
        return (addr1.port == addr2.port)
            && (addr1.ipVersion == addr2.ipVersion)
            && !strncmp(addr1.ipStr, addr2.ipStr, INET6_ADDRSTRLEN);
    }
}

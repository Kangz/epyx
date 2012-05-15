#include "address.h"
#include "../core/common.h"
#include <sstream>
#include <string.h>

namespace Epyx
{

    Address::Address(const std::string& addressFormat)
    :ip(), port(0), ipVersion(0) {
        // portstr may be a ":port" suffix
        std::string portstr = "";
        if (addressFormat.empty())
            return;
        if (addressFormat.at(0) == '[') {
            //Case if IPv6
            size_t n = addressFormat.find(']');
            if (n == std::string::npos) {
                // TODO: Better sanity checks
                log::fatal << "Unknown address " << addressFormat << log::endl;
                throw ParserException("Address", "Invalid IPv6 address");
            }
            ip = addressFormat.substr(1, n - 1);
            portstr = addressFormat.substr(n + 1);
            ipVersion = 6;
        } else {
            //If not IPv6, it's IPv4, obviously :P
            size_t n = addressFormat.find(':');
            if (n == std::string::npos) {
                ip = addressFormat;
            } else {
                ip = addressFormat.substr(0, n);
                portstr = addressFormat.substr(n);
            }
            // TODO: write here some sanity check about an IP address
            ipVersion = 4;
        }
        // Use port
        if (!portstr.empty() && portstr[0] == ':')
            port = (unsigned short) String::toInt(portstr.substr(1));
        else
            port = 0;
    }

    Address::Address()
    :ip(), port(0), ipVersion(0) {
    }

    Address::Address(const std::string& ip, unsigned short port, int ipVersion)
    :ip(ip), port(port), ipVersion(ipVersion) {
    }

    Address::Address(const struct sockaddr *saddr) {
        EPYX_ASSERT(saddr != NULL);
        char ipBuffer[INET6_ADDRSTRLEN + 1];

        if (saddr->sa_family == AF_INET) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) saddr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipBuffer, sizeof (ipBuffer));
            this->ip.assign(ipBuffer);
            this->port = ntohs(ipv4->sin_port);
            this->ipVersion = 4;
        } else if (saddr->sa_family == AF_INET6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) saddr;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipBuffer, sizeof (ipBuffer));
            this->ip.assign(ipBuffer);
            this->port = ntohs(ipv6->sin6_port);
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

    Address::Address(const Address& addr)
    :ip(addr.ip), port(addr.port), ipVersion(addr.ipVersion) {
    }

    Address& Address::operator=(const Address& addr) {
        if (this != &addr) {
            this->ip = addr.ip;
            this->port = addr.port;
            this->ipVersion = addr.ipVersion;
        }
        return *this;
    }

    const std::string& Address::getIp() const {
        return ip;
    }

    unsigned short Address::getPort() const {
        return port;
    }

    void Address::getSockAddr(struct sockaddr *saddr) const {
        EPYX_ASSERT(saddr != NULL);
        EPYX_ASSERT(this->ipVersion > 0);
        if (this->ipVersion == 4) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) saddr;
            memset(&(ipv4->sin_zero), 0, sizeof (ipv4->sin_zero));
            ipv4->sin_family = AF_INET;
            inet_pton(AF_INET, ip.c_str(), &(ipv4->sin_addr));
            ipv4->sin_port = htons(this->port);
        } else if (this->ipVersion == 6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) saddr;
            ipv6->sin6_family = AF_INET6;
            ipv6->sin6_flowinfo = 0;
            ipv6->sin6_scope_id = 0;
            inet_pton(AF_INET6, ip.c_str(), &(ipv6->sin6_addr));
            ipv6->sin6_port = htons(this->port);
        } else {
            throw FailException("Address", "Bad IP Version");
        }
    }

    std::ostream& operator<<(std::ostream& os, const Address& addr) {
        if (addr.ipVersion == 6)
            os << '[' << addr.ip << ']';
        else if (addr.ip.length())
            os << addr.ip;
        else
            os << '*';

        if (addr.port)
            os << ':' << addr.port;
        return os;
    }

    std::string Address::toString() const {
        std::ostringstream stream;
        stream << *this;
        return stream.str();
    }

    bool operator==(const Address& addr1, const Address& addr2) {
        return (addr1.port == addr2.port)
            && (addr1.ipVersion == addr2.ipVersion)
            && !addr1.ip.compare(addr2.ip);
    }

    bool operator!=(const Address& addr1, const Address& addr2) {
        return !(addr1 == addr2);
    }
}

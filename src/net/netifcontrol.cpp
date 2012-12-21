#include "netifcontrol.h"
#include "../core/common.h"
#include "netif.h"
#include <cstring>
#include <sys/ioctl.h>

namespace Epyx
{

    NetIfControl::NetIfControl(const std::string& ifname, int ipVersion)
    :ifname(ifname) {
        int family = 0;
        switch (ipVersion) {
            case 4:
                family = PF_INET;
                break;
            case 6:
                family = PF_INET6;
                break;
            default:
                throw FailException("NetIfControl", "Bad IP version");
        }
        sock = ::socket(family, SOCK_DGRAM, 0);
        if (sock < 0) {
            throw ErrException("NetIfControl", "Unable to open a socket");
        }
    }

    NetIfControl::NetIfControl(const NetIf& netif) {
        ifname = netif.getName();
        // If IPv6, use IPv6 socket. If no address or IPv4 address, use IPv4
        if (netif.getAddress().getVersion() == 6) {
            sock = ::socket(PF_INET6, SOCK_DGRAM, 0);
        } else {
            sock = ::socket(PF_INET, SOCK_DGRAM, 0);
        }
        if (sock < 0) {
            throw ErrException("NetIfControl", "Unable to open a socket");
        }
    }

    NetIfControl::~NetIfControl() {
        if (sock >= 0) {
            ::close(sock);
            sock = -1;
        }
    }

    const std::string& NetIfControl::getIfName() const {
        return ifname;
    }

    bool NetIfControl::setMTU(int mtu) {
        struct ifreq ifr;
        ifr.ifr_mtu = mtu;
        return this->ioctlReq(SIOCSIFMTU, &ifr);
    }

    int NetIfControl::getMTU() const {
        struct ifreq ifr;
        return this->ioctlReq(SIOCGIFMTU, &ifr) ? ifr.ifr_mtu : 0;
    }

    bool NetIfControl::getHwAddr(byte hwaddr[6]) const {
        struct ifreq ifr;
        if (!this->ioctlReq(SIOCGIFHWADDR, &ifr)) return false;
        memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, 6);
        return true;
    }

    bool NetIfControl::activateInterface(bool up) {
        struct ifreq ifr;
        if (!ioctlReq(SIOCGIFFLAGS, &ifr)) return false;
        ifr.ifr_flags = (up ? ifr.ifr_flags | IFF_UP : ifr.ifr_flags & ~IFF_UP);
        return ioctlReq(SIOCSIFFLAGS, &ifr);
    }

    bool NetIfControl::isUp() const {
        struct ifreq ifr;
        if (!this->ioctlReq(SIOCGIFFLAGS, &ifr)) return false;
        return (bool)(ifr.ifr_flags & IFF_UP);
    }

    bool NetIfControl::enableBroadcast(bool enable) {
        struct ifreq ifr;
        if (!ioctlReq(SIOCGIFFLAGS, &ifr)) return false;
        ifr.ifr_flags = (enable
            ? ifr.ifr_flags | IFF_BROADCAST
            : ifr.ifr_flags & ~IFF_BROADCAST);
        return ioctlReq(SIOCSIFFLAGS, &ifr);
    }

    bool NetIfControl::ioctlReq(int request, struct ifreq *ifr) const {
        EPYX_ASSERT(sock >= 0);
        EPYX_ASSERT(ifr != nullptr);
        strncpy(ifr->ifr_name, ifname.c_str(), sizeof (ifr->ifr_name));
        if ((::ioctl(sock, request, ifr)) < 0) {
            log::error << "Socket IO Ctl error on " << ifname << ": "
                << log::errstd << log::endl;
            return false;
        }
        return true;
    }
}

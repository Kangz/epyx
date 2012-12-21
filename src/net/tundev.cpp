#include "tundev.h"
#include "netifcontrol.h"
#include "../core/common.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>

namespace Epyx
{

    Tundev::Tundev(const std::string& ifname)
    :NetIfControl(open(ifname)) {
    }

    const std::string Tundev::open(const std::string& ifname) {
        fd = ::open("/dev/net/tun", O_RDWR);
        if (fd < 0) {
            throw ErrException("TunDev", "open(/dev/net/tun)");
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof (ifr));
        ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
        if (!ifname.empty()) {
            strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ);
        }
        if ((ioctl(fd, TUNSETIFF, (void*) &ifr)) < 0) {
            ::close(fd);
            throw ErrException("TunDev", "ioctl(create interface)");
        }
        return std::string(ifr.ifr_name);
    }

    Tundev::~Tundev() {
        if (fd >= 0) {
            ::close(fd);
            fd = -1;
        }
    }

    int Tundev::read(void *buffer, size_t nbytes) {
        EPYX_ASSERT(buffer != nullptr);
        EPYX_ASSERT(nbytes > 0);
        return ::read(fd, buffer, nbytes);
    }

    int Tundev::write(const void *buffer, size_t nbytes) {
        EPYX_ASSERT(buffer != nullptr);
        EPYX_ASSERT(nbytes > 0);
        return ::write(fd, buffer, nbytes);
    }
}

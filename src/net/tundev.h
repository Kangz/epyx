/**
 * @file tundev.h
 * @brief Tun network device management system to receive or emit local IP packets
 */

#ifndef EPYX_NET_TUNDEV_H
#define EPYX_NET_TUNDEV_H

#include <string>
#include "ipaddress.h"
#include "netifcontrol.h"

namespace Epyx
{
    class Tundev : public NetIfControl
    {
    public:
        /**
         * @brief Open this device
         * @param ifname Interface name
         */
        Tundev(const std::string& ifname = "");
        virtual ~Tundev();

        /**
         * @brief Read at most nbytes from the device in buffer
         * @param buffer
         * @param nbytes
         * @return number of bytes read
         */
        int read(void *buffer, size_t nbytes);

        /**
         * @brief Write at most nbytes to the device from buffer
         * @param buffer
         * @param nbytes
         * @return number of bytes written
         */
        int write(const void *buffer, size_t nbytes);

    private:
        /**
         * @brief Open a TUN device
         * @param ifname
         * @return final ifname
         * @throw Exception if a problem occurs
         */
        const std::string open(const std::string& ifname);

        // File descriptor to device
        int fd;
    };
}

#endif /* EPYX_NET_TUNDEV_H */


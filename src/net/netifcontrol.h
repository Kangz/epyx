/**
 * @file netifcontrol.h
 * @brief Manage IOCtl to an interface
 */

#ifndef EPYX_NET_CTLSOCKET_H
#define EPYX_NET_CTLSOCKET_H

#include <boost/noncopyable.hpp>
#include <net/if.h>
#include <string>
#include "../core/byte.h"
#include "netif.h"

namespace Epyx
{
    /**
     * @class CtlSocket
     * @brief Socket for IOCTL on some devices
     */
    class NetIfControl : private boost::noncopyable
    {
    public:
        /**
         * @brief Build a control object from a device name
         * @param devname
         * @param ipVersion
         */
        NetIfControl(const std::string& devname, int ipVersion = 4);

        /**
         * @brief Constructor from a NetIf object
         */
        NetIfControl(const NetIf& netif);

        ~NetIfControl() throw ();

        /**
         * Cleanly close the interface
         */
        void close() throw ();

        /**
         * @brief Set Maximal Transmission Unit of the device interface
         * @param mtu
         * @return true on success
         */
        bool setMTU(int mtu);

        /**
         * @brief Get Maximal Transmission Unit
         * @return MTU, or 0 if ioctl failed
         */
        int getMTU() const;

        /**
         * @brief Get hardware address
         * @param addr hardware address (6 bytes)
         * @return false on error
         */
        bool getHwAddr(byte hwaddr[6]) const;

        /**
         * @brief Activate the interface (up/down)
         * @param up true to bring up, false to tear down
         * @return true if successful
         */
        bool activateInterface(bool up);

        /**
         * @brief Tell wether the interface is UP
         * @return 
         */
        bool isUp() const;

        /**
         * @brief Enable broadcast flag on the interface
         * @param enable
         * @return true if successful
         */
        bool enableBroadcast(bool enable);

    private:
        bool ioctlReq(int request, struct ifreq *ifr) const;

        std::string devname;
        int sock;
    };
}

#endif /* EPYX_NET_CTLSOCKET_H */

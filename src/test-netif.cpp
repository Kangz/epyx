/**
 * This program tests Network interfaces management system
 */

#include <cstring>
#include <iomanip>
#include "api.h"
#include "net/netifcontrol.h"

using namespace Epyx;

void test_netif() {
    std::vector<NetIf> ifaces = NetIf::getAllNet();
    log::info << "Interfaces:" << log::endl;
    for (auto iface = ifaces.begin(); iface != ifaces.end(); ++iface) {
        NetIfControl ctl(*iface);
        byte hwaddr[6];
        std::string hwaddr_s;
        if (ctl.getHwAddr(hwaddr)) {
            std::stringstream s;
            s << " HWAddr: " << std::hex;
            for (int i = 0; i < 6; i++) {
                if (i > 0) s << ':';
                s << std::setfill('0') << std::setw(2) << (int) hwaddr[i];
            }
            hwaddr_s = s.str();
        }
        log::info << "* " << *iface << ": "
            << (ctl.isUp() ? "UP" : "DOWN")
            << " MTU=" << ctl.getMTU()
            << hwaddr_s
            << log::endl;
    }
}

int main() {
    API epyx;
    test_netif();
    return 0;
}

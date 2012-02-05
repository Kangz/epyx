#include "udpsocket.h"
namespace Epyx{
    UDPSocket::UDPSocket() {
        this->setAddress("");
        this->setPort(0);
        this->sock = ::socket(PF_INET,SOCK_DGRAM,0);
    }
}

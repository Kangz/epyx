#include "udpserver.h"
#include "../core/common.h"


namespace Epyx
{

    UDPServer::UDPServer(const SockAddress& addr)
    :Server(addr) {
        if (!this->_internal_bind(SOCK_DGRAM))
            return;

        // Directly create a socket to receive connections
        sock.setFd(sockfd);
        sock.setLocalAddress(address);
    }

    void UDPServer::close() {
        // Close socket and tell it is closed
        sock.close();
        sockfd = -1;
    }

    int UDPServer::recv(void *data, int size) {
        return sock.recv(data, size);
    }

    int UDPServer::recvFrom(SockAddress* addr, void *data, int size) {
        return sock.recvFrom(addr, data, size);
    }

    int UDPServer::sendTo(const SockAddress& addr, const void *data, int size) {
        return sock.sendTo(addr, data, size);
    }

    void UDPServer::bindToDevice(const std::string& devicename) {
        sock.bindToDevice(devicename);
    }
}

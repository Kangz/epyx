#include "openconnection.h"
#include "../core/string.h"
#include "upnp/openconnect.h"
#include "../parser/gttpacket.h"
#include "listener.h"
#include <unistd.h>

namespace Epyx
{
    namespace DirectConnection
    {
        // In this protocol, the client is stupid and just obey orders
        const std::string OpenConnection::n2npMethodName = "DIRECTCONNECTION";
        const std::string OpenConnection::protoName = "OPENCONNECTION";

        OpenConnection::OpenConnection(const std::shared_ptr<N2NP::Node>& node,
            const N2NP::NodeId &remoteHost, bool clients)
        :remoteHost(remoteHost), state(clients ? STATE_CLIENT : STATE_SERVER),
        client_tried(false), server_tried(false), tested_method(DIRECT),
        node(node), running_thread(&OpenConnection::run, this) {
        }

        OpenConnection::~OpenConnection() {
            log::debug << "OpenConn: waiting for OpenConnect thread" << log::endl;
            running_thread.join();
        }

        void OpenConnection::getMessage(const std::string& command, const std::map<std::string, std::string>& headers) {
            if (command == "SEND" && state == STATE_CLIENT) {
                // Send data to the server
                // Get socket address from the headers
                std::map<std::string, std::string>::const_iterator headAddr = headers.find("Address");
                if (headAddr == headers.end()) {
                    log::error << "OpenConnection error: SEND packet without address in headers"
                        << log::endl;
                    return;
                }
                const SockAddress addr(headAddr->second);

                // Get message to be sent from headers
                std::map<std::string, std::string>::const_iterator headMessage = headers.find("Message");
                if (headMessage == headers.end()) {
                    log::error << "OpenConnection error: SEND packet without message in headers"
                        << log::endl;
                    return;
                }
                const std::string& message = headMessage->second;

                // Create a socket
                log::debug << "OpenConn: send " << message.size() << " bytes to "
                    << addr << log::endl;
                socket.reset(new TCPSocket(addr));
                if (!socket->sendBytes(string2bytes_c(message))) {
                    // Failed. Send Did Not Work Message
                    this->tryNextMethod(true);
                } else {
                    //Success, Do nothing and wait for N2NP confirmation
                }
            } else if (command == "DID_NOT_WORK") {
                this->tryNextMethod(false);
            } else if (command == "ESTABLISHED") {
                // Do Shit, and register the socket to the N2NP module
                node->offerDirectConn(remoteHost, std::move(socket));
            }
        }

        void OpenConnection::run() {
            if (state == STATE_SERVER) {
                Thread::setName("OpenConnection-Server");
                log::debug << "OpenConn: starting server" << log::endl;
                GTTPacket pkt(protoName, "SEND");
                serverStateOpen();
            } else if (state == STATE_CLIENT) {
                Thread::setName("OpenConnection-Client");
                log::debug << "OpenConn: starting client" << log::endl;
            }
        }

        bool OpenConnection::tryNextMethod(bool sendDidNotWorkMessage) {
            if (sendDidNotWorkMessage) {
                GTTPacket pkt(protoName, "DID_NOT_WORK");
                node->send(remoteHost, n2npMethodName, pkt);
            }

            // Was server, try client now
            if (state == STATE_SERVER && !client_tried) {
                log::debug << "OpenConn: let's try client role !" << log::endl;
                server_tried = true;
                state = STATE_CLIENT;
                return true;
            }
            // Was client, try server now
            if (state == STATE_CLIENT && !server_tried) {
                log::debug << "OpenConn: let's try server role !" << log::endl;
                client_tried = true;
                state = STATE_SERVER;
                // Todo: call serverStateOpen() if etat == STATE_SERVER
                return true;
            }

            // Change method
            client_tried = false;
            server_tried = false;
            if (tested_method == DIRECT) {
                log::debug << "OpenConn: let's try UPnP !" << log::endl;
                tested_method = UPNP;
                state = (state == STATE_SERVER ? STATE_CLIENT : STATE_SERVER);
                // Todo: call serverStateOpen() if etat == STATE_SERVER
                return true;
            }
            log::debug << "OpenConn: exhausted connecting methods :(" << log::endl;
            return false;
        }

        void OpenConnection::serverStateOpen() {
            std::string testMessage = "Test";

            //First we open a listening socket on an available port
            NetSelect selectListener(2, "DirectConnListenerW");
            std::thread threadListener(&NetSelect::runLoop, &selectListener, "DirectConnListener");
            std::shared_ptr<Listener> srvListener(new Listener(new TCPServer(SockAddress("0.0.0.0:0"), 20)));
            selectListener.add(srvListener);
            SockAddress addr = SockAddress(node->getNodeAddress().getIp(), srvListener->getListenAddress().getPort());

            //If we're using UPNP, we open a port mapping
            if (tested_method == UPNP) {
                Epyx::UPNP::Natpunch nat;
                //log::fatal << "nat.openMapPort(addr.getPort(),remotePort); not yet implemented" << log::endl;
                addr = nat.openMapPort(addr.getPort());
            }
            //Now, Ask to open a connection.
            GTTPacket testpkt(protoName, "SEND");
            testpkt.headers["Address"] = addr.toString();
            testpkt.headers["Message"] = testMessage;
            node->send(this->remoteHost, n2npMethodName, testpkt);

            // Wait
            std::unique_ptr<TCPSocket> clientSock = srvListener->waitForAccept(5000);
            char data[10];
            size_t size = 0;

            // Stop server
            log::debug << "OpenConn: stopping listener server" << log::endl;
            selectListener.stop();
            threadListener.join();

            if (clientSock) {
                size = clientSock->recv((void*) data, 10);
                if (std::string(data, size) == testMessage) {
                    GTTPacket estabpkt(protoName, "ESTABLISHED");
                    node->offerDirectConn(this->remoteHost, std::move(clientSock));
                    node->send(this->remoteHost, n2npMethodName, estabpkt);
                    return;
                }
                clientSock->close();
            }

            // An error happened
            GTTPacket errpkt(protoName, "DID_NOT_WORK");
            node->send(this->remoteHost, n2npMethodName, errpkt);
            this->getMessage("DID_NOT_WORK", std::map<std::string, std::string > ());
        }
    }
}

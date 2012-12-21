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
        node(node), receivedDidNotWork(false), isEstablished(false),
        running_thread(&OpenConnection::run, this) {
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
                // FIXME: the connect of the socket may take a very long time.
                // It needs to be timeouted.
                log::debug << "(client) send " << message.size() << " bytes to "
                    << addr << log::endl;
                socket.reset(new TCPSocket(addr));
                if (!socket->isOpened() || !socket->sendBytes(string2bytes_c(message))) {
                    // Failed. Send Did Not Work Message
                    log::debug << "(client) failed to send data" << log::endl;
                } else {
                    //Success, Do nothing and wait for N2NP confirmation
                }
            } else if (command == "DID_NOT_WORK") {
                if (state == STATE_SERVER) {
                    log::debug << "(server) received DID_NOT_WORK, sending back one"
                        << log::endl;
                    GTTPacket pkt(protoName, "DID_NOT_WORK");
                    node->send(remoteHost, n2npMethodName, pkt);
                } else if (state == STATE_CLIENT) {
                    log::debug << "(client) received DID_NOT_WORK" << log::endl;
                }

                receivedDidNotWork = true;
                condition.notify_all();
            } else if (command == "ESTABLISHED") {
                // Do Shit, and register the socket to the N2NP module
                node->offerDirectConn(remoteHost, std::move(socket));
                isEstablished = true;
                condition.notify_all();
            }
        }

        void OpenConnection::run() {
            // Start the state machine
            if (state == STATE_SERVER) {
                Thread::setName("OpenConnection-Server");
                log::debug << "OpenConn: starting server" << log::endl;

                // Send the first packet to the client
                GTTPacket pkt(protoName, "OPENCONNECTION");
                node->send(remoteHost, n2npMethodName, pkt);
            } else if (state == STATE_CLIENT) {
                Thread::setName("OpenConnection-Client");
                log::debug << "OpenConn: starting client" << log::endl;
            }

            // Loop over the methods
            while (true) {
                if (state == STATE_SERVER) {
                    // Run server code
                    if (serverStateOpen()) {
                        // Done.
                        return;
                    }
                    // Send DID_NOT_WORK message
                    GTTPacket pkt(protoName, "DID_NOT_WORK");
                    node->send(remoteHost, n2npMethodName, pkt);
                } else {
                    EPYX_ASSERT(state == STATE_CLIENT);
                    // Client waits for messages from the server
                    log::debug << "(client) waiting for messages"
                        << log::endl;
                    while (!receivedDidNotWork && !isEstablished) {
                        std::mutex m;
                        std::unique_lock<std::mutex> lock(m);
                        if (condition.wait_for(lock, std::chrono::seconds(5)) == std::cv_status::timeout) {
                            // Timeout
                            log::debug << "(client) Timeout in waiting DID_NOT_WORK" << log::endl;
                            // The server may decide to change state
                            GTTPacket pkt(protoName, "DID_NOT_WORK");
                            node->send(remoteHost, n2npMethodName, pkt);
                        }
                        if (!node->isSocketOpened()) {
                            log::debug << "(client) Oops, node vanished. Stop !"
                                << log::endl;
                            return;
                        }
                    }
                    if (isEstablished) {
                        return;
                    }
                    receivedDidNotWork = false;
                }

                if (!tryNextMethod()) {
                    log::debug << "OpenConn: exhausted connecting methods :(" << log::endl;
                    return;
                }
            }
        }

        bool OpenConnection::tryNextMethod() {
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
                return true;
            }

            // Change method
            client_tried = false;
            server_tried = false;
            if (tested_method == DIRECT) {
                log::debug << "OpenConn: let's try UPnP !" << log::endl;
                tested_method = UPNP;
                state = (state == STATE_SERVER ? STATE_CLIENT : STATE_SERVER);
                return true;
            }
            return false;
        }

        bool OpenConnection::serverStateOpen() {
            std::string testMessage = "Test";

            // First we open a listening socket on an available port
            NetSelect selectListener(2, "DirectConnListenerW");
            std::thread threadListener(&NetSelect::runLoop, &selectListener, "DirectConnListener");
            std::shared_ptr<Listener> srvListener(new Listener(new TCPServer(SockAddress("0.0.0.0:0"), 20)));
            selectListener.add(srvListener);
            log::debug << "(srv) Started TCP server on " << srvListener->getListenAddress() << log::endl;

            // Get distant address. If we're using UPNP, we open a port mapping
            SockAddress addr(node->getNodeAddress().getIp(),
                srvListener->getListenAddress().getPort());
            if (tested_method == UPNP) {
                Epyx::UPNP::Natpunch nat;
                addr = nat.openMapPort(addr.getPort());
            }
            log::debug << "(srv) Remote address to the TCP server is " << addr << log::endl;

            //Now, Ask to open a connection.
            GTTPacket testpkt(protoName, "SEND");
            testpkt.headers["Address"] = addr.toString();
            testpkt.headers["Message"] = testMessage;
            log::debug << "(srv) Sending SEND message to client" << log::endl;
            node->send(this->remoteHost, n2npMethodName, testpkt);

            // Wait
            std::unique_ptr<TCPSocket> clientSock = srvListener->waitForAccept(5000);

            // Stop server
            log::debug << "(srv) stopping listener server, "
                << (clientSock ? "got incoming" : "got nothing") << log::endl;
            selectListener.stop();
            threadListener.join();

            if (clientSock) {
                char data[10];
                size_t size = clientSock->recv((void*) data, sizeof (data));
                if (std::string(data, size) == testMessage) {
                    log::debug << "(srv) Connection established !" << log::endl;
                    node->offerDirectConn(this->remoteHost, std::move(clientSock));
                    GTTPacket estabpkt(protoName, "ESTABLISHED");
                    node->send(this->remoteHost, n2npMethodName, estabpkt);
                    return true;
                }
                clientSock->close();
            }

            // An error happened
            return false;
        }
    }
}

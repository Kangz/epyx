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
        :remoteHost(remoteHost), etat(clients ? STATE_CLIENT : STATE_SERVER),
        client_tried(false), server_tried(false), tested_method(DIRECT),
        node(node), running_thread(&OpenConnection::run, this) {
        }

        OpenConnection::~OpenConnection() {
            log::debug << "Waiting for OpenConnect thread" << log::endl;
            running_thread.join();
        }

        void OpenConnection::getMessage(const std::string& command, const std::map<std::string, std::string>& headers) {
            if (command == "SEND" && etat == STATE_CLIENT) {
                std::map<std::string, std::string>::const_iterator headAddr = headers.find("Address");
                if (headAddr == headers.end())
                    throw ParserException("OpenConnection", "No address in headers");
                std::map<std::string, std::string>::const_iterator headMessage = headers.find("Message");
                if (headMessage == headers.end())
                    throw ParserException("OpenConnection", "No message in headers");

                SockAddress addr(headAddr->second);
                socket.reset(new TCPSocket(addr));
                if (!socket->sendBytes(string2bytes_c(headMessage->second))) {
                    //Failed
                    //Send Did Not Work Message
                    GTTPacket pkt(protoName, "DID_NOT_WORK");
                    node->send(this->remoteHost, n2npMethodName, pkt);
                    this->getMessage("DID_NOT_WORK", std::map<std::string, std::string > ());
                } else {
                    //Success, Do nothing and wait for N2NP confirmation
                }
            } else if (command == "DID_NOT_WORK") {
                if (etat == STATE_SERVER && !client_tried) {
                    // If we played the server, we now play the client. Easy Case.  
                    server_tried = true;
                } else if (etat == STATE_SERVER) {
                    if (tested_method == DIRECT) {
                        tested_method = UPNP;
                        client_tried = false;
                        server_tried = false;
                        etat = STATE_CLIENT;
                    }
                } else if (etat == STATE_CLIENT && !server_tried) {
                    etat = STATE_SERVER;
                    serverStateOpen();
                } else if (STATE_CLIENT) {
                    if (tested_method == DIRECT) {
                        tested_method = UPNP;
                        client_tried = false;
                        server_tried = false;
                        etat = STATE_SERVER;
                    }
                    serverStateOpen();
                }
            } else if (command == "ESTABLISHED") {
                //Do Shit, and register the socket to the N2NP module
                node->offerDirectConn(remoteHost, std::move(socket));
            }
        }

        void OpenConnection::run() {
            if (etat == STATE_SERVER) {
                Thread::setName("OpenConnection-Server");
                GTTPacket pkt(protoName, "SEND");
                serverStateOpen();
            } else if (etat == STATE_CLIENT) {
                Thread::setName("OpenConnection-Client");
            }
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

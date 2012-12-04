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
                    GTTPacket pkt;
                    pkt.method = "DID_NOT_WORK";
                    node->send(this->remoteHost, "DIRECTCONNECTION", pkt);
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
                GTTPacket pkt;
                pkt.method = "SEND";
                serverStateOpen();
            } else if (etat == STATE_CLIENT) {
                Thread::setName("OpenConnection-Client");
            }
        }

        void OpenConnection::serverStateOpen() {
            std::string testMessage = "Test";

            //First we open a listening socket on an available port
            Listener sockListen(new TCPServer(SockAddress("0.0.0.0:0"), 20));
            SockAddress addr = SockAddress(node->getNodeAddress().getIp(), sockListen.getListenAddress().getPort());
            //If we're using UPNP, we open a port mapping
            if (tested_method == UPNP) {
                Epyx::UPNP::Natpunch nat;
                //log::fatal << "nat.openMapPort(addr.getPort(),remotePort); not yet implemented" << log::endl;
                addr = nat.openMapPort(addr.getPort());
            }
            //Now, Ask to open a connection.
            GTTPacket pkt;
            pkt.method = "SEND";
            pkt.headers["Address"] = addr.toString();
            pkt.headers["Message"] = testMessage;
            node->send(this->remoteHost, "DIRECTCONNECTION", pkt);
            bool accepted = false;
            for (int i = 0; i < 5; i++) {
                accepted = sockListen.hasAccepted();
                if (accepted)
                    break;
                else
                    sleep(1);
            }
            char data[10];
            size_t size = 0;
            std::unique_ptr<TCPSocket> clientSock = sockListen.retrieveSocket();
            if (accepted && clientSock) {
                size = clientSock->recv((void*) data, 10);
                if (std::string(data, size) == testMessage) {
                    pkt.method = "ESTABLISHED";
                    node->offerDirectConn(this->remoteHost, std::move(clientSock));
                    node->send(this->remoteHost, "DIRECTCONNECTION", pkt);
                    return;
                }
            }

            // An error happened
            if (clientSock)
                clientSock->close();

            pkt.method = "DID_NOT_WORK";
            node->send(this->remoteHost, "DIRECTCONNECTION", pkt);
            this->getMessage("DID_NOT_WORK", std::map<std::string, std::string > ());
        }
    }
}

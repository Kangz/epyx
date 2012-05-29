#include "openconnection.h"
#include "../core/string.h"
#include "upnp/openconnect.h"
#include "../parser/gttpacket.h"
#include "listener.h"
namespace Epyx {
    namespace DirectConnection {
        // In this protocol, the client is stupid and just obey orders
        OpenConnection::OpenConnection(N2NP::Node &node, N2NP::NodeId &remoteHost, bool clients) 
        :remoteHost(remoteHost), etat(STATE_SERVER), client_tried(false), server_tried(false),
        tested_method(DIRECT), node(&node), socket(NULL)
        {
            if (clients){
                etat = STATE_CLIENT;
            }
        }
        void OpenConnection::getMessage(const std::string& command, const std::map<std::string, std::string>& headers){
            if (command == "SEND" && etat == STATE_CLIENT){
                std::string message;
                std::map<std::string, std::string>::const_iterator headAddr = headers.find("Address");
                if (headAddr == headers.end())
                    throw ParserException("OpenConnection", "No address in headers");
                std::map<std::string, std::string>::const_iterator headMessage = headers.find("Message");
                if (headMessage == headers.end())
                    throw ParserException("OpenConnection", "No message in headers");

                Address addr(headAddr->second);
                socket = new TCPSocket(addr);
                if (socket->send(headMessage->second.c_str(), headMessage->second.size()) != 0){
                    //Success, Do nothing and wait for N2NP confirmation
                }else{
                    //Failed
                    //Send Did Not Work Message
                    GTTPacket pkt;
                    pkt.method ="DID_NOT_WORK";
                    this->node->send(this->remoteHost,"DIRECTCONNECTION",pkt);
                    this->getMessage("DID_NOT_WORK", std::map<std::string, std::string>());
                }
            }else if (command == "DID_NOT_WORK"){
                if (etat == STATE_SERVER && client_tried == false) // If we played the server, we now play the client. Easy Case.
                    server_tried = true;
                else if (etat == STATE_SERVER){
                    if (tested_method == DIRECT){
                        tested_method = UPNP;
                        client_tried = false;
                        server_tried = false;
                        etat = STATE_CLIENT;
                    }
                }else if(etat == STATE_CLIENT && server_tried == false){
                    etat = STATE_SERVER;
                    serverStateOpen();
                }else if (STATE_CLIENT){
                    if (tested_method == DIRECT){
                        tested_method = UPNP;
                        client_tried = false;
                        server_tried = false;
                        etat = STATE_SERVER;
                    }
                    serverStateOpen();
                }
            }else if (command == "ESTABLISHED") {
                //Do Shit, and register the socket to the N2NP module
                node->offerDirectConn(this->remoteHost,this->socket);
/*            }else if (command == "FAILED" ){ // No Useful yet
                
              }//*/ 
            }
        }
        
        void OpenConnection::serverStateOpen(){
            std::string testMessage ="Test";
            //First we open a listening socket on an available port
            Listener sockListen(new TCPServer(Address("0.0.0.0:0"),20));
            sockListen.start();
            Address addr = Address (node->getNodeAddress().getIp(),sockListen.getLocalAddress().getPort());
            //If we're using UPNP, we open a port mapping
            if (tested_method == UPNP){
                Epyx::UPNP::Natpunch nat;
                //log::fatal << "nat.openMapPort(addr.getPort(),remotePort); not yet implemented" << log::endl;
                addr = nat.openMapPort(addr.getPort());
            }
            //Now, Ask to open a connection.
            GTTPacket pkt;
            pkt.method = "SEND";
            pkt.headers["Address"] = addr.toString();
            pkt.headers["Message"] = testMessage;
            this->node->send(this->remoteHost,"DIRECTCONNECTION",pkt);
            char[10] data;
            sockListen.getSocket()->recv((void *) data,10);
            if (std::string(data) == testMessage){
                pkt.method="ESTABLISHED";
                node->offerDirectConn(this->remoteHost,sockListen.getSocket());
                node->send(this->remoteHost,"DIRECTCONNECTION",pkt);
            }else{
                pkt.method="DID_NOT_WORK";
                sockListen.getSocket()->close();
                sockListen.term();
                node->send(this->remoteHost,"DIRECTCONNECTION",pkt);
                this->getMessage("DID_NOT_WORK", std::map<std::string, std::string>());
            }
                    
        }
        void OpenConnection::run(){
            if (etat == STATE_SERVER){
                GTTPacket pkt;
                pkt.method = "SEND";
                
                serverStateOpen();
            }else if (etat == STATE_CLIENT){
                
            }
        }
    } // namespace DirectConnection
} // namespace Epyx

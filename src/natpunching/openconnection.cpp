#include "openconnection.h"
#include "../core/string.h"
#include "upnp/openconnect.h"
#include "../parser/gttpacket.h"
#include "listener.h"
namespace Epyx {
    namespace DirectConnection {
        // In this protocol, the client is stupid and just obey orders
        OpenConnection::OpenConnection(N2NP::Node &node, N2NP::NodeId &remoteHost, bool client = true) 
            : client_tried(false), server_tried(false), tested_method(DIRECT), etat(STATE_SERVER), remoteHost(remoteHost), node(node)
        {
            if (client){
                etat = STATE_CLIENT;
            }
        }
        void OpenConnection::getMessage(std::string command, std::map<std::string, std::string> headers){
            if (command == "SEND" && etat == STATE_CLIENT){
                Address addr (headers["Address"]);
                socket = TCPSocket(addr);
                if (socket.send(headers["Message"].cstr(), headers["Message"].size()) != 0){
                    //Success, Do nothing and wait for N2NP confirmation
                }else{
                    //Failed
                    //Send Did Not Work Message
                    GTTPacket pkt;
                    pkt.method ="DID_NOT_WORK";
                    this->node.send(this->remoteHost,"DIRECTCONNECTION",pkt);
                    this->getMessage("DID_NOT_WORK", std::map<std::string, std::string>);
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
                node.offerDirectConn(this->remoteHost,this->socket);
/*            }else if (command == "FAILED" ){ // No Useful yet
                
              }//*/ 
            }
        }
        
        void OpenConnection::serverStateOpen(){
            //First we open a listening socket on an available port
            Listener sockListen(new TCPServer(Address("0.0.0.0:0"),20));
            sockListen.start();
            Address addr = Address (node.getNodeAddress().getIp(),sockListen.getLocalAddress().getPort());
            //If we're using UPNP, we open a port mapping
            if (tested_method == UPNP){
                Epyx::UPNP::Natpunch nat;
                addr = nat.openMapPort(addr.getPort(),remotePort);
            }
            //Now, Ask to open a connection.
            GTTPacket pkt;
            pkt.method = "SEND";
            pkt.headers["Address"] = addr.getIp()+":"+addr.getPort();
                    
        }
        void OpenConnection::run(){
            if (etat == STATE_SERVER){
                serverStateOpen();
            }else if (etat == STATE_CLIENT){
                
            }
        }
    } // namespace DirectConnection
} // namespace Epyx

#include <iostream>
#include <sstream>
#include "command.h"
#include "openconnect.h"
#include "../../core/socket.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#define SOCKET_ERROR (-1)


int main(int argc, char* argv[]){
    std::string endl ="\r\n";
    std::string address, service, path;
    unsigned short port;
    
    std::cout << "Enter Address to connect with : " ;
    std::cin >> address;
    std::cout << "Enter Port : ";
    std::cin >> port;
    std::cout << "Enter Service Name : " ;
    std::cin >> service;
    std::cout << "Enter Path to access : " ;
    std::cin >> path;
    
   std::cout << "The command is being created ..." << std::endl;
    Epyx::UPNP::Command ordre(address,port);
    ordre.setOption(Epyx::UPNP::UPNP_ACTION_GET_EXT_IP);
    ordre.setService(service);
    ordre.setPath(path);
    
    ordre.buildCommand();
    
    std::cout << "Command Built. Sending ..." << std::endl <<ordre.getOrder() << std::endl;
    try{
        ordre.send();
        std::cout << "Command sent. Waiting for Answer..." << std::endl;
        ordre.Receive();
    }catch(Epyx::ErrException e){
        std::cerr << e;
    }

    std::cout << "Received Answer of size " << ordre.getAnswer().size() <<" : "<< std::endl;
    std::cout << ordre.getAnswer() << std::endl;
    
    
    return 0;
    
}

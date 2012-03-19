#include "openconnect.h"
#include "../../net/udpsocket.h"
#include <sstream>
#include <iostream>
#include <ctime>

#define BUF_SIZE 4096
#define TIMEOUT_SEC 5

namespace Epyx {
    namespace UPNP {
        Natpunch::Natpunch() {
            this->endl = "\r\n";
            success = false;
        }

        void Natpunch::discover(){
            char data[BUF_SIZE];
            Epyx::UDPSocket sock;
            //UDPSocket is preparing to multicast on port 1900 For UPnP discover
            Address multicastAddr("239.255.255.250", 1900);
            sock.setAddress(multicastAddr);
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << endl
                    << "HOST: 239.255.255.250:1900" << endl
                    << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << endl
                    << "MAN: \"ssdp:discover\"" << endl
                    << "MX: 2" << endl << endl;
            sock.write(message.str());
            
            bool foundGoodIGD = false;
            time_t startTime = time(NULL);
            while (!foundGoodIGD && time(NULL) - startTime < TIMEOUT_SEC){
                int bytes = sock.recv(data,BUF_SIZE);
                std::string packet (data, bytes);
                std::cout << packet << std::endl;
                if (packet.find("Gateway") != std::string::npos){
                    foundGoodIGD = true;
                    //Getting Path Info
                    int begin = packet.find("LOCATION:")+9;
                    int end = packet.find("\r\n",begin);
                    std::string completePath = packet.substr(begin,end);
                    int startAddress = completePath.find("http://");
                    if (startAddress == (int) std::string::npos)
                        startAddress = 0;
                    else
                        startAddress += 7;
                    int startPath = completePath.find("/",startAddress);
                    int startPort = (completePath.find(":",startAddress) == std::string::npos ) ? (int) completePath.find(":",startAddress) : -1;
                    int port = (startPort == -1) ? 80 : Epyx::String::toInt(completePath.substr(startPort+1,startPath).c_str());
                    startPort = (startPort == -1) ? startPath : startPort;
                    
                    this->addr = Address(completePath.substr(startAddress,startPort).c_str(),port);
                    this->path = completePath.substr(startPath);
                }
            }
            if (foundGoodIGD)
                success = true;
            sock.close();
        }

        Address Natpunch::openMapPort(unsigned short localPort, unsigned short remotePort){
            discover();
            Epyx::Address addr;
            if (success){
                igd.setAddress(this->addr);
                igd.setRootDescPath(path);
                igd.getServices();
                try{
                    addr = igd.addPortMap(localPort,Epyx::UPNP::TCP,remotePort);
                    
                }
                catch(Epyx::FailException e){
                    std::cerr << e << std::endl; //replace by Logger.
                }
            }
            return addr;
        }
    }
}

#include "discovery.h"
#include <sstream>
#include "../../core/log.h"

namespace Epyx {
    namespace UPNP {
        
        Discovery::Discovery(UDPSocket &s)
            :NetSelectSocket(s)
        {
            std::string endl="\r\n";
            socket().setAddress(Address("239.255.255.250",1900));
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << endl
                    << "HOST: 239.255.255.250:1900" << endl
                    << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << endl
                    << "MAN: \"ssdp:discover\"" << endl
                    << "MX: 2" << endl << endl;
            socket().write(message.str());
        }
        
        void Discovery::eat(const char *data, long size){
            int n;
            log::debug << "Eating : "<< size << " bytes" << log::endl << data << log::endl;
            log::flush();
            htpars.eat(data, size);
            pkt = htpars.getPacket();                
            if ((pkt != NULL) && (pkt->headers["usn"]).find("InternetGateway")){
                std::string location = pkt->headers["location"];
                if ((n = location.find("http://")) != std::string::npos){
                    location = location.substr(n+7);
                }
                log::debug << location << log::endl;
                n = location.find('/');
                log::debug << location.substr(0,n) << log::endl;
                addr = Address(location.substr(0,n));
                rootPath = location.substr(n);
                htpars.reset();
                log::debug << "Address : " <<  addr << log::endl << "Path : " << rootPath << log::endl;
                log::flush();
            }
        }
        
        Address Discovery::getAddress(){
            return this->addr;
        }
        
        std::string Discovery::getPath(){
            return this->rootPath;
        }
    } // namespace UPNP
} // namespace Epyx

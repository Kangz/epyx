#include "openconnect.h"
#include "discovery.h"
#include "../../net/udpsocket.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

#define BUF_SIZE 4096
#define TIMEOUT_SEC 5

namespace Epyx
{
    namespace UPNP
    {

        Natpunch::Natpunch()
        :success(false) {
        }

        Natpunch::~Natpunch(){
            if (igd != NULL) {
                delete igd;
                igd = NULL;
            }
        }

        const SockAddress Natpunch::openMapPort(unsigned short localPort,
            unsigned short remotePort) {
            // Discovery UDP socket
            Discovery disco;
            if (!disco.queryAnswerIn(10000, &uri)) {
                log::error << "UPnP discovery failed" << log::endl;
                return SockAddress();
            }
            //If remotePort is not set, we try to find an available one.
            // Now use IGD
            log::debug << "URI : " << uri << log::endl;
            igd = new IGD(uri);
            if (!igd->getServices()) {
                log::error << "Unable to get IGD services" << log::endl;
                return SockAddress();
            }

            log::debug << "IP addr : " << igd->getExtIPAdress() << log::endl;
            //If remotePort is not set, we try to find an available one.
            if (remotePort==0){
                std::list<portMap> portMapList= igd->getListPortMap(); 
                //We shall Take a number at random between 1024 and 65536, and check this number is not taken yet.
                srand((unsigned)time(0));
                bool foundValidPort = false;
                unsigned short remotePort;
                unsigned short range = 65536-1024;
                while (!foundValidPort){
                    remotePort = (unsigned short) 1024+(rand()*range/(RAND_MAX+1.0));
                    foundValidPort = true;
                    for (std::list<portMap>::iterator it = portMapList.begin(); it != portMapList.end(); ++it )
                        if (it->nat_port == remotePort)
                            foundValidPort = false;
                }
            }

            SockAddress addr = igd->addPortMap(localPort, Epyx::UPNP::TCP, remotePort);
            return addr;
        }
    }
}

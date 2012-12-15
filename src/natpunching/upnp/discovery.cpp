#include "discovery.h"
#include <sstream>
#include "../../core/common.h"
#include "../../net/udpsocket.h"

namespace Epyx
{
    namespace UPNP
    {

        Discovery::Discovery()
        :NetQuery(new UDPSocket(SockAddress("239.255.255.250", 1900))) {
        }
        
        std::unique_ptr<IGD> Discovery::discoverIGD(const Timeout& timeout) {
            std::unique_ptr<IGD> nullresult;
            // Discovery UDP socket
            Discovery disco;
            URI uri;
            if (!disco.queryAnswerIn(timeout, &uri)) {
                log::error << "UPnP discovery failed" << log::endl;
                return nullresult;
            }
            //If remotePort is not set, we try to find an available one.
            // Now use IGD
            log::debug << "UPnP: Found IGD at URI " << uri << log::endl;
            std::unique_ptr<IGD> igd(new IGD(uri));
            if (!igd->getServices()) {
                log::error << "UPnP: Unable to get IGD services" << log::endl;
                return nullresult;
            }
            return std::move(igd);
        }

        bool Discovery::query() {
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << String::crlf
                << "HOST: 239.255.255.250:1900" << String::crlf
                << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << String::crlf
                << "MAN: \"ssdp:discover\"" << String::crlf
                << "MX: 2" << String::crlf
                << String::crlf;
            return socket()->write(message.str());
        }

        std::unique_ptr<URI> Discovery::eat(const byte_str& data) {
            std::unique_ptr<URI> uri;

            // Eat data with HTTP parser
            htpars.eat(data);
            std::unique_ptr<GTTPacket> pkt = htpars.getPacket();
            if (!pkt) {
                // Incomplete packet, there may be an error
                std::string error;
                if (htpars.getError(error)) {
                    log::error << "HTTP Parser error during discovery:\n"
                        << error << log::endl;
                }
                return uri;
            }

            // Here, pkt is an HTTP packet. Filter in Internet Gateways
            if (pkt->headers["usn"].find("InternetGateway")) {
                // Return location
                uri.reset(new URI(pkt->headers["location"]));
            }
            return uri;
        }
    }
}

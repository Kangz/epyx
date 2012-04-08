#include "dht-packet.h"
#include <exception>
#include <map>
#include <sstream>
#include <cstring>
#include "../core/string.h"

namespace Epyx
{
namespace DHT
{
    DHTPacket::DHTPacket(GTTPacket& pkt){
        //TODO: what should we do if a packet is not valid ?
        if(pkt.headers.count("From")==0){
            throw;
        }
        std::istringstream ss(pkt.headers["From"]);
        ss >> from;

        if(pkt.method == "PING"){
            method = M_PING;

        }else if(pkt.method == "PONG"){
            method = M_PONG;

        }else if(pkt.method == "STORE"){
            method = M_STORE;
            if(pkt.headers.count("ConnectionId") == 0 || pkt.headers.count("Key") == 0 || !pkt.body){
                throw;
            }

            value = pkt.acquireBody();
            valueSize = pkt.size;
            connectionId = String::toInt(pkt.headers["ConnectionId"]);

            std::stringstream sst(pkt.headers["Key"]);
            sst >> key;


        }else if(pkt.method == "STORED"){
            method = M_STORED;
            if(pkt.headers.count("ConnectionId") == 0 || pkt.headers.count("Status") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["ConnectionId"]);
            status = String::toInt(pkt.headers["Status"]);

        }else if(pkt.method == "GET"){
            method = M_GET;
            if(pkt.headers.count("ConnectionId") == 0 || pkt.headers.count("Key") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["ConnectionId"]);

            std::stringstream sst(pkt.headers["Key"]);
            sst >> key;


        }else if(pkt.method == "GOT"){
            method = M_GOT;
            if(pkt.headers.count("ConnectionId") == 0 || !pkt.body){
                throw;
            }

            value = new char[pkt.size];
            valueSize = pkt.size;
            connectionId = String::toInt(pkt.headers["ConnectionId"]);

        }else if(pkt.method == "FIND"){
            method = M_FIND;
            if(pkt.headers.count("ConnectionId") == 0 || pkt.headers.count("Count") == 0 ||
               pkt.headers.count("Target") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["ConnectionId"]);
            count = String::toInt(pkt.headers["Count"]);

            std::stringstream sst(pkt.headers["Target"]);
            sst >> idToFind;

        }else if(pkt.method == "FOUND"){
            method = M_FOUND;
            if(pkt.headers.count("ConnectionId") == 0 || pkt.headers.count("Count") == 0i || !pkt.body){
                throw;
            }
            connectionId = String::toInt(pkt.headers["ConnectionId"]);

            if(pkt.size != count*49){ //HACK: this is the size of the hexadecimal representation of an Id
                throw;
            }

            Id id;
            std::string data(pkt.body, pkt.size);
            std::istringstream ssdata(data);
            for(int i=0; i<count; i++){
                ssdata >> id;
                foundIds.push_back(id);
            }

        }else{
            throw;
        }
    }

    GTTPacket* DHTPacket::toGTTPacket(){
        GTTPacket* pkt = new GTTPacket();
        pkt->protocol = "DHT";

        std::ostringstream oss;
        oss << from;
        pkt->headers["From"] = oss.str();
        oss.str("");

        switch(method){
            case M_PING:
                pkt->method = "PING";
                break;

            case M_PONG:
                pkt->method = "PONG";
                break;

            case M_STORE:
                pkt->method = "STORE";

                oss << idToFind;
                pkt->headers["Key"] = oss.str();
                oss.str("");

                pkt->headers["ConnectionId"] = String::fromInt(connectionId);

                pkt->body = value;
                value = NULL; //To avoid to free it after we give away the GTTPacket
                pkt->size = valueSize;
                break;

            case M_STORED:
                pkt->method = "STORED";
                pkt->headers["ConnectionId"] = String::fromInt(connectionId);
                pkt->headers["Status"] = String::fromInt(status);

            case M_GET:
                pkt->method = "GET";

                oss << key;
                pkt->headers["Key"] = oss.str();
                oss.str("");

                pkt->headers["ConnectionId"] = String::fromInt(connectionId);
                break;

            case M_GOT:
                pkt->method = "GOT";

                pkt->headers["ConnectionId"] = String::fromInt(connectionId);
                pkt->headers["Status"] = String::fromInt(status);

                pkt->body = value;
                value = NULL; //To avoid to free it after we give away the GTTPacket
                pkt->size = valueSize;
                break;

            case M_FIND:
                pkt->method = "FIND";

                pkt->headers["ConnectionId"] = String::fromInt(connectionId);
                pkt->headers["Count"] = String::fromInt(count);

                oss << idToFind;
                pkt->headers["Target"] = oss.str();
                oss.str("");
                break;

            case M_FOUND:
                pkt->method = "FOUND";

                pkt->headers["ConnectionId"] = String::fromInt(connectionId);
                pkt->headers["Status"] = String::fromInt(status);
                pkt->headers["Count"] = String::fromInt(foundIds.size());

                for(std::list<Id>::iterator i=foundIds.begin(); i != foundIds.end(); ++i){
                    oss << *i;
                }
                std::string data = oss.str();
                pkt->body = new char[data.size()];
                memcpy(pkt->body, data.c_str(), data.size());
                oss.str("");
                break;
        }

        return pkt;
    }
}
}

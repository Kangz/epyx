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
    DHTPacket::DHTPacket(){}
    DHTPacket::~DHTPacket(){}

    DHTPacket::DHTPacket(GTTPacket& pkt){
        //TODO: what should we do if a packet is not valid ?
        if(pkt.headers.count("from")==0){
            throw;
        }
        std::istringstream ss(pkt.headers["from"]);
        ss >> from;

        if(pkt.method == "PING"){
            method = M_PING;

        }else if(pkt.method == "PONG"){
            method = M_PONG;

        }else if(pkt.method == "STORE"){
            method = M_STORE;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("key") == 0 || !pkt.body){
                throw;
            }

            value = std::string(pkt.acquireBody(), pkt.size);
            connectionId = String::toInt(pkt.headers["connectionid"]);

            key = pkt.headers["key"];

        }else if(pkt.method == "STORED"){
            method = M_STORED;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("status") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["connectionid"]);
            status = String::toInt(pkt.headers["status"]);

        }else if(pkt.method == "GET"){
            method = M_GET;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("key") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["connectionid"]);

            key = pkt.headers["Key"];


        }else if(pkt.method == "GOT"){
            method = M_GOT;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("status") == 0 || !pkt.body){
                throw;
            }

            value = std::string(pkt.body, pkt.size);
            connectionId = String::toInt(pkt.headers["connectionid"]);

        }else if(pkt.method == "FIND"){
            method = M_FIND;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("count") == 0 ||
               pkt.headers.count("target") == 0){
                throw;
            }

            connectionId = String::toInt(pkt.headers["connectionid"]);
            count = String::toInt(pkt.headers["count"]);

            std::stringstream sst(pkt.headers["target"]);
            sst >> idToFind;

        }else if(pkt.method == "FOUND"){
            method = M_FOUND;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("count") == 0 ||
               pkt.headers.count("status") == 0 || !pkt.body){
                throw;
            }
            connectionId = String::toInt(pkt.headers["connectionid"]);
            count = String::toInt(pkt.headers["count"]);
            status = String::toInt(pkt.headers["status"]);

            if(pkt.size != count*49){ //HACK: this is the size of the hexadecimal representation of an Id
                return;//throw;
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
        pkt->headers["from"] = oss.str();
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

                pkt->headers["key"] = key;

                pkt->headers["connectionid"] = String::fromInt(connectionId);

                pkt->body = String::toNewChar(value);
                pkt->size = value.size();
                break;

            case M_STORED:
                pkt->method = "STORED";
                pkt->headers["connectionid"] = String::fromInt(connectionId);
                pkt->headers["status"] = String::fromInt(status);
                break;

            case M_GET:
                pkt->method = "GET";

                pkt->headers["key"] = key;

                pkt->headers["connectionid"] = String::fromInt(connectionId);
                break;

            case M_GOT:
                pkt->method = "GOT";

                pkt->headers["connectionid"] = String::fromInt(connectionId);
                pkt->headers["status"] = String::fromInt(status);

                pkt->body = String::toNewChar(value);
                pkt->size = value.size();
                break;

            case M_FIND:
                pkt->method = "FIND";

                pkt->headers["connectionid"] = String::fromInt(connectionId);
                pkt->headers["count"] = String::fromInt(count);

                oss << idToFind;
                pkt->headers["target"] = oss.str();
                oss.str("");
                break;

            case M_FOUND:
                pkt->method = "FOUND";

                pkt->headers["connectionid"] = String::fromInt(connectionId);
                pkt->headers["status"] = String::fromInt(status);
                pkt->headers["count"] = String::fromInt(foundIds.size());

                for(std::list<Id>::iterator i=foundIds.begin(); i != foundIds.end(); ++i){
                    oss << *i;
                }
                std::string data = oss.str();
                pkt->body = new char[data.size()];
                memcpy(pkt->body, data.c_str(), data.size());
                pkt->size = data.size();
                oss.str("");
                break;
        }

        return pkt;
    }
}
}

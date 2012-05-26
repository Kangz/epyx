/**
 * @file dht/packet.cpp
 * @brief the implementation of the DHT Packet
 */

#include "packet.h"
#include <exception>
#include <map>
#include <sstream>
#include <cstring>
#include "../core/string.h"
#include "../core/common.h"

namespace Epyx
{
namespace DHT
{
    Packet::Packet(){}
    Packet::~Packet(){}

    Packet::Packet(GTTPacket& pkt){
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

            value = std::string(pkt.body, pkt.size);
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

            key = pkt.headers["key"];


        }else if(pkt.method == "GOT"){
            method = M_GOT;
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("status") == 0 || !pkt.body){
                throw;
            }

            connectionId = String::toInt(pkt.headers["connectionid"]);
            this->getValueFromGTT(pkt);

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
            this->getValueFromGTT(pkt);
            connectionId = String::toInt(pkt.headers["connectionid"]);
            count = String::toInt(pkt.headers["count"]);

            Peer p;
            std::istringstream ssdata(value);
            for(int i=0; i<count; i++){
                p.unserialize(ssdata);
                foundPeers.push_back(p);
            }

        }else{
            throw;
        }
    }

    GTTPacket* Packet::toGTTPacket(){
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
                this->setValueForGTT(pkt);
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
                pkt->headers["count"] = String::fromInt(foundPeers.size());

                for(std::vector<Peer>::iterator i=foundPeers.begin(); i != foundPeers.end(); ++i){
                    (*i).serialize(oss);
                }
                value = oss.str();
                this->setValueForGTT(pkt);
                break;
        }

        return pkt;
    }

    void Packet::getValueFromGTT(GTTPacket& pkt){
        status = String::toInt(pkt.headers["status"]);
        if(status == 0){
            value = std::string(pkt.body, pkt.size);
        }else{
            value = std::string("");
        }
    }

    void Packet::setValueForGTT(GTTPacket* pkt){
        pkt->headers["status"] = String::fromInt(status);
        if (status == 0) {
            pkt->body = String::toNewChar(value);
            pkt->size = value.size();
        } else {
            pkt->body = NULL;
            pkt->size = 0;
        }
    }

}
}

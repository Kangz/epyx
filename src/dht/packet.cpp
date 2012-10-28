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
    Packet::Packet() {
        foundPeers = NULL;
    }
    Packet::~Packet() {
        if(foundPeers != NULL) {
            delete foundPeers;
        }
    }

    Packet::Packet(GTTPacket& pkt) {
        //TODO: what should we do if a packet is not valid ?
        foundPeers = NULL;

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
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("key") == 0){
                throw;
            }

            this->getValueFromGTT(pkt, false);
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
            if(pkt.headers.count("connectionid") == 0 || pkt.headers.count("status") == 0){
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
               pkt.headers.count("status") == 0 || pkt.body.empty()){
                throw;
            }
            this->getValueFromGTT(pkt);
            connectionId = String::toInt(pkt.headers["connectionid"]);
            count = String::toInt(pkt.headers["count"]);

            foundPeers = new std::vector<Peer::SPtr>();
            std::istringstream ssdata(value);
            for(int i=0; i<count; i++){
                std::shared_ptr<Peer> p(new Peer);
                p->unserialize(ssdata);
                foundPeers->push_back(p);
            }

        }else{
            throw;
        }
    }

    GTTPacket* Packet::toGTTPacket() {
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

                this->setValueForGTT(pkt, false);
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
                pkt->headers["count"] = String::fromInt(foundPeers->size());

                for(auto i=foundPeers->begin(); i != foundPeers->end(); ++i){
                    (*i)->serialize(oss);
                }

                value = oss.str();
                this->setValueForGTT(pkt);
                break;
        }

        return pkt;
    }

    void Packet::getValueFromGTT(GTTPacket& pkt, bool useStatus) {
        if(useStatus) {
            status = String::toInt(pkt.headers["status"]);
        }
        if(! useStatus || status == 0){
            // TO SEE
            value = std::string((const char*)(pkt.body.c_str()), pkt.body.size());
        }else{
            value = std::string("");
        }
    }

    void Packet::setValueForGTT(GTTPacket* pkt, bool useStatus) {
        if(useStatus) {
            pkt->headers["status"] = String::fromInt(status);
        }
        if (! useStatus || status == 0) {
            pkt->body = string2bytes_c(value);
        } else {
            pkt->body = byte_str();
        }
    }

}
}

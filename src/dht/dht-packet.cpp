#include	"dht-packet.h"
#include	<exception>
#include	<map>
#include	<sstream>
#include <cstring>

namespace	Epyx
{
namespace	DHT{

		int	str2int(const	std::string&	str){
			std::istringstream	iss(str);
			int	nombre;
			iss	>>	nombre;
			return	nombre;
		}

		std::string	int2str(const	int&	n){
			std::stringstream	out;
			out	<<	n;
			return	out.str();
		}
		
		char* str2char(const std::string& str){
			 size_t size = str.size() + 1;
			 char* buffer = new char[size];
			 strncpy( buffer, str.c_str(), size );
			 return buffer;
		}
   
		
		DHTPacket::DHTPacket(GTTPacket&	pkt)
		{
			//	TO	DO	?	check	the	structure	of	key/value	with	regex.
			method	=	pkt.method;
			if(method=="PING"){
				
			}

			else if(method=="PONG"){
				
			}

			else if(method=="STORE"){
						if(pkt.headers.count("ID")==0){
							throw;
						}
						if(!pkt.body){
							throw;
						}
						std::string	str;
						str=*(pkt.body);
						size_t	found	=	str.find_first_of(":");
						if(found==std::string::npos){
							throw;
						}
						idConnection	=	str2int(pkt.headers["ID"]);
						key	=	str.substr(0,found);
						value	=	str.substr(found+1);
						
				}

			else if(method=="STORED"){
					if(pkt.headers.count("ID")==0){
						throw;
					}
					if(pkt.headers.count("STATUS")==0){
						throw;
					}
					idConnection	=	str2int(pkt.headers["ID"]);
					status	=	str2int(pkt.headers["STATUS"]);
					

			}

			else if(method=="GET"){
					if(pkt.headers.count("ID")==0){
						throw;
					}
					if(!pkt.body){
						throw;
					}
					idConnection	=	str2int(pkt.headers["ID"]);
					key	=	*(pkt.body);
					
			}

			else if(method=="GOT"){
					if(pkt.headers.count("ID")==0){
						throw;
					}
					if(pkt.headers.count("STATUS")==0){
						throw;
					}
					idConnection	=	str2int(pkt.headers["ID"]);
					status	=	str2int(pkt.headers["STATUS"]);
					value	=	*(pkt.body);
					
			}

			else if(method=="FIND"){
					if(pkt.headers.count("ID")==0){
						throw;
					}
					if(pkt.headers.count("N")==0){
						throw;
					}
					if(!pkt.body){
						throw;
					}
					idConnection	=	str2int(pkt.headers["ID"]);
					n	=	str2int(pkt.headers["N"]);
					std::string str;
					str=*(pkt.body);
					std::istringstream	iss(str);
					iss	>>	idToFind;
			}

			else if(method=="LIST"){
					if(pkt.headers.count("ID")==0){
						throw;
					}
					if(pkt.headers.count("N")==0){
						throw;
					}
					if(!pkt.body){
						throw;
					}
					idConnection	=	str2int(pkt.headers["ID"]);
					n	=	str2int(pkt.headers["N"]);
					Id	id;
					std::string str;
					str=*(pkt.body);
					std::istringstream	iss(str);
					for(int	i=0;i<n;i++){
						iss	>>	id;
						idFound.push_back(id);
					}
			}

			else{
					throw;
			}
		}

		GTTPacket	DHTPacket::toGTTPacket(){
			GTTPacket	pkt;
			pkt.protocol="DHT";
			pkt.method	=	method;
			if(method=="PING"){
				
			}

			else if(method=="PONG"){
				
			}

			else if(method=="STORE"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				std::string str(key+":"+value);
				pkt.body=str2char(str);
				pkt.size	=	str.size();
				
			}

			else if(method=="STORED"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				pkt.headers.insert(std::pair<std::string,std::string>("STATUS",int2str(status)));
				
			}

			else if(method=="GET"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				pkt.body=str2char(key);
				pkt.size	=	key.size();
				
			}

			else if(method=="GOT"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				pkt.headers.insert(std::pair<std::string,std::string>("STATUS",int2str(status)));
				pkt.body=str2char(value);
				pkt.size	=	value.size();
				
			}

			else if(method=="FIND"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				pkt.headers.insert(std::pair<std::string,std::string>("N",int2str(n)));
				std::stringstream	out;
				out	<<	idToFind;
				std::string str(out.str());
				pkt.body=str2char(str);
				pkt.size	=	str.size();
				
			}

			else if(method=="LIST"){
				pkt.headers.insert(std::pair<std::string,std::string>("ID",int2str(idConnection)));
				pkt.headers.insert(std::pair<std::string,std::string>("N",int2str(n)));
				pkt.headers.insert(std::pair<std::string,std::string>("STATUS",int2str(status)));
				std::vector<Id>::iterator	id;
				std::stringstream	out;
				std::string str("");
				for(	id=idFound.begin();	id!=idFound.end();	id++	){
					out	<<	*id;
					str+=out.str();
				}
				pkt.size	=	str.size();
				pkt.body=str2char(str);
				
			}

		}
}
}

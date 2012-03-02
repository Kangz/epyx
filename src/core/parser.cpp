#include"exception.h"
#include"parser.h"
#include<fstream>
#include<iostream>
using std::cout;
using std::ifstream;
namespace Epyx
{
	void Msg::push_back(Token tk){
		this->msg.push_back(tk);
	}
	string Msg::toString(){
		string ret="";
		for (int i=0;i< msg.size();i++)
			ret+=msg[i].str.c_str();
		return ret;
	}
	void parser::init(){
		cur.str="";
		cur.type=start;
		flag=start;
		hasBody=false;
		bodyCount=-1;
		Header.clear();
		
	}
	parser::parser(){
		init();
		size=0;
		remained=NULL;
	}
	parser::~parser(){
		if(remained==NULL)
			delete[] remained;
	}
	int parser::stringToint(const char *str){
		const char * tmp=str;
		while(*tmp){
			if(!Character::isDigit(*tmp))
				throw FailException("Parser", "Not a valid number for the value of contente-length");
			tmp++;
		}
		return atoi(str);
	}
	void parser::eat(char * str_,int size_){
		if(size_>0){
			char * tmp=new char[size+size_+1];
			if(size){
			strcpy(tmp,remained);
			delete [] remained;
			}
			remained=tmp;
		
		memcpy(remained+size,str_,sizeof(char)*(size_+1));
		size+=size_;
		}
		else{
			throw FailException("Parser","Nothing to eat ><");
		}
	}
	string parser::getPacket(){
		char * peek=parser::remained;
		while(size){
			switch(cur.type){
			case start:
				if(Character::isCapital(*peek)){
					cur.str=*peek;
					cur.type=protocol_name;
					flag=other;
				}
				else
					throw FailException("Parser", "protocol_name should begin with capital letters");
   
				break;
			case protocol_name:
				if(Character::isCapitalDigit_(*peek)){
					cur.str+=*peek;
				}
				else if(*peek==' '){
					cur.str+=*peek;
					Header.push_back(cur);
					cur.str="";
					cur.type=method;
					flag=start;
				}
				else
					throw FailException("Parser","protocolname should continue with [A-Z0-9]* or end with a space");
				break;
			case method:
				if(flag==start){
					if(Character::isCapital(*peek)){
						cur.str=*peek;
						flag= other;
					}
					else 
						throw FailException("Parser", "method should begin with capital letters");

				}
				else if(Character::isCapitalDigit_(*peek)){
					cur.str+=*peek;

				}
				else if(Character::isCR(*peek)){
					cur.str+=*peek;
					cur.type= method_line;
				}
				else{
					throw FailException("Parser","method should continue with [A-Z0-9_] or end with CRLF");
				}
				break;
			case  method_line:
				if(Character::isLF(*peek)){
					cur.str+=*peek;
					Header.push_back(cur);
					cur.str="";
					cur.type= flag_name;
					flag= start;
				}
				else
					throw FailException("Parser","newline should be CRLF");
			    break;
			case  flag_name:
				if(flag== start){
					if(Character::isLetter(*peek)){
						cur.str=*peek;
						flag= other;
					}
					else if(Character::isCR(*peek)){
						cur.str=*peek;
						cur.type= newline;
						flag= other;
						
					}
					else
						throw FailException("Parser","flag_name should begin with [a-zA-Z] or we should add newline to end the header");

				}
				else if(Character::isLetterDigit_hyphen(*peek)){
					cur.str+=*peek;
				}
				else if(*peek==':'){
					if(cur.str=="content-length"){
						if(hasBody)
							throw FailException("Parser","content-length flag has already appeared");
						else
						hasBody=true;
						
					}
					cur.str+=*peek;
					cur.type= flag_indicator;
				}
				else
					throw FailException("Parser","flag_name should continue with [a-zA-Z0-9_-] or end with :");
				break;
			case  flag_indicator:
				if(*peek==' '){
					cur.str+=*peek;
					Header.push_back(cur);
					cur.str="";
					cur.type= flag_value;
				}
				else
					throw FailException("Parser","Expected un space right after : to seperate the flag_name and value");
				break;
			case  newline:
				if(Character::isLF(*peek)){
					cur.str+=*peek;
					Header.push_back(cur);
					if(hasBody){
						cur.str="";
						cur.type= body;
						hasBody=false;
					}
					else{
						
						string packet=Header.toString();
						init();
						peek++;
						size--;
						if(size){
							char *tmp=new char[size+1];
							strcpy(tmp,peek);
							delete []remained;
							remained=tmp;
						}
						else{
							delete [] remained;
							remained=NULL;
						}
						return packet;
						
					}
				}
				else 
					throw FailException("Parser","newline should end with CRLF");
				break;
			case  flag_value:
				if(Character::isPrintable(*peek)){
					cur.str+=*peek;
				}
				else if(Character::isCR(*peek)){
					if(hasBody){
						bodyCount=stringToint(cur.str.c_str());
					if(bodyCount==INT_MAX)
						throw FailException("Parser","body size too big");
					if(bodyCount<=0)
						throw FailException("Parser","not valid body size, body size should be a positive integer");
					}
					cur.str+=*peek;
					cur.type= flag_line;
				}
				else
					throw FailException("Parser","flag_value should consist of printable characters or just end with CRLF");
				break;
			case  flag_line:
				if(Character::isLF(*peek)){
					cur.str+=*peek;
					Header.push_back(cur);
					cur.str="";
					cur.type= flag_name;
					flag= start;
				}
				else
					throw FailException("Parser","newline should end with CRLF");
				break;
			case  body:
				if(bodyCount){
					cur.str+=*peek;
				}
				bodyCount--;
				
				if(bodyCount==0){
					Header.push_back(cur);
					string packet=Header.toString();
					init();
					
					
					peek++;
					size--;
					if(size){
							char *tmp=new char[size+1];
							strcpy(tmp,peek);
							delete []remained;
							remained=tmp;
						}
					else{
						delete [] remained;
						remained=NULL;
					}
					return packet;

				}
				break;
			default:
				throw FailException("Parser","no corresponding etat");
			}
			size--;
			peek++;
			}
		
		if(remained)
		delete []remained;
		return "";
		}
	

}


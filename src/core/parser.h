#ifndef EPYX_PARSER_H
#define EPYX_PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string>
#include <iostream>
#include <cstring>
#include<vector>
using std::string;
using std::vector;
namespace Epyx
{
enum tokenType{start,protocol_name,method,method_line,flag_name,flag_indicator,flag_value,flag_line,newline,body,other};
class Character{
public:
	static bool isCapital(char c){
		return c>='A'&&c<='Z';
	}
	static bool isDigit(char c){
		return c>='1'&&c<='9';
	}
	static bool isCapitalDigit_(char c){
		return isCapital(c)||isDigit(c)||c=='_';
	}
	static bool isLetter(char c){
		return isCapital(c)||(c>='a'&&c<='z');
	}
	static bool isLetterDigit_hyphen(char c){
		return isLetter(c)||isDigit(c)||c=='_'||c=='-';
	}
	static bool isPrintable(char c){
		return c>31&&c<127;
	}
	static bool isCR(char c){
		return c==13;
	}
	static bool isLF(char c){
		return c==10;
	}
};
class Token{
public:
	string str;
	tokenType type;
};
class Msg{

private:
	vector<Token> msg;
public:
	void push_back(Token tk);
	string toString();
	void clear(){
		msg.clear();
	}

};
class parser{
private:
	 Msg Header;
	 char * remained;
	 int size;
	 bool hasBody;
	 int bodyCount;
	 Token cur;
	 tokenType flag;
	 void init();
public:
	parser();
	~parser();
	int stringToint(const char * str);
	void eat(char * str_, int size_);
	string getPacket();

};}
#endif /*the classes used in parser*/
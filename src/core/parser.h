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

    struct tokenType{int t;};
    struct Token{int t;};
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
    };

}
#endif /*the classes used in parser*/
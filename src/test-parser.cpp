#include"core/exception.h"
#include"core/parser.h"
#include<fstream>
#include<iostream>

using std::ifstream;
using std::ios_base;
using std::ios;
using std::cout;
using namespace Epyx;
const int MAXLENGTH=10000;
int getFileLength(ifstream * ifs){
	if(ifs){
		ifs->seekg(0,ios::end);
		int n=ifs->tellg();
		ifs->seekg(ios::beg);
		return n;
	}
	else 
		throw Epyx::FailException("testParser","No such file exists!");
}
	int main(){
		char *str,*name;
		try{
		Epyx::parser test;
		ifstream filenames("msg.txt");
		if(!filenames)
			throw Epyx::FailException("testParser","please provide \"msg.txt\" to indicate all filename of messages");
		int length=getFileLength(&filenames);
		name=new char[length+1];
		while(filenames.getline(name,length)&&(name[0])){
		//	filenames.getline(name,length);
		ifstream input(name,ios_base::in|ios_base::binary);
		if(!input){
			string errmsg="No such msg File: ";
			errmsg.append(name);
			throw Epyx::FailException("testParser",errmsg.c_str());
		}
		int n=getFileLength(&input);
		str=new char[n+1];
		input.read(str,n);
		str[n]='\0';
		input.close();
		test.eat(str,n);
		string pac;
		while((pac=test.getPacket())!=""){
			cout<<"here comes one packet:\n"<<pac;
		}
		delete[] str;
		}
		delete []name;
		filenames.close();
return 0;
		}
		catch(Epyx::Exception e){
			std::cerr<<e<<std::endl;
			delete[] str;
			delete []name;
return 0;
		//	exit(0);

		}
	}


#include "api.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#define LightGreen "\033[01;33m"
#define LightBlue "\033[01;34m"
#define Restore "\033[0m"
class Demo{
public:
	static std::string msg;
	static std::string historique;
	static std::string pseudo;
	static Epyx::N2NP::Node *node;
	static Epyx::N2NP::NodeId nodeid;

	static void run();
	static void affichage();
};

std::string Demo::msg("");
std::string Demo::historique("");
std::string Demo::pseudo("");
Epyx::N2NP::Node* Demo::node =NULL;
Epyx::N2NP::NodeId Demo::nodeid("local@127.0.0.1:4242");

void Demo::affichage(){
	system("clear");
	std::cout << historique;
	std::cout << "<" << pseudo << "> : " << msg;
	fflush(stdout);
}

void Demo::run(){
	struct termios tios;
	tcgetattr(STDIN_FILENO, &tios);
    	//tcflag_t old_c_lflag = tios.c_lflag;
    	tios.c_lflag &= ~ICANON;
    	tcsetattr(STDIN_FILENO, TCSANOW, &tios);
	unsigned char c;

	while(true){
		read(STDIN_FILENO, &c, 1);
		msg.append(1,c);
		if(c=='\n'){
			node->send(nodeid, "DISPLAY", msg.c_str(), msg.length() + 1);
			historique.append(LightBlue).append(std::string("<").append(pseudo).append(std::string("> : ")).append(msg)).append(Restore);
			msg = std::string("");
		}
		affichage();
        }
}



class Displayer :public Epyx::N2NP::Module
{
public:
    std::string sentence;
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
};

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {

    Demo::historique.append(LightGreen).append(sentence).append(data).append(Restore);
	Demo::affichage();
}

int main(int argc, char **argv) {
	struct termios tios;


    
    Epyx::API epyx;
    try {
        std::string pseudo_ext;
        
        epyx.setNetWorkers(50);
        
        Epyx::Address relayAddr(argv[1]);
        
        Epyx::N2NP::Node *node = new Epyx::N2NP::Node(relayAddr);

	Demo::node = node;
        
        std::cout << "Entrez votre pseudo : ";
        
        std::cin >> Demo::pseudo;
        
        std::cout<< "Entrez le pseudo que vous voulez contacter : ";
        
        std::cin >> pseudo_ext;
        
        std::string name;
        
        std::cin >> name;
        
        Displayer displayModule;
        
        displayModule.sentence = std::string("<").append(pseudo_ext).append("> : ");
        
        node->addModule("DISPLAY", &displayModule);
        
        epyx.addNode(node);
       
       	tcgetattr(STDIN_FILENO, &tios);
    	//tcflag_t old_c_lflag = tios.c_lflag;
    	tios.c_lflag &= ~ICANON;
    	tcsetattr(STDIN_FILENO, TCSANOW, &tios); 
        system("clear");
	
        
        Epyx::N2NP::NodeId n(name,relayAddr);
	Demo::nodeid = n;
	Demo::affichage();
        Demo::run();
        
    } catch (Epyx::Exception e) {
       std::cout << e << std::endl;
    }
    return 0;
}

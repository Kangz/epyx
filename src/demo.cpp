
#include "api.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include <strings.h>
class Displayer :public Epyx::N2NP::Module
{
public:
    std::string pseudo;
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
};

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    printf("\033[01;34m<%s> : %s\033[0m\n",pseudo.c_str(),data);
    
}

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        std::string pseudo_ext;
        std::string pseudo;
        
        epyx.setNetWorkers(50);
        
        Epyx::Address relayAddr(argv[1]);
        
        Epyx::N2NP::Node *node = new Epyx::N2NP::Node(relayAddr);
        
        std::cout << "Entrez votre pseudo : ";
        
    
        getline(std::cin,pseudo,'\n');
        
        std::cout<< "Entrez le pseudo que vous voulez contacter : ";
        
   
         getline(std::cin,pseudo_ext,'\n');
        std::string name;
        

         getline(std::cin,name,'\n');
       
        Displayer displayModule;
        
        displayModule.pseudo = pseudo_ext;
        
        node->addModule("DISPLAY", &displayModule);
        
        epyx.addNode(node);
       std::cin.clear();
        std::string msg;
      
        Epyx::N2NP::NodeId nodeid(name,relayAddr);
        while(true){
           
            getline(std::cin,msg,'\n');
           /* for(int t=0;t<msg.length();t++){
                printf("\b \b");
            }*/
            printf("\033[01;33m<me> : %s\033[0m\n",msg.c_str());
        //   std::cout << "<" << pseudo << "> : "<< msg <<std::endl;
           node->send(nodeid, "DISPLAY", msg.c_str(), msg.length() + 1);
        }
        
    } catch (Epyx::Exception e) {
       std::cout << e << std::endl;
    }
    return 0;
}

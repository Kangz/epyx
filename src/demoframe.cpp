
#include<stdio.h> 
#include <stdlib.h>
#include<iostream>

using std::cout;
using std::cin;
using std::endl;

bool getkey(char * password, int size){
    char c;  
    int i=0;  
  /*  while (true)  
    {  
        if(i>=size){
            cout<<"password too long!";
            //break;
            return false;
        }
        c=getch();  
        if (c=='\r')  
        {  
            break;  
        }  
        else if (c=='\b')  
        {  
            if (i>0)  
              i--;  
            putchar('\b');  
            putchar(' ');  
            putchar('\b');  
        }  
        else  
        {  
            password[i++] = c;  
            putchar('*');  
        }  
    }  */
    password[i] = '\0';  
//    cout<<"\nyour password:"<<password<<endl;  
  
    return true;
}  
void welcome(void)
{
    
    cout<<"                                                                           "<<endl;
    cout<<"                                                                           "<<endl;
    
    cout<<"       Welcome to ";
    
    cout<<"EPYX";
    
    cout<<" !                                              "<<endl;
    
    cout<<"                                                                        "<<endl;
    cout<<"                                                                        "<<endl;
    cout<<"       A Fancy Free Net Environment                  ┊ ┊  ┊ ┊┊ ┊ ┊  "<<endl;
    cout<<"                                                    ┊ ┊ ┊ ┊┊ ┆ ┊ "<<endl;
    cout<<"                                                  ☆ ┊ ☆ ┊☆ ┆ ┊  "<<endl;
    cout<<"                                                  ☆   ☆  ┆ ┊     "<<endl; 
    cout<<"       │★☆   ╮。。...                                    ☆ ┊  "<<endl;
    cout<<"      ╭╯☆★☆★╭╯。。...                                     ┊  "<<endl;
    cout<<"      ╰╮★☆★╭╯。。。...                                。    ┊  "<<endl;
    cout<<"       │☆╭—╯                                                    ☆  "<<endl;
    cout<<"      ╭╯╭╯                                                          "<<endl;
    cout<<"     ╔╝★╚╗                                                         "<<endl; 
    cout<<"     ║★☆★║╔═══╗ ╔═══╗ ╔═══╗ ╔═══╗           "<<endl; 
    cout<<"     ║☆★☆║║ E ║ ║ P ║ ║ Y ║ ║ X ║           "<<endl;
    cout<<"    ◢◎══◎╝╚◎═◎╝═╚◎═◎╝═╚◎═◎╝═╚◎═◎╝           "<<endl;
    cout<<"     Please Enter anykey to Continue  ^_^                          ━━*━━━*━━>>  "<<endl;
    cin.get();
   
        cout << "\033c";
}
void menu(void)
{
    int namesize=512;
    int keysize=512;
    
    cout<<"Epyx ";

    cout<<"an easy tool for a unlimited world"<<endl;
    cout<<endl;
    cout<<"\nplease input your username: ";
    char * username=new char[namesize];
    
    cin.getline(username,namesize);
    
    cout<<"\nplease input your key: ";
    char * key=new char[keysize];
    
    getkey(key, keysize);
    cout<<endl;
    
    delete []username;
    delete []key;

    
}
void endscene()
{
    
    cout<<"                                                                         "<<endl;
    cout<<"                                                                         "<<endl;
    cout<<"                  Thank you for your experience in Eypx !  ^_^          "<<endl;
    cout<<"                                                                      "<<endl;
    cout<<"      ┏━  <<*━━*━━*━━━━━━*━━**********━*━**            "<<endl;
    cout<<"      ┃e                                                 ┗┓           "<<endl;
    cout<<"                                         ┗┓          "<<endl;
    cout<<"        p                                                 ┗┓           "<<endl; 
    cout<<"                                                               *┃     "<<endl;
    cout<<"        y    We hope you have great fun with epyx       ┃*      "<<endl; 
    cout<<"                                                          ┃             "<<endl; 
    cout<<"        x                                                     *┃      "<<endl;
    cout<<"          ┃ A bientot! ^_^ !   ( please enter anykey to quit )  *     "<<endl;
    cout<<"        ━┛                                       ━━*━━━*━━>>"<<endl;
   
    cin.get();
}
void Exit(void)
{
    
    cout<<"Are you sure to exit our frendily program ?:Y/N"<<endl;
    
    char ch;
    
    cin>>ch;

    if(ch=='Y'||ch=='y')
    {
        system("cls");
        endscene();
        
    }
    else if(ch=='N')
    {
        system("cls");
        // go back to uplayer system;
    }
    else{
        cout<<"Failure please input Y/y or N/n"<<endl;
        Exit();
    }
    return;
}

int main(){
    welcome();
    menu();
    Exit();
}


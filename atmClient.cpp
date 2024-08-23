// atm client program 
#include "includes.h"


int main()
{
    std::string userName;
    std::string PIN;
    
    std::cout<<"Please insert your card(input your user name): " <<std::endl;
    std::cin>>userName;
    std::cout<<"Please enter your PIN code: "<<std::endl;
    getline(std::cin, PIN);

    //here we should connect to the server and make a query for the user tried to log in.
}
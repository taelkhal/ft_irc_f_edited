#include "conf/configFile.hpp"
#include "multuplixing/multuplixing.hpp"
#include "Server.hpp"
int main(int ac, char **av)
{
    server *myServer;
    (void )myServer;
    try
    {
        myServer = parseUser(ac, av);
        getSocket(myServer);
        mult(myServer);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}
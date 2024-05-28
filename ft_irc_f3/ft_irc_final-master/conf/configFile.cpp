#include "configFile.hpp"
#include <iostream>
#include <vector>
#include <sstream>

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}


int checkForPort(char *port)
{
    for (size_t i = 0; i < std::strlen(port); i++)
    {
        if (port[i] < '0' || port[i] > '9'){
            printf("%c\n", port[i]);
            throw std::runtime_error("Invalid port");
    }}
    if (std::atoi(port) > 65535)
        throw (std::runtime_error("Invalid range of port"));
    return (1);
}



server *parseUser(int ac, char **av)
{
    User *myUser = new User;
    server *myServer = new server;
    std::string pswd(av[2]);
    if (ac != 3)
        throw std::runtime_error("ERROR: example of using: ./ircserv <port> <password> ");
    if (checkForPort(av[1]))
        myServer->port = std::atoi(av[1]);
    if (pswd.empty())
        throw std::runtime_error("Password invalid");
    myServer->password = av[2];
    myServer->name = "localhost";
    myServer->Users.push_back(*myUser);
    return (myServer);
}

void passMsg(int fd)
{
    std::string msg = "Pleas enter the pass for this server...";
    send(fd, msg.c_str(), msg.length(), 0);
}

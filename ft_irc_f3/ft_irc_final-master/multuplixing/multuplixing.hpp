#ifndef MULTUPLIXING_HPP
#define MULTUPLIXING_HPP


#include "../conf/configFile.hpp"
#include  "../parse/parse.hpp"

#include <iostream>
#include <fstream>
#include <poll.h>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstdlib>
#include <string>
#include <csignal>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

class request
{
    public:
        int fd;
        std::string file;
        int fileIndex;
        char buff[1024];
        std::ofstream fileD;
    public:
        request(){};
        request (const request &copy){(void)copy;};
        request& operator=(const request &copy){
            (void)copy;
            return *this;
            };
};

class client{
    private:
        int indice;
    public:
        int fd;
        std::string nick;
        std::string name;
        client(){};
        client(int fd){this->fd = fd;};
        request req;
        void setIndice(int n){indice = n;};
        int getIndice(){return indice;};
};

class request;
void mult (server *ser);
void handleCtrlZ(int signum);
void getSocket(server *ser);
void poll_fds(int fd, server *ser);
void passMsg(int fd);
void send_welcome_message(int client_socket, server *ser, std::string);
void send_host_info(int client_socket, server *ser, std::string);
void send_creation_date(int client_socket, server *ser, std::string);
void send_server_info(int client_socket, server *ser, std::string);
void send_message(int client_socket, const char* message);
std::vector<client> removeClient(std::vector<client> mycl, int fd);
client *AddClient(int newFd, server *ser);
client *getClient(int fd, std::vector<client> &mycl);
#endif
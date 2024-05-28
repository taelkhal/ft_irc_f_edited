#ifndef PARSE_HPP
#define PARSE_HPP


#include "../multuplixing/multuplixing.hpp"
#include "../conf/configFile.hpp"
#include "../Server.hpp"
class client;
class Server;

void parseBuff(int fd, std::string buff, server *ser, client *cl, Server *s, std::vector<client> );
int validUser(int fd, server *ser, client *cl, std::string buff, std::vector<client> mycl);
int validPass(std::string buff, std::string pass, server *ser, int fd, client *cl);
int validNick(std::string buff, int fd, server *ser, client *cl, Server *s, std::vector<client> mycl);
int matchWord (std::string t, std::string tmp);

#endif
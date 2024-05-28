#include "../conf/configFile.hpp"
#include "../multuplixing/multuplixing.hpp"
#include "parse.hpp"
#include "../Server.hpp"

int matchWord (std::string t, std::string tmp)
{
    size_t i = 0;
    for (; t[i] != 32 && t[i] != '\t' && t[i]; i++){
        if (tmp[i] != t[i])
            return 0;
    }
    if (i != tmp.length())
        return 0;
    return 1;
}

int validNick(std::string buff, int fd, server *ser, client *cl, Server *s, std::vector<client> mycl)
{
    //nc no 10
    //irssi 13
    (void )ser;
    std::vector <std::string >t = splitString(buff, ' ');
    std::string tmp;
    if (t[1][t[1].length() - 2] == 13)
        tmp = t[1].substr(0, t[1].length() - 2);
    else
        tmp = t[1].substr(0, t[1].length() - 1);
    if (tmp.empty() || buff[0] == '$' || buff[0] == ':'){
        std::string t = "irssi " + tmp + " :Erroneus nickname\r\n";
        send_message(fd, t.c_str());
        send_message(fd, "please enter your nickname :\r\n");
        return 0;
    }
    for (int i = 0; tmp[i]; i++)
    {
        if (tmp[i] == 32 || tmp[i] == '.' || tmp[i] == '\t' || tmp[i] == '?' || tmp[i] == '!' || tmp[i] == ',' || tmp[i] == '@' || tmp[i] == '*' || tmp[i] == '#'){
            std::string t = "irssi " + tmp + " :Erroneus nickname\r\n";
            send_message(fd, "please enter your nickname :\r\n");
            return 0;
        }
    }
    for (std::vector<client>::iterator it = mycl.begin(); it != mycl.end(); it++)
    {
        if (tmp == it->nick)
        {
            std::string t = "irssi " + tmp + " :Erroneus nickname\r\n";
            send_message(fd, t.c_str());
            send_message(fd, "please enter your nickname :\r\n");
            return 0;
        }
    }
    cl->nick = tmp;
    cl->setIndice(2);
    s->set_fd_users(cl->nick, fd);
    send_message(fd, "Valid Nick\r\n");
    send_message(fd, "Please enter your username :\r\n");
    
    return 1;

}
int validPass(std::string buff, std::string pass, server *ser, int fd, client *cl)
{
    std::vector<std::string> tmp = splitString(buff, ' ');
    if (tmp.size() !=  2){
        send_message(fd, "Password incorrect\r\n");
        send_message(fd, "please enter password :\r\n");
    }
    (void )ser;
    (void )cl;
    size_t i = 0;
    for (; pass[i]; i++){
        if (tmp[1][i] != pass[i]){
        send_message(fd, "Password incorrect\r\n");
        send_message(fd, "please enter password :\r\n");
        return 0;
        }
    }
    for (;tmp[1][i];i++)
    {
        if (tmp[1][i] > 16){
        send_message(fd, "Password incorrect\r\n");
        send_message(fd, "please enter password :\r\n");
        return 0;
    }
    }
    cl->setIndice(1);
    send_message(fd, "password match\r\n");
    send_message(fd, "please enter your nickname :\r\n");
    return 1;
}

int validUser(int fd, server *ser, client *cl, std::string buff, std::vector<client> mycl)
{
    (void )fd;
    (void )ser;
    (void )cl;
    (void )buff;
    std::vector<std::string> tmp = splitString(buff, ' ');
    if (tmp.size() != 5){
        send_message(fd, "irssi user : Not enought parametres\r\n");
        send_message(fd, "Please enter your userName :\r\n");
        return 0;
    }
    if (tmp[1].empty() || tmp[2].length() != 1 || tmp[3].length() != 1 || tmp[2][0] != '0' || tmp[3][0] != '*'){
        send_message(fd, "irssi user : Errors in  parametres\n");
        send_message(fd, "Please enter your userName :\r\n");
        return 0;
    }
    for (std::vector<client>::iterator it = mycl.begin(); it != mycl.end(); it++)
    {
        if (tmp[1] == it->name)
        {
            std::string t = "irssi " + tmp[0] + " :Erroneus nickname\r\n";
            send_message(fd, t.c_str());
            send_message(fd, "please enter your userName :\r\n");
            return 0;
        }
    }
    send_message(fd, "Valid User\r\n");
    cl->setIndice(3);
    send_welcome_message(fd, ser, cl->nick);
    send_host_info(fd, ser, cl->nick);
    send_creation_date(fd, ser, cl->nick);
    send_server_info(fd, ser, cl->nick);
    cl->name = tmp[1];
    return 1;
}

void parseBuff(int fd, std::string buff, server *ser, client *cl, Server *s, std::vector<client> mycl)
{
    if ((matchWord(buff, "pass") || matchWord(buff, "PASS")) && cl->getIndice() == 0){
        validPass(buff, ser->password, ser, fd, cl);
        }
    else if ((matchWord(buff, "user") || matchWord(buff, "USER")) && cl->getIndice() == 2){
        validUser(fd, ser, cl, buff, mycl);
        }
    else if ((matchWord(buff, "nick") || matchWord(buff, "NICK")) && cl->getIndice() == 1){
        validNick(buff, fd, ser, cl, s, mycl);
        }
}

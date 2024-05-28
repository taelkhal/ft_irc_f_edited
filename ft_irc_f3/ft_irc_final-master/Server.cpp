#include "Server.hpp"
#include "multuplixing/multuplixing.hpp"
#include "parse/parse.hpp"
bool Server::_signal = false;

Server::Server() {}

Server::~Server() {}


void Server::set_av(std::string av)
{
    this->av = av;
}

std::string Server::get_av()
{
    return this->av;
}

void Server::parseArgs(int ac, char **av) {
    if (ac != 3)
        throw std::runtime_error("Usage: ./ircserv <port> <password>");
    std::string port(av[1]);
    std::string pwd(av[2]);
    if (port.empty() || port.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Error: Invalid arguments");

    long _port = atol(av[1]);
    if (!(_port >= 1 && _port <= 65535))
        throw std::runtime_error("Error: Invalid arguments");

    if (pwd.empty())
        throw std::runtime_error("Error: Invalid arguments");

    this->_port = _port;
    this->_password = pwd;
}

void Server::receiveSignal(int signum) {
    _signal = true;
    (void)signum;
}

void Server::init() {
    signal(SIGINT, receiveSignal);
    signal(SIGQUIT, receiveSignal);

    create_server();
    std::cout << ">>> SERVER STARTED <<<" << std::endl;
    std::cout << "Waiting for connections..." << std::endl;
}

void Server::run() {
    while (!_signal) {
        int ret = poll(&_fds[0], _fds.size(), -1);
        if (ret == -1)
            throw std::runtime_error("Error: poll() failed");

        for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _serverSocketFd)
                    client_connection();
                // else
                //     process_data(_fds[i].fd);
            }
        }
    }
    closeFds();
}

void Server::create_server() {
    _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocketFd == -1)
        throw std::runtime_error("Error: failed to create socket");

    int optval = 1;
    if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        throw std::runtime_error("Error: setsockopt() failed");

    if (fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error: fcntl() failed");

    bind_server();

    if (listen(_serverSocketFd, SOMAXCONN) == -1)
        throw std::runtime_error("Error: listen() failed");

    poll_fds(_serverSocketFd, POLLIN, 0);
}

void Server::bind_server() {
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(_port);
    sa.sin_addr.s_addr = INADDR_ANY;
    if (bind(_serverSocketFd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        throw std::runtime_error("Error: failed to bind socket");
    }
}

void Server::poll_fds(int fd, short events, short revents) {
    struct pollfd newPollfd;
    newPollfd.fd = fd;
    newPollfd.events = events;
    newPollfd.revents = revents;
    _fds.push_back(newPollfd);
}

void Server::client_connection() 
{
    struct sockaddr_in client_addr;
    socklen_t clientAddrSize = sizeof(sockaddr_in);
    int newFd = accept(_serverSocketFd, (struct sockaddr *)&client_addr, &clientAddrSize);
    if (newFd == -1) {
        throw std::runtime_error("Error: accept() failed");
    }

    if (fcntl(newFd, F_SETFL, O_NONBLOCK) == -1) // set non-blocking to the new socket fd it's mean that the socket will not block the program
        throw std::runtime_error("Error: fcntl() failed");
    
    std::string entrepass = "Enter Password Please : \n";

    send(newFd, entrepass.c_str(), entrepass.size(), 0); 

    poll_fds(newFd, POLLIN, 0);
    _clients.push_back(Client(newFd, inet_ntoa((client_addr.sin_addr))));
    std::cout << "Client <" << newFd << "> Connected" << std::endl;
}

void Server::process_data(int fd, int nbytes , char *buffer, server *ser, client *cl, std::vector<client> mycl, Server *s) 
{
    std::string command ;
    std::string n;
    static std::string c;

    bool foundEof = false;

    for (ssize_t i = 0; i < nbytes; ++i) 
    {
        if (buffer[i] == '\n') 
        {
            c += buffer;
            command  = c;
            c.clear();
            foundEof = true;
            break;
        }
    }
    if (!foundEof) 
    {
        buffer[nbytes] = '\0';
        command += buffer;
        c += buffer;
    } 
    else 
    {
    bool is_admin = false;
        std::cout << "command when enter: " << command << "|{}|" << std::endl;
        if ((matchWord(command, "pass") || matchWord(command, "PASS")) && cl->getIndice() == 0){
            validPass(command, ser->password, ser, fd, cl);
        }
        else if ((matchWord(command, "user") || matchWord(command, "USER")) && cl->getIndice() == 2){
            validUser(fd, ser, cl, command, mycl);
        }
        else if ((matchWord(command, "nick") || matchWord(command, "NICK")) && cl->getIndice() == 1){
            validNick(command, fd, ser, cl, s, mycl);
        }
        else if (command.substr(0, 5) == "join " || command.substr(0, 5) == "JOIN ")
        {            
            std::cout << "hna hna" << std::endl;
            std::map<std::string, std::vector<std::string> >::iterator it;
            std::map<std::string, std::string>::iterator it_key;
            std::cout << "hna hna 2" << std::endl;
            std::istringstream iss(command);
            std::string cmd ,channelname, pass;
            std::cout << "hna hna 3" << std::endl;

            iss >> cmd;
            iss >> channelname;
            std::getline(iss, pass); // Read the rest of the line as message text
            if (!pass.empty())
            {
                pass = pass.substr(1);
                std::cout << "hna hna 4" << std::endl;
                pass = pass.substr(0, pass.size());
                pass = pass.substr(0, pass.size() - 1);
            }
            std::cout << "hna hna 5" << std::endl;
            int check_key = 0;
            int key_valid = 0;


            if(channel_password.size() > 0)
            {
                it_key = channel_password.find(channelname);
                if (it_key != channel_password.end())
                    check_key = 1;
                if (pass == it_key->second)
                    key_valid = 1;
            }
            std::map<std::string, int>::iterator it_limit;
            int limited = 0;

            if(channel_limit.size() > 0)
            {
                it_limit = channel_limit.find(channelname);
                if (it_limit != channel_limit.end())
                {
                    std::set<int> fl = send_msg(channelname);
                    it = channel.find(channelname);
                    int size = fl.size();
                    if(it_limit->second == size)
                        limited = 1;
                }
            }

            it = invited.find(channelname);
            int check = 0;
            if (it == invited.end())
                check = 1;
            int check_invited = if_member_invited(fd, channelname);
            if((check == 0 && check_invited == 1) || check == 1)
            {
                if((check_key == 1 && key_valid == 1) || check_key == 0)
                {
                    if(limited == 0)
                    {
                        if (cmd == "join" || cmd =="JOIN") {
                            std::string user_c;
                            for (std::map<std::string, std::vector<int> >::iterator it = nick_fd.begin(); it != nick_fd.end(); ++it) 
                            {
                                std::vector<int>::iterator vec_it = std::find(it->second.begin(), it->second.end(), fd);
                                if (vec_it != it->second.end()) {
                                    user_c = it->first;
                                    break;
                                }
                            }
                                    /*creatchannel*/
                            create_channel(channelname, user_c);
                            for (std::map<std::string, std::vector<int> >::iterator it = nick_fd.begin(); it != nick_fd.end(); ++it) 
                            {
                                std::vector<int>::iterator vec_it = std::find(it->second.begin(), it->second.end(), fd);
                                if (vec_it != it->second.end()) 
                                {
                                    user_c = it->first;
                                    std::map<std::string, std::vector<std::string> >::iterator it_1 = channel.find(channelname);
                                    if (it_1 != channel.end()) 
                                    {
                                        for (size_t i = 0; i < it_1->second.size(); ++i) 
                                        {
                                            if (it_1->second[i][0] == '@' && it_1->second[i].substr(1) == user_c) 
                                            {
                                                is_admin = true;
                                                set_admin_perm(is_admin);
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            std::string join_msg = ":" + user_c + " JOIN " + channelname + "\r\n";
                            std::map<std::string, std::vector<std::string> >::iterator it_1 = channel.find(channelname);
                            std::string names_msg = ":irc.ChatWladMinah 353 " + user_c +  " = " + channelname + " :";
                            std::vector<std::string>::iterator it_v;
                            for (it_v = it_1->second.begin(); it_v != it_1->second.end(); it_v++)
                            {
                                if (is_admin == true) 
                                    names_msg += "@" + *it_v;
                                else 
                                    names_msg += *it_v;
                                if (it_v < it_1->second.end() - 1)
                                    names_msg += " ";
                            }
                            names_msg += "\n";
                            std::string endOfNamesMessage = ":irc.ChatWladMina 366 " + user_c + " " + channelname + " :End of /NAMES list.\r\n";
                            std::string channel_msg = ":irc.ChatWladMina 354 " + channelname + "\r\n";
                                // Send messages to the client
                            send(fd, join_msg.c_str(), join_msg.length(), 0);
                            it = channel.find(channelname);
                            if (it->second.size() == 1)
                            {
                                std::string mode_msg = ":irc.ChatWladMina MODE " + channelname + " +nt\r\n";
                                send(fd, mode_msg.c_str(), mode_msg.length(), 0);
                            }
                            else
                            {
                                std::map<std::string, std::string>::iterator it;
                                it = channel_topic.find(channelname);
                                std::string topic;
                                if (it == channel_topic.end())
                                    topic = "has no topic";
                                else
                                    topic = it->second;
                                std::string topic_msg = ":irc.ChatWladMina 332 " + user_c + " " + channelname + " :" + topic + " https://irc.com\r\n";
                                send(fd, topic_msg.c_str(), topic_msg.length(), 0);
                            }
                            send(fd, names_msg.c_str(), names_msg.length(), 0);
                            send(fd, endOfNamesMessage.c_str(), endOfNamesMessage.length(), 0);
                            send(fd, channel_msg.c_str(), channel_msg.length(), 0);
                        }
                    }
                }
            }
            /////******************************join************************************/////
            /////**********************************************************************/////
        }
        else if (command.substr(0, 7) == "privmsg" || command.substr(0, 7) == "PRIVMSG")
                processMessage(command, fd);
        else if (command.substr(0, 5) == "kick " || command.substr(0, 5) == "KICK ")
        {
            std::istringstream iss(command);
            std::string command, channel_kicked_from, user_kicked, reason;

            iss >> command;
            iss >> channel_kicked_from;
            iss >> user_kicked;
            std::getline(iss, reason);
            if(!reason.empty())
            {
                reason = reason.substr(1);
                reason = reason.substr(0, reason.size());
            }
            int perm = if_admin_inchannel(fd, channel_kicked_from);
            std::string user = get_user(fd);
            if(perm == 1)
            {
                int check_ = 20;
                if(user_kicked[0] != '@' || user == get_admin(channel_kicked_from))
                {
                    check_ = kick_memeber(channel_kicked_from, user_kicked, reason, fd);
                    kick_memeber_invited(channel_kicked_from,user_kicked);
                }
                else
                {
                    std::string user_not_found = ":" + admin + " PRIVMSG " + channel_kicked_from + " the user : " + user_kicked + " not member of this channel.\r\n";
                    send(fd, user_not_found.c_str(), user_not_found.size(), 0);
                }
                if(check_ == 1)
                {
                    std::string user_not_found = ":" + admin + " PRIVMSG " + channel_kicked_from + " the user : " + user_kicked + " not member of this channel.\r\n";
                    send(fd, user_not_found.c_str(), user_not_found.size(), 0);
                }
                else if(check_ == 2)
                {
                    std::string ser = "Server";
                    std::string not_admin = ":" + ser + " PRIVMSG " + channel_kicked_from + " not allowed to use this command kick " + user_kicked + "\r\n";
                    send(fd, not_admin.c_str(), not_admin.size(), 0);
                }
            }
            else
            {
                std::string ser = "Server";
                std::string not_admin = ":" + ser + " PRIVMSG " + channel_kicked_from + " not allowed to use this command kick " + user_kicked + "\r\n";
                send(fd, not_admin.c_str(), not_admin.size(), 0);
            }
        }
        else if(command.substr(0, 7) == "invite " || command.substr(0, 7) == "INVITE ")
        {
            std::string invite;
            size_t space_pos = command.find(' ');
            if (space_pos != std::string::npos)
            {
                invite = command.substr(space_pos + 1);
                invite = invite.substr(0, invite.size() - 1);
                std::istringstream iss(command);
                std::string command, channel_invited_from, user_invited;
                iss >> command;
                iss >> user_invited;
                iss >> channel_invited_from;
                std::map<std::string, std::vector<int> >::iterator it;
                it = nick_fd.find(user_invited);
                
                std::cout << channel_invited_from << std::endl;
                if (it != nick_fd.end())
                {
                    std::cout << if_admin_inchannel(fd, channel_invited_from) << std::endl;
                    if (if_admin_inchannel(fd, channel_invited_from) == 1)
                    {
                        if (if_invited(user_invited, channel_invited_from) == false)
                        {
                            std::string invite_msg = ":" + get_user(fd) + " INVITE " + user_invited + " :" + channel_invited_from + "\r\n";
                            int invited_fd = get_fd_users(user_invited);
                            invited_to_channel(channel_invited_from, user_invited);
                            send(fd, invite_msg.c_str(), invite_msg.size(), 0);
                            send(invited_fd, invite_msg.c_str(), invite_msg.size(), 0);
                        }
                        else
                        {
                            std::string ser = "Server";
                            std::string user_found = ":" + ser + " PRIVMSG " + channel_invited_from + " user already in channel or invited " + user_invited + "\r\n";
                            send(fd, user_found.c_str(), user_found.size(), 0);
                        }
                    }
                    else
                    {
                        std::string ser = "Server";
                        std::string not_admin = ":" + ser + " PRIVMSG " + channel_invited_from + " not allowed to use this command invite " + user_invited + "\r\n";
                        send(fd, not_admin.c_str(), not_admin.size(), 0);
                    }
                }
                else
                {
                    std::string serv = "Server";
                    std::string user_not_found = ":" + serv + " PRIVMSG " + channel_invited_from + " the user : " + user_invited + " not found.\r\n";
                    send(fd, user_not_found.c_str(), user_not_found.size(), 0); 
                }
            }
        }
        else if (command.substr(0, 5) == "mode " || command.substr(0, 5) == "MODE ")
        {
            std::istringstream iss(command);
            std::string cmd,name_channel, mode, key;
            iss >> cmd;
            iss >> name_channel;
            iss >> mode;
            iss >> key;           
            int check = if_admin_inchannel(fd, name_channel);
            if (check == 1)
            {
                if (mode == "+i" || mode == "-i")
                {
                    mode_i(mode, name_channel);
                    std::string msg_i = ":server.host MODE " + name_channel + " " + mode + " by " + get_admin(name_channel) + "\n";
                    send(fd, msg_i.c_str(), msg_i.size(), 0);
                }
                else if (mode == "+t" || mode == "-t")
                {
                    mode_topic(mode, name_channel, fd);
                }
                else if (mode == "+k" || mode == "-k")
                {
                    mode_k(mode, name_channel, key);
                    std::string msg_k = ":server.host MODE " + name_channel + " " + mode + " by " + get_admin(name_channel) + "\n";
                    send(fd, msg_k.c_str(), msg_k.size(), 0);
                }
                else if (mode == "+o" || mode == "-o")
                {
                    mode_o(mode, name_channel, key, fd);
                }
                else if (mode == "+l" || mode == "-l")
                {
                    mode_l(mode, name_channel, key, fd);
                }
                else if (mode != "")
                {
                    std::string ser = "Server";
                    std::string not_admin = ":" + ser + " PRIVMSG " + name_channel + " invalid command " + mode + "\r\n";
                    send(fd, not_admin.c_str(), not_admin.size(), 0);
                }
            }
            else if(check == 0 && mode != "" && name_channel[0] == '#')
            {
                std::string ser = "Server";
                std::string not_admin = ":" + ser + " PRIVMSG " + name_channel + " not allowed to use this command mode " + mode + "\r\n";
                send(fd, not_admin.c_str(), not_admin.size(), 0);
            }
        }
        else if (command.substr(0, 6) == "topic " || command.substr(0, 6) == "TOPIC ")
        {
            std::istringstream iss(command);
            std::string cmd, channel_nn, topic;
            iss >> cmd;
            iss >> channel_nn;
            iss >> topic;
            channel_topic[channel_nn] = topic;
            
            std::vector<std::string>::iterator it = std::find(topic_mode.begin(), topic_mode.end(), channel_nn);
            int check = if_admin_inchannel(fd, channel_nn);
            if (it != topic_mode.end() || check == 1)
            {
                std::string topic_t = ":" + get_user(fd) + " TOPIC " + channel_nn + " :" + topic + "\r\n";
                send(fd, topic_t.c_str(), topic_t.size(), 0);
            }
            else
            {
                std::string msg_e = ":" + get_user(fd) + " PRIVMSG " + channel_nn + " :Error: You can't execute this command " + "\r\n";
                send(fd, msg_e.c_str(), msg_e.size(), 0);
            }
        }
        else if (command.substr(0, 5) == "ping " || command.substr(0, 5) == "PING ")
            ping(command, fd);
        else if (command.substr(0, 5) == "quit " || command.substr(0, 5) == "QUIT ")
        {
            std::string user = get_user(fd);
            std::map<std::string, std::vector<std::string> >::iterator it;
            std::vector<std::string>::iterator it1;
            std::vector<std::string> temp;

            for (it = channel.begin(); it != channel.end(); it++)
            {
                if(it->second.size() > 0)
                {
                    for (it1 = it->second.begin(); it1 != it->second.end(); it1++)
                    {
                        std::string tmp = *it1;
                        tmp = &tmp[1];
                        if (user == *it1 || user == tmp)
                        {
                            std::string q = it->first;
                            std::string a = *it1;
                            erase_it(q, a);
                            break;
                        }   
                    }
                }
                if(it->second.size() == 0)
                {
                    std::string leave = it->first;
                    temp.push_back(leave);
                }       
            }
            for(it1 = temp.begin(); it1 != temp.end(); it1++)
            {
                channel.erase(*it1);
                channel_limit.erase(*it1);
                channel_password.erase(*it1);
            }
                
            temp.erase(temp.begin(), temp.end());
            for (it = invited.begin(); it != invited.end(); it++)
            {
                if(it->second.size() > 0)
                {
                    for (it1 = it->second.begin(); it1 != it->second.end(); it1++)
                    {
                        std::string tmp = *it1;
                        tmp = &tmp[1];
                        if (user == *it1 || user == tmp)
                        {
                            std::string q = it->first;
                            erase_inv(q, user);
                            break;
                        }   
                    }
                }
                if(it->second.size() == 0)
                {
                    std::string leave = it->first;
                    temp.push_back(leave);
                }
            }
            for(it1 = temp.begin(); it1 != temp.end(); it1++)
                invited.erase(*it1);
            temp.erase(temp.begin(), temp.end());
            nick_fd.erase(user);
        }
        else if (command.substr(0, 3) == "bot" || command.substr(0, 3) == "BOT")
        {
            std::map<std::string, std::vector<int> >::iterator it;
            std::vector<std::string> users;
            if (nick_fd.size() != 0)
            {
                for (it = nick_fd.begin(); it != nick_fd.end(); it++)
                {
                    users.push_back(it->first);
                }
                srand(time(0));
                int random_index = rand() % users.size();
                std::string winner = users[random_index];
                if (winner == get_user(fd))
                {
                    std::string bot_msg = ":server.host PRIVMSG " + get_user(fd) + " Congratulations ,you win the jackpot\n";
                    send(fd, bot_msg.c_str(), bot_msg.size(), 0);
                }
                else
                {
                    std::string bot_msg = ":server.host PRIVMSG " + get_user(fd) + " Unlucky good luck next time \n";
                    send(fd, bot_msg.c_str(), bot_msg.size(), 0);
                }
            }
        }
    }
    if (nbytes == 0) {
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        clientCleanup(fd);
    } else if (nbytes == -1) {
        std::cerr << "Error reading data from client <" << fd << ">" << std::endl;
        clientCleanup(fd);
    }
}

void Server::clientCleanup(int fd) {
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it) {
        if (it->fd == fd) {
            _fds.erase(it);
            close(fd);
            break;
        }
    }

    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->getFd() == fd) {
            _clients.erase(it);
            break;
        }
    }
}

void Server::welcome(const std::string& nickname, int fd)
{
    std::string msg_one = ":irc.ChatWladMina 001 " + nickname + " :Welcome to the ChatWladMina Network, " + nickname + '\n';
    std::string msg_two = ":irc.ChatWladMina 002 " + nickname + " :Your host is ChatWladMina, running version 4.5" + '\n';
    std::string msg_tre = ":irc.ChatWladMina 003 " + nickname + " :This server was created " + '\n';
    std::string msg_foor = ":irc.ChatWladMina 004 " + nickname + " ChatWladMina ChatWladMina(enterprise)-2.3(12)-netty(5.4c)-proxy(0.9) ChatWladMina" + '\n';
    send(fd, msg_one.c_str(), msg_one.length(), 0);
    send(fd, msg_two.c_str(), msg_two.length(), 0);
    send(fd, msg_tre.c_str(), msg_tre.length(), 0);
    send(fd, msg_foor.c_str(), msg_foor.length(), 0);
}

void Server::closeFds() {
    for (size_t i = 0; i < _clients.size(); i++){
        int fd = _clients[i].getFd();
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        close(fd);
    }

    if (_serverSocketFd != -1)
        close(_serverSocketFd);

    _fds.clear();
}
void Server::ping(const std::string& command, int fd) {
    std::istringstream iss(command);
    std::string host = command.substr(5);
    std::string msg = "PONG " + host + "\r\n";
    send(fd, msg.c_str(), msg.length(), 0);
}

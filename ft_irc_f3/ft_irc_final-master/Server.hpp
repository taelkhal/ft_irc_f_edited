#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include <cstring>
#include <unistd.h>
#include <map>
#include <sstream>
#include <set>
#include "Client.hpp"
#include "multuplixing/multuplixing.hpp"

class client;
class Server {
    private:
        int _port;
        int _serverSocketFd;
        std::string av;
        int fd_u;
        std::string admin;
        std::string user_n;
        bool admin_perm;
        
        std::string nick_n;
        static bool _signal;
        std::string _password;
        std::map<std::string, std::vector<std::string> > channel;
        std::map<std::string, std::vector<int> > nick_fd;
        std::map<std::string, std::vector<std::string> > invited;
        std::map<std::string, std::string> channel_password;
        std::map<std::string, int> channel_limit;
        // std::map<std::string, std::vector<int> > user_fd;
        // std::vector<std::string> nicknames;
        // std::string channel_n;
        std::vector<struct pollfd> _fds;
        std::vector<Client> _clients;
        
        std::map<std::string, std::string> channel_topic;
        std::vector<std::string> topic_mode;


    public:
        Server();
        ~Server();
        
        void parseArgs(int ac, char **av);
        static void receiveSignal(int signum);
        void init();
        void run();

        void create_server();
        void bind_server();
        void poll_fds(int fd, short events, short revents);
        void client_connection();
        void clientCleanup(int fd);
        void closeFds();
        void set_av(std::string av);
        std::string get_av();
        void welcome(const std::string& nickname, int fd);
        void    set_fd_users(const std::string &user_name,int fd);
        bool    if_member(std::string user_inchannel, std::string channels_name);
        std::set<int>    send_msg(const std::string &channel_msg);
        int     get_fd_users(std::string &users_in_channel);
        void    create_channel(const std::string &channel_name,const std::string& user_name);
        void processMessage(const std::string& messag, int fd);
        void sendMessageToChannel(const std::string& channel, const std::string& message);
        void sendMessageToUser(int user, const std::string& message);
        std::string constructCreationTimeMessage(const std::string& channelName);
        std::string formatCreationTime();
        void ping(const std::string& command, int fd);
        int kick_memeber(std::string &channel_kicked_from, std::string &user_kicked, std::string reason, int fd);
        // void set_admin(std::string channel_a);
        std::string get_admin(std::string channel_a);
        void set_admin_perm(bool is_admin);
        bool get_admin_perm();
        void process_data(int fd, int nbytes , char *buffer, server *ser, client *cl, std::vector<client> mycl, Server *s) ;
        int if_admin_inchannel(int fd, std::string &channel_perm);
        void invited_to_channel(const std::string &channel_invited_to, const std:: string& user_invited);
        void mode_i(std::string mode, std::string name_channel_i);
        bool if_invited(std::string user_inchannel, std::string channels_name);
        int if_member_invited(int fd, std::string &channel_joined);
        int a_d(std::string user_inchannel, std::string channels_name);
        void kick_memeber_invited(std::string &channel_kicked_from_i, std::string &user_kicked_i);
        void mode_k(std::string mode, std::string channel_name, std::string key);
        void mode_l(std::string mode, std::string channel_name_l, std::string key, int fd);
        void mode_o(std::string mode, std::string channel_name_o, std::string key_o, int fd);
        void mode_topic(std::string mode, std::string channel_name_t, int fd);
        std::string get_user(int fd);
        void erase_it(std::string &channel_erased, std::string &user_erased);
        void erase_inv(std::string &channel_inv, std::string &user_inv);
        // void leave_channel(std::string &channel_leave_from, int fd);
};

#endif
#include "multuplixing.hpp"


void getSocket(server *ser)
{
    if ((ser->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        throw ("creating socket");
    }
    int optval = 1;
    if (setsockopt(ser->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        throw std::runtime_error("Error: setsockopt() failed");
    fcntl(ser->sock, F_SETFL, O_NONBLOCK);
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(ser->port);
    sa.sin_addr.s_addr = INADDR_ANY;
    if (bind(ser->sock, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        throw std::runtime_error("Error: failed to bind socket");
    };
    if ((ser->listen_fd = listen(ser->sock, SOMAXCONN)) == -1){
        close(ser->sock);
        throw std::runtime_error("error in listen");
    }
    poll_fds(ser->sock, ser);
}

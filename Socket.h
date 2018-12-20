#ifndef CHAT_SOCKET_H
#define CHAT_SOCKET_H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <json/json.h>

#define MAX_PORT 128
#define IP "192.168.237.128"
//#define IP "127.0.0.1"

namespace EPOLL {
    class Epoll;

    class LoginEpoll;

    class OnlineEpoll;

    class RegisterEpoll;

    class CheckOnlineEpoll;
}


namespace SOCKET {
    class Socket {
    public:
        friend class EPOLL::Epoll;


        Socket(const std::string &port) : m_ip(IP), m_port(atoi(port.c_str())) {};

        void socket_init();

        int set_nonblock(int &sockfd);

        void socket_bind();

        void socket_listen();

        int socket_accept();

        void socket_connect();

        void sendn(const int &sockfd, const char *msg, const int &send_len);

        int socket_send(const int &sockfd, const char *msg);

        void recvn(const int &sockfd, char *msg, const int &recv_len);

        int socket_recv(const int &sockfd, char *msg);

        int server_sock;

    private:
        const std::string m_ip;
        const int m_port;
        struct sockaddr_in server_addr;
        //int server_sock;
    };
}

#endif //CHAT_SOCKET_H

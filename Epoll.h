
#ifndef CHAT_EPOLL_H
#define CHAT_EPOLL_H

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
#include <sys/epoll.h>
#include "Socket.h"
#include "Factory.h"
#include <vector>
#include <json/json.h>
#include <sstream>

#define MAXSIZE 1024

namespace EPOLL {

    class Epoll {
    public:

        static EPOLL::Epoll *online_service;

        Epoll(const std::string port) : socket(new SOCKET::Socket(port)) {
        };

        Epoll(SOCKET::Socket *socket) : socket(socket) {
        };

        void epoll_init();

        void event_add(const int &sockfd, const int &state);

        void event_delete(const int &sockfd);

        void event_mod(const int &sockfd, const int &state);

        virtual void createThread() {

        };

        virtual void handle_request(const int &active) {

        };

        void server_init();

        void epoll_listen();

        int set_nonblock(int &sockfd);

    protected:
        SOCKET::Socket *socket;
        int epoll_fd;
        struct epoll_event events[MAXSIZE];
    };


    class RegisterEpoll : public Epoll {
    public:
        RegisterEpoll(const std::string port) : Epoll(port) {
            chat = new FACTORY::RegisterFactory(socket);
        }

        virtual void createThread();

        virtual void handle_request(const int &active);


    private:
        FACTORY::Factory *chat;
    };

    class LoginEpoll : public Epoll {
    public:
        LoginEpoll(const std::string port) : Epoll(port) {
            chat = new FACTORY::LoginFactory(socket);
        }

        virtual void createThread();

        virtual void handle_request(const int &active);


    private:
        FACTORY::Factory *chat;
    };


    class OnlineEpoll : public Epoll {
    public:


        OnlineEpoll(const std::string port) : Epoll(port) {
            chat = new FACTORY::OnlineFactory(socket);
        }

        virtual void createThread();

        virtual void handle_request(const int &active);


    private:
        FACTORY::Factory *chat;
    };


    class CheckOnlineEpoll : public Epoll {
    public:


        CheckOnlineEpoll(const std::string port) : Epoll(port) {
            chat = new FACTORY::CheckOnlineFactory(socket);
        }

        virtual void createThread();

        virtual void handle_request(const int &active);


    private:
        FACTORY::Factory *chat;
    };

}


#endif //CHAT_EPOLL_H

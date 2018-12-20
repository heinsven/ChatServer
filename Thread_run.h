#ifndef CHAT_THREAD_RUN_H
#define CHAT_THREAD_RUN_H

#include <iostream>
#include <list>
#include "Control.h"
#include "Socket.h"
#include "UserDao.h"


class Thread_run {
public:
    friend class EPOLL::Epoll;

    Thread_run(SOCKET::Socket *m_socket) : socket(m_socket) {
        userDao = new UserDao();
    };

    void set_info(Control *m_info);

    void put_sockfd(int sockfd);

    void get_sockfd(int &sockfd);

    void handle_chat(int &sockfd);

    void handle_register(int &sockfd);

    void handle_login(int &sockfd);

    void handle_online(int &sockfd);

    void handle_check_online(int &sockfd);

    void run();

    void run_register();

    void run_login();

    void run_online();

    void run_check_online();

    void add_friend(int &sockfd, string &friend_username);

    void remove_friend(int &sockfd, string &friend_username);

    void edit_remark(int &sockfd, string &username, string &friend_username, string &remark);

    bool logout(int &sockfd);

    void send_text(int &sockfd, string &text, string &send_to, string &recv_from);

    void send_file(int &sockfd, string &send_to, string &recv_from);

    void check_online(int &sockfd,string &friend_username);

private:
    Control *info;
    SOCKET::Socket *socket;
    UserDao *userDao;

};


#endif //CHAT_THREAD_RUN_H

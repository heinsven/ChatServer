#include "Epoll.h"

using namespace EPOLL;

static void *thread_add_friends(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_add_friend *arg = (struct MODEL::arg_add_friend *) obj;
    std::cout << arg->friend_username << std::endl;
    arg->chat->getHandle()->add_friend(arg->sockfd, arg->friend_username);
}

static void *thread_remove_friends(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_remove_friend *arg = (struct MODEL::arg_remove_friend *) obj;
    std::cout << arg->friend_username << std::endl;
    arg->chat->getHandle()->remove_friend(arg->sockfd, arg->friend_username);
}

static void *thread_edit_remark(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_edit_remark *arg = (struct MODEL::arg_edit_remark *) obj;
    arg->chat->getHandle()->edit_remark(arg->sockfd, arg->username, arg->friend_username, arg->remark);
}

static void *thread_logout(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_logout *arg = (struct MODEL::arg_logout *) obj;

    bool result = arg->chat->getHandle()->logout(arg->sockfd);

    if (result) {
        cout << arg->sockfd << " logout" << endl;
        arg->socket->socket_send(arg->sockfd, "success");
    } else {
        arg->socket->socket_send(arg->sockfd, "failure");
    }
}

static void *thread_send_text(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_send_text *arg = (struct MODEL::arg_send_text *) obj;
    cout << arg->recv_from << " will send msg to " << arg->send_to << endl;
    arg->chat->getHandle()->send_text(arg->sockfd, arg->text, arg->send_to, arg->recv_from);
}

static void *thread_send_file(void *obj) {
    pthread_detach(pthread_self());
    struct MODEL::arg_send_file *arg = (struct MODEL::arg_send_file *) obj;
    arg->socket->socket_send(arg->sockfd, "success");
    cout << arg->recv_from << " will send file to " << arg->send_to << endl;
    arg->chat->getHandle()->send_file(arg->sockfd, arg->send_to, arg->recv_from);
}


EPOLL::Epoll *EPOLL::Epoll::online_service;

void Epoll::epoll_init() {
    epoll_fd = epoll_create(MAXSIZE);
}

void Epoll::event_add(const int &sockfd, const int &state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = sockfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);
}

void Epoll::event_delete(const int &sockfd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, NULL);
}

void Epoll::event_mod(const int &sockfd, const int &state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = sockfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, sockfd, &ev);
}

void Epoll::server_init() {
    socket->socket_init();
    socket->set_nonblock(socket->server_sock);
    event_add(socket->server_sock, EPOLLIN | EPOLLET);
    socket->socket_bind();
    socket->socket_listen();
}

void Epoll::epoll_listen() {
    epoll_init();
    server_init();
    int active = 0;
    createThread();
    while (1) {
        active = epoll_wait(epoll_fd, events, MAXSIZE, 50000);
        handle_request(active);
    }
}

int Epoll::set_nonblock(int &sockfd) {
    int old = fcntl(sockfd, F_GETFL);
    old |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, old); //设置fd状态标志
    return 0;
}

void RegisterEpoll::handle_request(const int &active) {
    for (int index = 0; index < active; index++) {
        if (socket->server_sock == events[index].data.fd) {
            int fd_client = socket->socket_accept();
            cout << "register client socket: " << fd_client << endl;
            chat->handle->put_sockfd(fd_client);
        }
    }
}

void RegisterEpoll::createThread() {
    chat->on();
}

void LoginEpoll::handle_request(const int &active) {
    if (active == 0) {
        std::cout << "epoll listen system timeout !" << std::endl;
    }

    for (int index = 0; index < active; index++) {
        if (socket->server_sock == events[index].data.fd) {
            int fd_client = socket->socket_accept();
            cout << "login client socket: " << fd_client << endl;
            chat->handle->put_sockfd(fd_client);
        }
    }
}

void LoginEpoll::createThread() {
    chat->on();
}

void CheckOnlineEpoll::handle_request(const int &active) {
    if (active == 0) {
        std::cout << "epoll listen system timeout !" << std::endl;
    }

    for (int index = 0; index < active; index++) {
        if (socket->server_sock == events[index].data.fd) {
            int fd_client = socket->socket_accept();
            cout << "checkOnlineEpoll socket_fd: " << fd_client << endl;
            chat->handle->put_sockfd(fd_client);
        }
    }
}

void CheckOnlineEpoll::createThread() {
    chat->on();
}


void OnlineEpoll::handle_request(const int &active) {
    if (active == 0) {
        std::cout << "epoll listen system timeout !" << std::endl;
    }

    for (int index = 0; index < active; index++) {
        if (socket->server_sock == events[index].data.fd) {
            int fd_client = socket->socket_accept();
            cout << "receive cliend_fd is " << fd_client << endl;
            chat->handle->put_sockfd(fd_client);
        } else if (events[index].events | EPOLLIN) {
            int sockfd = events[index].data.fd;
            int msg_len = 0;
            socket->recvn(sockfd, (char *) &msg_len, 4);
            char recv_buf[msg_len];
            memset(recv_buf, '\0', sizeof(recv_buf));
            socket->recvn(sockfd, recv_buf, msg_len);

            cout << "receive data, the cliend_fd is " << sockfd << endl;
            cout << sockfd << " data is " << recv_buf << endl;
            if (msg_len > 0) {
                Json::Reader reader;
                Json::Value value;
                reader.parse(recv_buf, value, false);
                std::cout << "get it" << std::endl;

                std::string type = value["type"].asString();
                std::cout << type << std::endl;

                if (type == "send_text") {
                    struct MODEL::arg_send_text arg;
                    arg.sockfd = sockfd;
                    arg.chat = chat;
                    arg.text = value["msg"]["text"].asString();
                    arg.send_to = value["msg"]["send_to"].asString();
                    arg.recv_from = value["msg"]["recv_from"].asString();

                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_send_text, (void *) &arg);
                } else if (type == "add_friend") {
                    struct MODEL::arg_add_friend arg;
                    arg.friend_username = value["msg"]["friend_username"].asString();
                    arg.sockfd = sockfd;
                    arg.chat = chat;

                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_add_friends, (void *) &arg);
                } else if (type == "remove_friend") {
                    struct MODEL::arg_remove_friend arg;
                    arg.friend_username = value["msg"]["friend_username"].asString();
                    arg.sockfd = sockfd;
                    arg.chat = chat;

                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_remove_friends, (void *) &arg);
                } else if (type == "edit_remark") {
                    struct MODEL::arg_edit_remark arg;
                    arg.username = value["msg"]["username"].asString();
                    arg.friend_username = value["msg"]["friend_username"].asString();
                    arg.remark = value["msg"]["remark"].asString();
                    arg.sockfd = sockfd;
                    arg.chat = chat;


                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_edit_remark, (void *) &arg);
                } else if (type == "logout") {
                    event_delete(sockfd);
                    struct MODEL::arg_logout arg;
                    arg.sockfd = sockfd;
                    arg.chat = chat;
                    arg.socket = socket;

                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_logout, (void *) &arg);
                } else if (type == "send_file") {
                    struct MODEL::arg_send_file arg;
                    arg.sockfd = sockfd;
                    arg.socket = socket;
                    arg.chat = chat;
                    arg.send_to = value["msg"]["send_to"].asString();
                    arg.recv_from = value["msg"]["recv_from"].asString();

                    event_delete(sockfd);

                    pthread_t tid = getpid();
                    pthread_create(&tid, NULL, thread_send_file, (void *) &arg);
                }
            }

            cout << "epoll finish!" << endl;
        }
    }
}

void OnlineEpoll::createThread() {
    chat->on();
}



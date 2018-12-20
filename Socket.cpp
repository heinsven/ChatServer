#include "Socket.h"

using namespace SOCKET;

//Server(const std::string& ip,const std::string& port):m_ip(ip),m_port(atoi(port.c_str())){}
void Socket::socket_init() {
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    //set_nonblock(server_sock);
    if (server_sock == -1) {
        perror("socket init");
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    server_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    //inet_aton(IP,&server_addr.sin_addr);
}

int Socket::set_nonblock(int &sockfd) {
    int old = fcntl(sockfd, F_GETFL);
    old |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, old);
    return old;
}

void Socket::socket_bind() {
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR,
               (const void *) &opt, sizeof(opt));
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("socket bind!");
    }
}

void Socket::socket_listen() {
    if (listen(server_sock, MAX_PORT) == -1) {
        perror("socket listen!");
    }
}

int Socket::socket_accept() {
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    int fd_client;
    /*
    while((fd_client=accept(server_sock,(struct sockaddr*)&client_addr,&length))<0)
    {
        if(fd_client==-1&&errno!=EAGAIN)
        {
            perror("socket accept!");
            return -1;
        }
    }*/
    if ((fd_client = accept(server_sock, (struct sockaddr *) &client_addr, &length)) == -1) {
        perror("socket accept");
    }
    std::cout << "connetc client IP: " << inet_ntoa(client_addr.sin_addr) << " PORT:" << ntohs(client_addr.sin_port)
              << std::endl;
//    set_nonblock(fd_client);
    return fd_client;
}

void Socket::socket_connect() {
    int ret = 0;
    /*
    while((ret=connect(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr)))<0)
    {
        if(ret==-1&&errno!=EAGAIN)
        {
            perror("client connect");
            return;
        }
    }*/
    if ((ret = connect(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1) {
        perror("socket connect");
        return;
    }
}

void Socket::sendn(const int &sockfd, const char *msg, const int &send_len) {
    int send_ret = 0;
    int send_sum = 0;
    while (send_sum < send_len) {
        send_ret = send(sockfd, msg + send_sum, send_len - send_sum, 0);
        if (send_ret == -1 && errno != EAGAIN) {
            perror("send error");
            break;
        }
        send_sum += send_ret;
    }
}

int Socket::socket_send(const int &sockfd, const char *msg) {
    int msg_len = strlen(msg);
    sendn(sockfd, (char *) &msg_len, 4);
    sendn(sockfd, msg, msg_len);
    return msg_len;
}

void Socket::recvn(const int &sockfd, char *msg, const int &recv_len) {
    int recv_ret = 0;
    int recv_sum = 0;
    while (recv_sum < recv_len) {
        recv_ret = recv(sockfd, msg + recv_sum, recv_len - recv_sum, 0);
        if (recv_ret == -1 && errno != EAGAIN) {
            perror("recv error");
            break;
        }
        recv_sum += recv_ret;
    }
}

int Socket::socket_recv(const int &sockfd, char *msg) {
    int msg_len = 0;
    recvn(sockfd, (char *) &msg_len, 4);
    recvn(sockfd, msg, msg_len);
    return msg_len;
}
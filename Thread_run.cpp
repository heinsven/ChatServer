#include "Thread_run.h"
#include "MyDB.h"
#include "User.h"
#include "Epoll.h"

#define MAXLINE 4096

using namespace zlb_mysql;

void Thread_run::set_info(Control *m_info) {
    info = m_info;
}

void Thread_run::put_sockfd(int sockfd)/*client sockfd放入工作队列*/
{
    info->lock();
    (info->work_queue).push(sockfd);
    info->notify();
    info->unlock();
}

void Thread_run::get_sockfd(int &sockfd)/*从工作队列取出client sockfd*/
{
    info->lock();
    while ((info->work_queue).empty()) {
        info->wait();
    }
    sockfd = (info->work_queue).front();
    (info->work_queue).pop();
    info->notify();
    info->unlock();
}

void Thread_run::handle_chat(int &sockfd) {
    char recv_buf[1024] = "";
    int read_ret;
    while ((read_ret = socket->socket_recv(sockfd, recv_buf)) != 0) {
        if (read_ret == -1 && errno != EAGAIN) {
            perror("client closed");
            break;
        } else if (read_ret > 0) {
            std::string msgmsg(recv_buf);
            std::list<int>::iterator iter = info->online_client.begin();
            for (iter; iter != info->online_client.end(); iter++) {
                if (*iter == sockfd) {
                    continue;
                }
                socket->socket_send(*iter, msgmsg.c_str());
            }
            if (msgmsg.find("off") != std::string::npos) {
                break;
            }
        }
    }
    info->online_client.remove(sockfd);
    std::cout << sockfd << " has been closed " << std::endl;
    close(sockfd);
}

void Thread_run::handle_register(int &sockfd) {
    char recv_buf[1024] = "";
    int read_ret;
    while ((read_ret = socket->socket_recv(sockfd, recv_buf)) != 0) {
        if (read_ret == -1 && errno != EAGAIN) {
            perror("client closed");
            break;
        } else if (read_ret > 0) {
            Json::Reader reader;
            Json::Value value;
            reader.parse(recv_buf, value, false);
            std::string username = value["username"].asString();
            std::string password = value["password"].asString();
            std::string nickname = value["nickname"].asString();
            std::cout << username << ": " << password << ": " << nickname << std::endl;

            CDataBase *dataBase = new CDataBase();
            dataBase->Connect("localhost", "root", "root", "chat");

            string sql = "insert into t_user(username,password,nickname,status) values('";
            sql += username + "','" + password + "','" + nickname + "',0)";
            std::cout << sql;

            string querySql = "select * from t_user where username = '" + username + "'";

            if (!dataBase->ExecQueryGetSingValue(querySql)) {
                bool result = dataBase->ExecQuery(sql);
                if (result != 0) {
                    socket->socket_send(sockfd, "register success!");
                } else {
                    socket->socket_send(sockfd, "register failure!");
                }
            } else {
                socket->socket_send(sockfd, "the username has been occupied!");
            }

            dataBase->DisConnect();
        }
    }
    close(sockfd);
}

void Thread_run::handle_login(int &sockfd) {
    char recv_buf[1024] = "";
    int read_ret;
    read_ret = socket->socket_recv(sockfd, recv_buf);
    if (read_ret == -1 && errno != EAGAIN) {
        perror("client closed");
//        break;
    } else if (read_ret > 0) {
        Json::Reader reader;
        Json::Value value;
        reader.parse(recv_buf, value, false);
        std::string username = value["username"].asString();
        std::string password = value["password"].asString();
        std::cout << username << ": " << password << std::endl;

        CDataBase *dataBase = new CDataBase();
        dataBase->Connect("localhost", "root", "root", "chat");
        CRecordSet recordSet(dataBase->Get_Mysql());

        string querySql =
                "select * from t_user where username = '";
        querySql.append(username + "' and password = '");
        querySql.append(password + "'");
        cout << querySql;

        if (dataBase->ExecQueryGetSingValue(querySql)) {
            string sql = "select * from t_user where username = '" + username + "'";
            int result = recordSet.ExecuteSQL(sql.c_str());
            if (result == 1) {
                CRecord record = recordSet[0];
                Json::Value user;
                Json::Value attr;
                attr["username"] = record["username"];
                attr["password"] = record["password"];
                attr["nickname"] = record["nickname"];
                attr["status"] = record["status"];
                user.append(attr);
                Json::FastWriter writer;
                std::string userJson = writer.write(user);
                socket->socket_send(sockfd, userJson.c_str());
            } else {
                socket->socket_send(sockfd, "login failure");
            }
        } else {
            socket->socket_send(sockfd, "the username or the password is wrong!");
        }

        dataBase->DisConnect();
    }
    close(sockfd);
}

void Thread_run::handle_online(int &sockfd) {
    char recv_buf[256];
    int read_ret;
    USER::User *user = nullptr;
    read_ret = socket->socket_recv(sockfd, recv_buf);
    if (read_ret == -1 && errno != EAGAIN) {
        perror("client closed");
//        break;
    } else if (read_ret > 0) {
        recv_buf[read_ret] = '/0';
        Json::Reader reader;
        Json::Value value;
        reader.parse(recv_buf, value, false);
        user = new USER::User();
        user->setUsername(value["username"].asString());
        user->setPassword(value["password"].asString());
        user->setNickname(value["nickname"].asString());
        user->setStatus(value["status"].asString());
        std::cout << user << std::endl;

        std::list<MODEL::Relationship *> relationships = userDao->getUserFriends(user);
        Json::Value userFriends;

        for (std::list<MODEL::Relationship *>::iterator iter = relationships.begin();
             iter != relationships.end(); iter++) {
            Json::Value attr;
            attr["username"] = (*iter)->getUsername();
            attr["friend_username"] = (*iter)->getFriend_username();
            attr["remark_name"] = (*iter)->getRemark_name();
            userFriends.append(attr);

            USER::User *t_user = user->getOnlineFriend((*iter)->getFriend_username());
            if (t_user != NULL) {
                t_user->addOnlineFriend(sockfd, (*user));
            }
        }

        USER::User::addOnlineUsers(sockfd, (*user));

        Json::FastWriter writer;
        std::string relationshipJson = writer.write(userFriends);
        cout << relationshipJson << endl;
        socket->socket_send(sockfd, relationshipJson.c_str());

//        while (read_ret = (socket->socket_recv(sockfd, recv_buf)) <= 0);
//        EPOLL::Epoll::online_service->set_nonblock(sockfd);
        EPOLL::Epoll::online_service->event_add(sockfd, EPOLLIN | EPOLLET);
        cout << sockfd << " has been joined in epoll " << endl;

    }
}

void Thread_run::handle_check_online(int &sockfd) {
    char recv_buf[256];
    memset(recv_buf, '\0', strlen(recv_buf));
    int read_ret = socket->socket_recv(sockfd, recv_buf);
    if (read_ret == -1 && errno != EAGAIN) {
        perror("client closed");
    } else if (read_ret > 0) {
        recv_buf[read_ret] = '\0';
        Json::Reader reader;
        Json::Value value;
        reader.parse(recv_buf, value, false);
        std::string friend_username = value["friend_username"].asString();
        check_online(sockfd, friend_username);
    }
}

void Thread_run::run() {
    while (1) {
        int fd_client;
        get_sockfd(fd_client);
        handle_chat(fd_client);
    }
}

void Thread_run::run_register() {
    while (1) {
        int fd_client;
        get_sockfd(fd_client);
        handle_register(fd_client);
    }
}

void Thread_run::run_login() {
    while (1) {
        int fd_client;
        get_sockfd(fd_client);
        handle_login(fd_client);
    }
}

void Thread_run::run_online() {
    while (1) {
        int fd_client;
        get_sockfd(fd_client);
        handle_online(fd_client);
    }
}

void Thread_run::run_check_online() {
    while (1) {
        int fd_client;
        get_sockfd(fd_client);
        handle_check_online(fd_client);
    }
}


void Thread_run::add_friend(int &sockfd, string &friend_username) {
    int index = -1;

    for (auto iter = USER::User::onlineSockets.begin(); iter != USER::User::onlineSockets.end(); iter++) {
        index++;
        if ((*iter) == sockfd) {
            break;
        }
    }
    USER::User user;
    int i = -1;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        i++;
        if (i == index) {
            user = (*iter);
            break;
        }
    }
    std::cout << user.getUsername() << " will add " << friend_username << std::endl;
    USER::User *friend_user = userDao->getUser(friend_username);
    if (friend_user != nullptr) {
        int result = 0;
        userDao->addRelationship(friend_username, user.getUsername());
        result = userDao->addRelationship(user.getUsername(), friend_username);
        if (result != 0)
            socket->socket_send(sockfd, "success");
        else
            socket->socket_send(sockfd, "failure");
    } else {
        socket->socket_send(sockfd, "failure");
    }

    cout << "finish add friend " << endl;
}

void Thread_run::remove_friend(int &sockfd, string &friend_username) {
    int index = -1;

    for (auto iter = USER::User::onlineSockets.begin(); iter != USER::User::onlineSockets.end(); iter++) {
        index++;
        if ((*iter) == sockfd) {
            break;
        }
    }
    USER::User user;
    int i = -1;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        i++;
        if (i == index) {
            user = (*iter);
            break;
        }
    }
    std::cout << user.getUsername() << " will remove " << friend_username << std::endl;
    int result = 0;
    userDao->rmRelationship(friend_username, user.getUsername());
    result = userDao->rmRelationship(user.getUsername(), friend_username);
    if (result > 0) {
        socket->socket_send(sockfd, "success");
    } else {
        socket->socket_send(sockfd, "failure");
    }
}

void Thread_run::edit_remark(int &sockfd, string &username, string &friend_username, string &remark) {
    std::cout << username << " will edit " << friend_username << " as " << remark << std::endl;
    int result = 0;
    result = userDao->editRemark(username, friend_username, remark);
    if (result > 0) {
        socket->socket_send(sockfd, "success");
    } else {
        socket->socket_send(sockfd, "failure");
    }

}

bool Thread_run::logout(int &sockfd) {
    bool result_1 = false;
    bool result_2 = false;

    int index = -1;

    for (auto iter = USER::User::onlineSockets.begin(); iter != USER::User::onlineSockets.end(); iter++) {
        index++;
        if ((*iter) == sockfd) {
            USER::User::onlineSockets.erase(iter);
            result_1 = true;
            break;
        }
    }
    int i = -1;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        i++;
        if (i == index) {
            result_2 = true;
            USER::User::onlineUsers.erase(iter);
            break;
        }
    }
    return result_1 && result_2;
}

void Thread_run::send_text(int &sockfd, string &text, string &send_to, string &recv_from) {
    int i = -1;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        i++;
        if (send_to == (*iter).getUsername()) {
            break;
        }
    }

    int index = -1;
    int to_sockfd = 0;
    for (auto iter = USER::User::onlineSockets.begin(); iter != USER::User::onlineSockets.end(); iter++) {
        index++;
        if (index == i) {
            to_sockfd = (*iter);
            break;
        }
    }

    if (to_sockfd > 0) {
        Json::Value msg;
        Json::Value attr;
        attr["type"] = "send_text";
        attr["text"] = text;
        attr["send_to"] = send_to;
        attr["recv_from"] = recv_from;
        msg.append(attr);
        Json::FastWriter writer;
        std::string msgJson = writer.write(msg);
        cout << msgJson << endl;

        socket->socket_send(to_sockfd, msgJson.c_str());
    } else {
        cout << send_to << " is not online" << endl;
    }
}

void Thread_run::send_file(int &sockfd, string &send_to, string &recv_from) {
    int i = -1;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        i++;
        if (send_to == (*iter).getUsername()) {
            break;
        }
    }

    int index = -1;
    int to_sockfd = 0;
    for (auto iter = USER::User::onlineSockets.begin(); iter != USER::User::onlineSockets.end(); iter++) {
        index++;
        if (index == i) {
            to_sockfd = (*iter);
            break;
        }
    }


    if (to_sockfd > 0) {
        char recv_buf[4096];
        memset(recv_buf, '\0', 4096);
        socket->socket_recv(sockfd, recv_buf);

        socket->socket_send(to_sockfd, recv_buf);

        cout << "finish thansfering file to " << to_sockfd << endl;

    } else {
        cout << send_to << " is not online" << endl;
        char buff[MAXLINE];
        while (read(sockfd, buff, MAXLINE) > 0);
    }

    EPOLL::Epoll::online_service->event_add(sockfd, EPOLLIN | EPOLLET);
    cout << sockfd << " has been joined in epoll again" << endl;
}

void Thread_run::check_online(int &sockfd, string &friend_username) {
    bool flag = false;
    for (auto iter = USER::User::onlineUsers.begin(); iter != USER::User::onlineUsers.end(); iter++) {
        if (friend_username == (*iter).getUsername()) {
            flag = true;
            break;
        }
    }
    if (flag) {
        socket->socket_send(sockfd, "on");
    } else {
        socket->socket_send(sockfd, "off");
    }

}


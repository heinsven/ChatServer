#include "User.h"

USER::User::User(const std::string &username, const std::string &password, const std::string &nickname,
                 const std::string &status) : username(username), password(password), nickname(nickname),
                                              status(status) {
    userInfo = new UserInfo();
}

USER::User::User() {
    userInfo = new UserInfo();
}


const std::string &USER::User::getUsername() const {
    return username;
}

void USER::User::setUsername(const std::string &username) {
    User::username = username;
}

const std::string &USER::User::getPassword() const {
    return password;
}

void USER::User::setPassword(const std::string &password) {
    User::password = password;
}

const std::string &USER::User::getNickname() const {
    return nickname;
}

void USER::User::setNickname(const std::string &nickname) {
    User::nickname = nickname;
}

const std::string &USER::User::getStatus() const {
    return status;
}

void USER::User::setStatus(const std::string &status) {
    User::status = status;
}

USER::User *USER::User::getOnlineFriend(std::string username) {
    USER::User *user = nullptr;
    std::list<USER::User>::iterator iter = onlineUsers.begin();
    for (iter; iter != onlineUsers.end(); iter++) {
        if ((*iter).getUsername() == username) {
            user = &(*iter);
        }
    }
    return user;
}

void USER::User::addOnlineFriend(int socket, USER::User user) {
//    userInfo->lock();
    userInfo->friends.push_back(user);
    userInfo->friendsSockets.push_back(socket);
//    userInfo->notify();
//    userInfo->unlock();
}

void USER::User::removeOnlineFriend(USER::User user) {
//    userInfo->lock();
//    if (userInfo->friends.size() == 0) {
//        userInfo->wait();
//    }
    int index = (*this)[user];
    if (index >= 0) {
        userInfo->friends.remove(user);
        std::list<int>::iterator iter = userInfo->friendsSockets.begin();
        int i = -1;
        for (iter; i < index; iter++) {
            i++;
            if (index == i) {
                userInfo->friendsSockets.remove((*iter));
            }
        }
    }
//    userInfo->notify();
//    userInfo->unlock();

}

bool USER::User::operator==(const USER::User &user) {
    return (getUsername() == user.getUsername());
}

int USER::User::operator[](User user) {
//    userInfo->lock();
    std::list<User>::iterator iter = userInfo->friends.begin();
    int index = -1;
    for (iter; iter != userInfo->friends.end(); iter++) {
        index++;
        if (*iter == user) {
            break;
        }
    }
//    userInfo->unlock();
    return index;
}

USER::User::~User() {
//    delete userInfo;
}

pthread_mutex_t USER::User::mutex;
pthread_cond_t USER::User::cond;
std::list<USER::User> USER::User::onlineUsers;
std::list<int> USER::User::onlineSockets;

void USER::User::initSignal() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}


void USER::User::addOnlineUsers(int socket, User user) {
    pthread_mutex_lock(&mutex);

    onlineSockets.push_back(socket);
    onlineUsers.push_back(user);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void USER::User::removeOnlineUsers(User user) {

    pthread_mutex_lock(&mutex);
    if (onlineUsers.size() <= 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    std::list<USER::User>::iterator iter = onlineUsers.begin();
    int index = -1;
    for (iter; iter != onlineUsers.end(); iter++) {
        index++;
        if (*iter == user) {
            onlineUsers.erase(iter);
            onlineSockets.remove(index);
             break;
        }
    }
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

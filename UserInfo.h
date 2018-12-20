#ifndef CHAT_USERINFO_H
#define CHAT_USERINFO_H

#include <iostream>
#include <list>
#include <zconf.h>
#include <pthread.h>
#include "User.h"

namespace USER {

    class User;

}


struct UserInfo {

    UserInfo();

    ~UserInfo();

    void lock();

    void unlock();

    void wait();

    void notify();

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    std::list<int> friendsSockets;
    std::list<USER::User> friends;
};

#endif //CHAT_USERINFO_H

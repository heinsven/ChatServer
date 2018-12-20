#include "UserInfo.h"

UserInfo::UserInfo() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

void UserInfo::lock() {
    pthread_mutex_lock(&mutex);
}

void UserInfo::unlock() {
    pthread_mutex_unlock(&mutex);
}

void UserInfo::wait() {
    pthread_cond_wait(&cond, &mutex);
}

void UserInfo::notify() {
    pthread_cond_broadcast(&cond);
}

UserInfo::~UserInfo() {
}

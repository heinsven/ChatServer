
#ifndef CHAT_CONTROL_H
#define CHAT_CONTROL_H

#include <zconf.h>
#include <pthread.h>
#include <iostream>
#include <queue>
#include <list>

struct Control {
    Control();

    void lock();

    void unlock();

    void wait();

    void notify();

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    std::queue<int> work_queue;
    std::list<int> online_client;
};

#endif //CHAT_CONTROL_H

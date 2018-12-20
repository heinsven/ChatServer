#include "Factory.h"

using namespace FACTORY;

Thread_run *FACTORY::Factory::getHandle(){
    return handle;
}

void LoginFactory::on() {
    if (start_flag == 1) {
        return;
    }
    start_flag == 1;
    for (int index = 0; index != THREADNUM; index++) {
        start(handle);
    }
}

pthread_t LoginFactory::start(Thread_run *arg) {
    arg->set_info(info);
    pthread_t tid = getpid();
    pthread_create(&tid, NULL, thread_func, (void *) arg);
    return tid;
}

void OnlineFactory::on() {
    if (start_flag == 1) {
        return;
    }
    start_flag == 1;
    for (int index = 0; index != THREADNUM; index++) {
        start(handle);
    }
}

pthread_t OnlineFactory::start(Thread_run *arg) {
    arg->set_info(info);
    pthread_t tid = getpid();
    pthread_create(&tid, NULL, thread_func, (void *) arg);
    return tid;
}

void RegisterFactory::on() {
    if (start_flag == 1) {
        return;
    }
    start_flag == 1;
    for (int index = 0; index != THREADNUM; index++) {
        start(handle);
    }
}

pthread_t RegisterFactory::start(Thread_run *arg) {
    arg->set_info(info);
    pthread_create(&tid, NULL, thread_func, (void *) arg);
    return tid;
}

void CheckOnlineFactory::on() {
    if (start_flag == 1) {
        return;
    }
    start_flag == 1;
    for (int index = 0; index != THREADNUM; index++) {
        start(handle);
    }
}

pthread_t CheckOnlineFactory::start(Thread_run *arg) {
    arg->set_info(info);
    pthread_create(&tid, NULL, thread_func, (void *) arg);
    return tid;
}
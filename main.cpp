#include "Epoll.h"
#include "User.h"
#include "Thread_run.h"

static void *thread_register(void *obj) {
    pthread_detach(pthread_self());
    EPOLL::Epoll *register_service = new EPOLL::RegisterEpoll("5301");
    register_service->epoll_listen();
}

static void *thread_login(void *obj) {
    pthread_detach(pthread_self());
    EPOLL::Epoll *login_service = new EPOLL::LoginEpoll("5302");
    login_service->epoll_listen();
}

static void *thread_online(void *obj) {
    USER::User::initSignal();
    EPOLL::Epoll::online_service = new EPOLL::OnlineEpoll("5303");
    EPOLL::Epoll::online_service->epoll_listen();
}

static void *thread_check_online(void *obj) {
    USER::User::initSignal();
    EPOLL::Epoll *check_online_service = new EPOLL::CheckOnlineEpoll("5304");
    check_online_service->epoll_listen();
}

int main(int argc, char **argv) {

    pthread_t tid = getpid();
    pthread_create(&tid, NULL, thread_register, NULL);
    pthread_create(&tid, NULL, thread_login, NULL);
    pthread_create(&tid, NULL, thread_online, NULL);
    pthread_create(&tid, NULL, thread_check_online, NULL);

    while (1);

    return 0;
}


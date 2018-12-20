
#ifndef CHAT_FACTORY_H
#define CHAT_FACTORY_H


#include "Thread_run.h"

namespace FACTORY {
    class Factory {

    public:
        friend class EPOLL::Epoll;

        friend class EPOLL::LoginEpoll;

        friend class EPOLL::OnlineEpoll;

        friend class EPOLL::RegisterEpoll;

        friend class EPOLL::CheckOnlineEpoll;

        Factory(SOCKET::Socket *socket) : info(new Control()), handle(new Thread_run(socket)) {
        }

        virtual void on() {

        };

        virtual pthread_t start(Thread_run *arg) {

        };


        Thread_run *getHandle();

    protected:
        pthread_t tid;
        Control *info;
        Thread_run *handle;
    };

    class RegisterFactory : public Factory {

    public:


        RegisterFactory(SOCKET::Socket *socket) : Factory(socket) {
            start_flag = 0;
        }

        void on();

        pthread_t start(Thread_run *arg);

        static void *thread_func(void *obj) {
            pthread_detach(pthread_self());
            Thread_run *obj1 = (Thread_run *) obj;
            obj1->run_register();
        }

    private:
        int start_flag;
        int THREADNUM = 1;
    };


    class LoginFactory : public Factory {
    public:


        LoginFactory(SOCKET::Socket *socket) : Factory(socket) {
            start_flag = 0;
        }

        static void *thread_func(void *obj) {
            pthread_detach(pthread_self());
            Thread_run *obj1 = (Thread_run *) obj;
            obj1->run_login();
        }

        virtual void on();

        virtual pthread_t start(Thread_run *arg);

    private:
        int start_flag;
        int THREADNUM = 3;
    };

    class OnlineFactory : public Factory {
    public:

        OnlineFactory(SOCKET::Socket *socket) : Factory(socket) {
            start_flag = 0;
        }

        static void *thread_func(void *obj) {
            pthread_detach(pthread_self());
            Thread_run *obj1 = (Thread_run *) obj;
            obj1->run_online();
        }

        virtual void on();

        virtual pthread_t start(Thread_run *arg);

    private:
        int start_flag;
        int THREADNUM = 5;
    };


    class CheckOnlineFactory : public Factory {
    public:

        CheckOnlineFactory(SOCKET::Socket *socket) : Factory(socket) {
            start_flag = 0;
        }

        static void *thread_func(void *obj) {
            pthread_detach(pthread_self());
            Thread_run *obj1 = (Thread_run *) obj;
            obj1->run_check_online();
        }

        virtual void on();

        virtual pthread_t start(Thread_run *arg);

    private:
        int start_flag;
        int THREADNUM = 5;
    };

}


#endif //CHAT_FACTORY_H

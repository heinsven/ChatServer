#include "Control.h"

Control::Control() {
    mutex = PTHREAD_MUTEX_INITIALIZER;
    cond = PTHREAD_COND_INITIALIZER;
//    pthread_mutex_init(&mutex, NULL);
//    pthread_cond_init(&cond, NULL);
}

void Control::lock() {
    pthread_mutex_lock(&mutex);
}

void Control::unlock() {
    pthread_mutex_unlock(&mutex);
}

void Control::wait() {
    pthread_cond_wait(&cond, &mutex);
}

void Control::notify() {
    pthread_cond_broadcast(&cond);
}


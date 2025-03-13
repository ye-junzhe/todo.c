#include <stdlib.h>
#include <unistd.h>
#include "thread.h"
#include "log.h"

pthread_t thread_logging_info_clear;
pthread_mutex_t mutex;
pthread_cond_t cond;
ThreadController thread_logging_info_clear_controller;

void thread_logging_info_init() {
    thread_logging_info_clear_controller.running = 1;
    thread_logging_info_clear_controller.got_a_job = 0;
    thread_logging_info_clear_controller.cool_down = 5;
}

void checkShouldClear() {
    while (thread_logging_info_clear_controller.running) {
        if (thread_logging_info_clear_controller.got_a_job && thread_logging_info_clear_controller.cool_down == 0) {
            CLEAR;
            printDashBoard();
            pthread_mutex_lock(&mutex);
            thread_logging_info_clear_controller.got_a_job = 0;
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_lock(&mutex);
            while (!thread_logging_info_clear_controller.got_a_job) {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
            thread_logging_info_clear_controller.cool_down = 0;
            sleep(5);
        }
    }
}

void * loggingInfoClear_ThreadJob(void * param) {
    checkShouldClear();
    return NULL;
}

void threadJoin() {
    pthread_mutex_lock(&mutex);
    thread_logging_info_clear_controller.running = 0;
    thread_logging_info_clear_controller.got_a_job = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    pthread_join(thread_logging_info_clear, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}


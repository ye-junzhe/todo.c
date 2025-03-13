#pragma once

#include <unistd.h>
#include <pthread.h>

typedef struct {
    int running;
    int got_a_job;
    int cool_down;
} ThreadController;

void thread_logging_info_init();
void * loggingInfoClear_ThreadJob(void * param);
void threadJoin();

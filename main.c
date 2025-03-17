#include <stdlib.h>

#include "file.h"
#include "cursor.h"
#include "thread.h"
#include "util.h"

// NOTE: Had an idea about telling the thread to do a bunch of things
// after waking up. Now it's just checking a variable to see what to do.
// Maybe use something like an instruction char* pointer?

// void checkShouldClear() {
//     while (thread_logging_info_clear_controller.running) {

//         // count_down == 0 means that during the sleep time,
//         // there is no 'c' being pressed
//         if (thread_logging_info_clear_controller.got_a_job && thread_logging_info_clear_controller.count_down == 0) {
//             CLEAR;
//             printDashBoard();
//             pthread_mutex_lock(&mutex);
//             thread_logging_info_clear_controller.got_a_job = 0;
//             pthread_mutex_unlock(&mutex);
//         } else {
//             pthread_mutex_lock(&mutex);
//             while (!thread_logging_info_clear_controller.got_a_job) {
//                 pthread_cond_wait(&cond, &mutex);
//             }
//             pthread_mutex_unlock(&mutex);

//             thread_logging_info_clear_controller.count_down = 0;
//             sleep(5); // clear the screen after 5 seconds
//             checkShouldClear();
//         }
//     }
// }

// Define global variables
TodoFilePaths paths = {NULL, NULL};
Cursor cursor = {0, 0};
pthread_t thread_logging_info_clear;
pthread_mutex_t mutex;
pthread_cond_t cond;
ThreadController thread_logging_info_clear_controller;

int main(int argc, char *argv[]) {
    #ifdef PATH
    printf("Starting to create file paths...\n");
    createFilePath(argv[1]);
    printf("File paths creation completed\n");
    #endif /* ifdef PATH */
    
    listenUserInput();
    return EXIT_SUCCESS;
}

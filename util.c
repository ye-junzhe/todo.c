#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "util.h"
#include "todo.h"
#include "thread.h"
#include "log.h"
#include "cursor.h"
#include "file.h"

extern int todo_count;
extern Todo* todos;
extern TodoFilePaths paths;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_t thread_logging_info_clear;
extern ThreadController thread_logging_info_clear_controller;

char userChoice;

void cleanMem() {
    for (int i = 0; i < todo_count; i++) {
        free(todos[i].todo_info);
    }
    free(todos);
    free(paths.todoFilePath);
    free(paths.tempFilePath);
    todos = NULL;
    paths.todoFilePath = NULL;
    paths.tempFilePath = NULL;
}

void listenUserInput() {

    /*
        Init
     */
    readTodos();

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    disableEcho();

    CLEAR;

    printHelp();

    if (pthread_create(&thread_logging_info_clear, NULL, loggingInfoClear_ThreadJob, NULL) != 0) {
        perror("Failed creating thread\n");
        exit(EXIT_FAILURE);
    } else {
        thread_logging_info_clear_controller.running = 1;
    }

    /*
        Listen user input
     */
    while (1) {

        printDashBoard();

        fflush(stdin);
        read(STDIN_FILENO, &userChoice, 1);

        switch (userChoice) {
            case 'c':
                createTodo();
                CLEAR;
                break;
            case 's':
                displayTodos();
                CLEAR;
                break;
            case 'r':
                renameTodo();
                CLEAR;
                break;
            case 'x':
                markDoneTodo();
                CLEAR;
                break;
            case 'h':
                CLEAR;
                printHelp();
                break;
            case 'k':
                CLEAR;
                arrowUp();
                break;
            case 'j':
                CLEAR;
                arrowDown();
                break;
            case 'q':
                printf("Quitting\n");
                enableEcho();
                threadJoin();
                cleanMem();
                return;
            default:
                printf("%c", userChoice);
                printDefaultMessage();
                CLEAR;
            }
    }
}


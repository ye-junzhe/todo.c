#include <stdio.h>
#include <unistd.h>
#include "log.h"

struct termios oldt, newt;

Logger logger = {"Do Something!\n", 0, 0, 0, 0};

/*
    Disable line buffering and echo
 */
void disableEcho() {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enableEcho() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

/*
    Logging methods for the Logger
*/
void load_log_info(char *info) {
    logger.log_info = info;
}

void log_info() {
    if LOG printf("%s", logger.log_info);
}

/*
    Display todos by refreshing the UI
 */
void printDashBoard() {
    printf(
        "                      d8b                                               d8b         \n"
        "   d8P                88P                        d8P                    88P         \n"
        "d888888P             d88                      d888888P                 d88          \n"
        "  ?88'   d8888b  d888888   d8888b  .d888b,      ?88'   d8888b      d888888   d8888b \n"
        "  88P   d8P' ?88d8P' ?88  d8P' ?88 ?8b,         88P   d8P' ?88    d8P' ?88  d8P' ?88\n"
        "  88b   88b  d8888b  ,88b 88b  d88   `?8b       88b   88b  d88    88b  ,88b 88b  d88\n"
        "  `?8b  `?8888P'`?88P'`88b`?8888P'`?888P'       `?8b  `?8888P'    `?88P'`88b`?8888P \n"
        "                                                                                    \n"
        "                                                                                    \n"
        "                                                                                    \n"
    );

    displayTodos();

    log_info();
    logger.isDone = 0;
    logger.isCreated = 0;
    logger.isNotAvailableOp = 0;
    logger.isRenamed = 0;
}

void printHelp() {
    printf("c => create\nk => up\nj => down\nr => rename\ns => refresh\nx => mark done\nq => quit\n");
}

void printDefaultMessage() {
    logger.isNotAvailableOp = 1;
    load_log_info("Not an available option\n");
}


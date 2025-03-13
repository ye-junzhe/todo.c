#pragma once

#include <termios.h>
#include <stdlib.h>

#ifdef RELEASE
#define CLEAR system("clear")
#else
#define CLEAR
#endif

#define LOG ( logger.isDone || logger.isCreated || logger.isNotAvailableOp || logger.isRenamed )

typedef struct {
    char* log_info;
    int isCreated,
        isDone,
        isNotAvailableOp,
        isRenamed;
} Logger;

void disableEcho();
void enableEcho();

void displayTodos();
void load_log_info(char* info);
void log_info();
void printDashBoard();

void checkShouldClear();

void printHelp();
void printDefaultMessage();

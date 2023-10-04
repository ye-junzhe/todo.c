#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define RELEASE

#ifdef RELEASE
#define CLEAR system("clear")
#else
#define CLEAR
#endif

typedef enum { PENDING, COMPLETE } Status;

typedef struct {
    char *todo_info;
    Status status;
} Todo;

typedef struct {
    char *log_info;
    int isCreated,
        isDone,
        isNotAvailableOp,
        isRenamed;
} Logger;

int todo_count = 0;
int previous_cursor = 0;
int next_cursor = 0;
Todo *todos = NULL;

Logger logger = {"Do Something!\n", 0, 0, 0, 0};

struct termios oldt, newt;

void disableEcho() {
    // Disable line buffering and echo
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void enableEcho() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void load_log_info(char *info) {
    logger.log_info = info;
}

void log_info() {
    if (logger.isDone ||
        logger.isCreated ||
        logger.isNotAvailableOp ||
        logger.isRenamed)
        printf("%s", logger.log_info);
}

void displayTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
}

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

    refreshUI();
    log_info();
    logger.isDone = 0;
    logger.isCreated = 0;
    logger.isNotAvailableOp = 0;
    logger.isRenamed = 0;
}

void concatArrow(int target) {

    for(int i = 0;; i++) {
        if(todos[target].todo_info[i] == '\n') {
            todos[target].todo_info[i] = ' ';
            break;
        }
    }

    previous_cursor = next_cursor;
    next_cursor = target; // cursor here is the index of the new todo

    todos[next_cursor].todo_info = strcat(todos[next_cursor].todo_info, "<-\n");

    if (todo_count == 0 || previous_cursor > todo_count - 1 || next_cursor == previous_cursor) return;

    int length = strlen(todos[previous_cursor].todo_info);
    char modifiedStr[length - 2];
    strncpy(modifiedStr, todos[previous_cursor].todo_info, length - 2);
    modifiedStr[length - 4] = '\n';
    modifiedStr[length - 3] = '\0';
    strcpy(todos[previous_cursor].todo_info, modifiedStr);
}

void arrowUp() {
    if (next_cursor - 1 < 0) return;

    concatArrow(next_cursor - 1);
}

void arrowDown() {
    if (next_cursor + 1 >= todo_count) return;

    concatArrow(next_cursor + 1);
}

void createTodo() {
    char *line = NULL;
    size_t linecap = 0;

    printf("Creating todo: => ");

    // fseek(stdin, 0, SEEK_END);
    fflush(stdin);

    enableEcho();
    getline(&line, &linecap, stdin);
    disableEcho();

    Todo todo;
    todo.todo_info = (char *)malloc(strlen(line) + 1 + 3);
    strcpy(todo.todo_info, line);

    todo.status = PENDING;

    todos = (Todo *)realloc(todos, (todo_count + 1) * sizeof(Todo));
    todos[todo_count] = todo;
    concatArrow(todo_count);
    todo_count++;

    load_log_info("New todo created\n");
    logger.isCreated = 1;
}

void renameTodo() {
    if (todo_count == 0) {
        load_log_info("No Todos yet\n");
        return;
    }

    char *line = NULL;
    size_t linecap = 0;

    printf("Renaming todo: => ");

    fflush(stdin);
    enableEcho();
    getline(&line, &linecap, stdin);
    disableEcho();

    todos[next_cursor].todo_info = (char *)realloc(todos[next_cursor].todo_info, strlen(line));
    strcpy(todos[next_cursor].todo_info, line);

    for(int i = 0;; i++) {
        if(todos[next_cursor].todo_info[i] == '\n') {
            todos[next_cursor].todo_info[i] = ' ';
            break;
        }
    }
    todos[next_cursor].todo_info = strcat(todos[next_cursor].todo_info, "<-\n");

    load_log_info("Todo Renamed\n");
    logger.isRenamed = 1;
}

void markDoneTodo() {
    if (todo_count == 0) {
        load_log_info("No Todos yet\n");
        return;
    }

    // TODO: Show todos that are already down, just declare a new Todo struct, and put this COMPLETE todo into it
    // It acts as exatly the same as the original todo!
    // It will log every time
    // And maybe we can move the cursor right and left
    todos[next_cursor].status = COMPLETE;

    for (int i = next_cursor; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count--;
    printf("%d\n", todo_count);

    if (todo_count == 0) return;
    if (next_cursor == todo_count) // cursor at the highest todo
        concatArrow(next_cursor - 1);
    else concatArrow(next_cursor);

    load_log_info("Todo done!\n");
    logger.isDone = 1;
}

void printHelp() {
    printf("c => create\nk => up\nj => down\nr => rename\ns => refresh\nx => mark done\nq => quit\n");
}

void cleanMem() {
    free(todos);
}

void listenUserInput() {

    disableEcho();

    CLEAR;

    char userInput;

    printHelp();
    while (1) {
        printDashBoard();

        fflush(stdin);
        scanf("%c", &userInput);

        switch (userInput) {
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
                cleanMem();
                return;
            default:
                logger.isNotAvailableOp = 1;
                printf("%c", userInput);
                load_log_info("Not an available option\n");
                CLEAR;
        }
    }
}

int main(int argc, char *argv[]) {
    listenUserInput();
    return EXIT_SUCCESS;
}

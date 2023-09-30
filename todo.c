#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RELEASE

#ifdef RELEASE
#define CLEAR system("clear")
#else
#define CLEAR
#endif

#define ASCII_OFFSET_DIGIT 48

typedef enum {
    PENDING,
    COMPLETE
} Status;

typedef struct {
    char *todo_info;
    Status status;
} Todo;

typedef struct {
    char *log_info;
    int isCreated;
    int isDone;
    int isNotAvailableOp;
} Logger;

int todo_count = 0;
Todo *todos = NULL;

Logger logger = {"Do Something!\n", 0, 0, 0};

void load_log_info(char *info) {
    logger.log_info = info;
}

void print_log_info() {
    if (logger.isDone ||
        logger.isCreated ||
        logger.isNotAvailableOp)
        printf("%s", logger.log_info);
}

void createTodo() {
    char *line = NULL;
    size_t linecap = 0;

    printf("Creating todo: => ");

    // fseek(stdin, 0, SEEK_END);
    fflush(stdin);
    getline(&line, &linecap, stdin);

    Todo todo;
    todo.todo_info = (char *)malloc(strlen(line) + 1);
    strcpy(todo.todo_info, line);

    todo.status = PENDING;

    todos = (Todo *)realloc(todos, (todo_count + 1) * sizeof(Todo));
    todos[todo_count] = todo;
    todo_count++;

    load_log_info("New todo created\n");
    logger.isCreated = 1;
}

void printTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
}

void markDoneTodo() {
    if (todo_count == 0) {
        load_log_info("No Todos yet\n");
        return;
    }

    printf("Which todo you wish to remove ? => ");
    fflush(stdin);
    int which_one = fgetc(stdin) - ASCII_OFFSET_DIGIT;

    if (which_one > todo_count || which_one <= 0) {
        load_log_info("No Todo has that index\n");
        return;
    }

    for (int i = which_one - 1; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count --;

    load_log_info("Todo done!\n");
    logger.isDone = 1;
}

void printHelp() {
    printf("c => create\ns => show\nd => mark done\nq => quit\n");
}

void cleanMem() {
    free(todos);
}

void listenUserInput() {

    system("clear");

    char userInput;

    printHelp();
while (1) { printf(
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

        printTodos();
        print_log_info();
        logger.isDone = 0;
        logger.isCreated = 0;
        logger.isNotAvailableOp = 0;

        fflush(stdin);
        scanf("%c", &userInput);

        switch (userInput) {
            case 'c':
                createTodo();
                CLEAR;
                break;
            case 's':
                printTodos();
                CLEAR;
                break;
            case 'd':
                markDoneTodo();
                CLEAR;
                break;
            case 'h':
                printHelp();
                CLEAR;
                break;
            case 'q':
                printf("Quitting\n");
                cleanMem();
                return;
            default:
                logger.isNotAvailableOp = 1;
                load_log_info("Not an available option\n");
                CLEAR;
        }
    }
}

int main(int argc, char *argv[])
{
    listenUserInput();
    return EXIT_SUCCESS;
}

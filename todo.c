#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RELEASE

typedef enum {
    PENDING,
    COMPLETE
} Status;

typedef struct {
    char *todo_info;
    Status status;
} Todo;

int todo_count = 0;
Todo *todos = NULL;

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

    printf("New todo added: %s", line);
}

void printTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
}

void markDoneTodo() {
    if (todo_count == 0) {
        printf("No Todos yet\n");
        return;
    }

    int which_one;
    printf("Which todo you wish to remove ? => ");
    scanf("%d", &which_one);

    if (!isdigit(which_one)) {
        printf("Not a digit");
        return;
    }

    if (which_one > todo_count || which_one <= 0) {
        printf("No Todo has that index\n");
        return;
    } 

    for (int i = which_one - 1; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count --;
}

void cleanMem() {
    free(todos);
}

void listenUserInput() {
    system("clear");
    char userInput;
    while (1) {
        printf("c => create\ns => show\nd => mark done\nq => quit\n");
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
        printTodos();
        fflush(stdin);
        scanf("%c", &userInput);
        switch (userInput) {
            case 'c':
                createTodo();
                #ifdef RELEASE
                system("clear");
                #endif /* ifdef  RELEASE */
                break;
            case 's':
                printTodos();
                #ifdef RELEASE
                system("clear");
                #endif /* ifdef  RELEASE */
                break;
            case 'd':
                markDoneTodo();
                #ifdef RELEASE
                system("clear");
                #endif /* ifdef  RELEASE */
                break;
            case 'q':
                printf("Quitting\n");
                cleanMem();
                return;
            default:
                printf("Not an available option\n");
        }
    }
}

int main(int argc, char *argv[])
{
    listenUserInput();
    return EXIT_SUCCESS;
}

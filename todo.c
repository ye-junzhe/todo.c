#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    Todo todo;

    char buffer[80];
    printf("Creating todo\nEnter todo info: ");
    scanf("%s", buffer); // TODO: read line, not just one word

    todo.todo_info = (char *)malloc(strlen(buffer) + 1); // TODO: malloc according to the input len
    strncpy(todo.todo_info, buffer, sizeof(buffer));

    todo.status = PENDING;

    todos = (Todo *)realloc(todos, (todo_count+1)*sizeof(Todo));
    todos[todo_count] = todo;

    todo_count ++;

    // printf("TODO Status: %d,\nTODO Info: %s,\nThis is the NO.%d todo\n", todo.status, todo.todo_info, todo_count);
}

// TODO: Clear screen
void printTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
}

void markDoneTodo() {
    int which_one;
    printf("Which todo you wish to remove ?\n");
    scanf("%d", &which_one);
    for (int i = which_one - 1; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count --;
}

void listenUserInput() {
    char userInput;
    while (1) {
        printf("c => create\ns => show\nd => mark done\nq => quit\n: ");
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
        scanf(" %c", &userInput);
        switch (userInput) {
            case 'c':
                createTodo();
                break;
            case 's':
                printTodos();
                break;
            case 'd':
                markDoneTodo();
                break;
            case 'q':
                printf("Quitting\n");
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

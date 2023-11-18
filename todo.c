#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#ifdef RELEASE
#define CLEAR system("clear")
#else
#define CLEAR
#endif

#define LOG ( logger.isDone || logger.isCreated || logger.isNotAvailableOp || logger.isRenamed )

typedef enum { PENDING, COMPLETE } Status;

typedef struct {
    char* todo_info;
    Status status;
} Todo;

typedef struct {
    char* log_info;
    int isCreated,
        isDone,
        isNotAvailableOp,
        isRenamed;
} Logger;

typedef struct {
    int previous_cursor;
    int next_cursor;
} Cursor;

typedef struct {
    int running;
    int got_a_job;
    int count_down;
} ThreadController;

typedef struct{
    char* todoFilePath;
    char* tempFilePath;
}TodoFilePaths ;

/*
        Initializing
*/
/* ========================= */
Cursor cursor = {0, 0};

int todo_count = 0;

Todo* todos = NULL;

Logger logger = {"Do Something!\n", 0, 0, 0, 0};

pthread_t thread_logging_info_clear;
pthread_mutex_t mutex;
pthread_cond_t cond;
ThreadController thread_logging_info_clear_controller;

char userChoice;

TodoFilePaths paths = {};

struct termios oldt, newt;
/* ========================= */

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
    Display todos by refreshing the UI
 */
void displayTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
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

void thread_logging_info_init() {
    thread_logging_info_clear_controller.running = 1;
    thread_logging_info_clear_controller.got_a_job = 0;
    thread_logging_info_clear_controller.count_down = 5;
}

// NOTE: Had a idea about telling the thread to do a bunch of things
// after waking up. Now it's just checking a variable to see what to do.
// Maybe use something like a instruction char* pointer?

void checkShouldClear() {
    while (thread_logging_info_clear_controller.running) {

        // count_down == 0 means that during the sleep time,
        // there is no 'c' being pressed
        if (thread_logging_info_clear_controller.got_a_job && thread_logging_info_clear_controller.count_down == 0) {
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

            thread_logging_info_clear_controller.count_down = 0;
            sleep(5); // clear the screen after 5 seconds
            checkShouldClear();
        }
    }
}

void * loggingInfoClear_ThreadJob(void * param) {
    checkShouldClear();
    return NULL;
}

void concatArrow(int target) {

    for(int i = 0;; i++) {
        if(todos[target].todo_info[i] == '\n') {
            todos[target].todo_info[i] = ' ';
            break;
        }
    }

    cursor.previous_cursor = cursor.next_cursor;
    cursor.next_cursor = target; // cursor here is the index of the new todo

    todos[cursor.next_cursor].todo_info = strcat(todos[cursor.next_cursor].todo_info, "<-\n");

    if (todo_count == 0 || cursor.previous_cursor > todo_count - 1 || cursor.next_cursor == cursor.previous_cursor) return;

    int length = strlen(todos[cursor.previous_cursor].todo_info);
    char modifiedStr[length - 2];
    strncpy(modifiedStr, todos[cursor.previous_cursor].todo_info, length - 2);
    modifiedStr[length - 4] = '\n';
    modifiedStr[length - 3] = '\0';
    strcpy(todos[cursor.previous_cursor].todo_info, modifiedStr);
}

void arrowUp() {
    if (cursor.next_cursor - 1 < 0) return;

    concatArrow(cursor.next_cursor - 1);
}

void arrowDown() {
    if (cursor.next_cursor + 1 >= todo_count) return;

    concatArrow(cursor.next_cursor + 1);
}

#ifdef PATH
TodoFilePaths createFilePath(char* base) {

    base = PATH;

    paths.todoFilePath = (char *) malloc(1 + strlen(base)+ strlen("/todos.txt") );
    paths.tempFilePath = (char *) malloc(1 + strlen(base)+ strlen("/todos_tmp.txt") );

    strcpy(paths.todoFilePath, base);
    strcat(paths.todoFilePath, "/todos.txt");
    strcpy(paths.tempFilePath, base);
    strcat(paths.tempFilePath, "/todos_tmp.txt");

    return paths;
}
#endif /* ifdef PATH */

void createTodo() {

    thread_logging_info_init();

    char* line = NULL;
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

    FILE* fptr = fopen(paths.todoFilePath, "a+");
    fputs(line, fptr);

    fclose(fptr);
    if (line)
        free(line);

    todo.status = PENDING;

    todos = (Todo *)realloc(todos, (todo_count + 1) * sizeof(Todo));
    todos[todo_count] = todo;
    concatArrow(todo_count);
    todo_count++;

    pthread_mutex_lock(&mutex);
    thread_logging_info_clear_controller.got_a_job = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    load_log_info("New todo created\n");
    logger.isCreated = 1;
}

void readTodos() {
    FILE* fptr = NULL;
    if (fopen(paths.todoFilePath, "r") != NULL)
        fptr = fopen(paths.todoFilePath, "r");
    else 
        fptr = fopen(paths.todoFilePath, "w"); // Create if todos.txt not exists
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    if (fptr == NULL) printf("Failed reading todos from %s\n", paths.todoFilePath);

    while ((read = getline(&line, &len, fptr)) != -1) {

        Todo todo;
        todo.todo_info = (char *)malloc(strlen(line) + 1 + 3);
        strcpy(todo.todo_info, line);

        todo.status = PENDING;

        todos = (Todo *)realloc(todos, (todo_count + 3) * sizeof(Todo));
        todos[todo_count] = todo;
        concatArrow(todo_count);
        todo_count++;

    }

    fclose(fptr);
    if (line)
        free(line);
}

void removeLine(
    FILE* src,
    FILE* temp,
    int cursor_pointing)
{
    char* line = NULL;
    size_t len = 0;
    int count = 1;

    while ((getline(&line, &len, src)) != -1) {
        if (cursor_pointing + 1 != count)
            fputs(line, temp);

        count++;
    }

    if (line)
        free(line);
}

void replaceLine(
    FILE* src,
    FILE* temp,
    int cursor_pointing,
    char* input_line)
{
    char* line = NULL;
    size_t len = 0;
    int count = 1;

    while ((getline(&line, &len, src)) != -1){

        if (cursor_pointing + 1 != count)
            fputs(line, temp);
        else
            fputs(input_line, temp);

        count++;
    }

    if (line)
        free(line);
}

void renameTodo() {

    thread_logging_info_init();

    if (todo_count == 0) {
        load_log_info("No Todos yet\n");
        return;
    }

    char* line = NULL;
    size_t linecap = 0;

    printf("Renaming todo: => ");

    fflush(stdin);
    enableEcho();
    getline(&line, &linecap, stdin);
    disableEcho();

    todos[cursor.next_cursor].todo_info = (char *)realloc(todos[cursor.next_cursor].todo_info, strlen(line));
    strcpy(todos[cursor.next_cursor].todo_info, line);

    // Rename the corresponding todo in todos.txt
    FILE* fptr = fopen(paths.todoFilePath, "r+");
    FILE* temp = fopen(paths.tempFilePath, "w+");
    replaceLine(fptr, temp, cursor.next_cursor, line);
    fclose(fptr);
    fclose(temp);
    remove(paths.todoFilePath);
    rename(paths.tempFilePath, "todos.txt");
    if (line)
        free(line);

    // Remove the \n of the new added todo
    for(int i = 0;; i++) {
        if(todos[cursor.next_cursor].todo_info[i] == '\n') {
            todos[cursor.next_cursor].todo_info[i] = ' ';
            break;
        }
    }
    todos[cursor.next_cursor].todo_info = strcat(todos[cursor.next_cursor].todo_info, "<-\n");

    pthread_mutex_lock(&mutex);
    thread_logging_info_clear_controller.got_a_job = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    load_log_info("Todo Renamed\n");
    logger.isRenamed = 1;
}

void markDoneTodo() {

    thread_logging_info_init();

    if (todo_count == 0) {
        load_log_info("No Todos yet\n");
        return;
    }

    todos[cursor.next_cursor].status = COMPLETE;

    for (int i = cursor.next_cursor; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count--;

    FILE* fptr = fopen(paths.todoFilePath, "r+");
    FILE* temp = fopen(paths.tempFilePath, "w+");
    removeLine(fptr, temp, cursor.next_cursor);
    fclose(fptr);
    fclose(temp);
    remove(paths.todoFilePath);
    rename(paths.tempFilePath, "todos.txt");

    if (todo_count == 0) return;
    if (cursor.next_cursor == todo_count) // cursor at the last todo
        concatArrow(cursor.next_cursor - 1);
    else concatArrow(cursor.next_cursor);

    pthread_mutex_lock(&mutex);
    thread_logging_info_clear_controller.got_a_job = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    load_log_info("Todo done!\n");
    logger.isDone = 1;
}

void printHelp() {
    printf("c => create\nk => up\nj => down\nr => rename\ns => refresh\nx => mark done\nq => quit\n");
}

void printDefaultMessage() {
    logger.isNotAvailableOp = 1;
    load_log_info("Not an available option\n");
}

void cleanMem() {
    free(todos);
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
                cleanMem();
                pthread_cond_signal(&cond);
                pthread_mutex_destroy(&mutex);
                pthread_cond_destroy(&cond);
                return;
            default:
                printf("%c", userChoice);
                printDefaultMessage();
                CLEAR;
        }
    }
}

int main(int argc, char *argv[]) {
    #ifdef PATH
    createFilePath(argv[1]);
    #endif /* ifdef PATH */
    listenUserInput();
    return EXIT_SUCCESS;
}

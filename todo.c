#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "todo.h"
#include "thread.h"
#include "log.h"
#include "cursor.h"
#include "file.h"

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern ThreadController thread_logging_info_clear_controller;
extern Logger logger;
extern TodoFilePaths paths;
extern Cursor cursor;

int todo_count = 0;
Todo* todos = NULL;

void displayTodos() {
    for (int i = 0; i < todo_count; i++) {
        printf("(%d). %s\n", i + 1, todos[i].todo_info);
    }
}

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
    // Try to open the file in append mode, which will create it if it doesn't exist
    FILE* fptr = fopen(paths.todoFilePath, "a+");
    if (fptr == NULL) {
        printf("Failed to open or create file: %s\n", paths.todoFilePath);
        perror("Error reason");
        return;
    }
    
    // Reset file pointer to the beginning, as a+ mode positions it at the end
    rewind(fptr);
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fptr)) != -1) {
        Todo todo;
        todo.todo_info = (char *)malloc(strlen(line) + 1 + 3);
        if (!todo.todo_info) {
            perror("Memory allocation failed");
            break;
        }
        strcpy(todo.todo_info, line);

        todo.status = PENDING;

        // Allocate the correct amount of memory
        todos = (Todo *)realloc(todos, (todo_count + 1) * sizeof(Todo));
        if (!todos) {
            perror("Memory reallocation failed");
            free(todo.todo_info);
            break;
        }
        todos[todo_count] = todo;
        concatArrow(todo_count);
        todo_count++;
    }

    fclose(fptr);
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

    todos[cursor.next_cursor].todo_info = (char *)realloc(todos[cursor.next_cursor].todo_info, strlen(line) + 4);
    strcpy(todos[cursor.next_cursor].todo_info, line);

    // Rename the corresponding todo in todos.txt
    FILE* fptr = fopen(paths.todoFilePath, "r+");
    FILE* temp = fopen(paths.tempFilePath, "w+");
    replaceLine(fptr, temp, cursor.next_cursor, line);
    fclose(fptr);
    fclose(temp);
    remove(paths.todoFilePath);
    rename(paths.tempFilePath, paths.todoFilePath);
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

    // Free the pointer pointing to the completed todo
    char* todo_done = todos[cursor.next_cursor].todo_info;

    todos[cursor.next_cursor].status = COMPLETE;

    for (int i = cursor.next_cursor; i <= todo_count - 1; i++) {
        todos[i] = todos[i + 1];
    }
    todo_count--;

    free(todo_done);

    FILE* fptr = fopen(paths.todoFilePath, "r+");
    FILE* temp = fopen(paths.tempFilePath, "w+");
    removeLine(fptr, temp, cursor.next_cursor);
    fclose(fptr);
    fclose(temp);
    remove(paths.todoFilePath);
    rename(paths.tempFilePath, paths.todoFilePath);

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


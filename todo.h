#pragma once

typedef enum { PENDING, COMPLETE } Status;

typedef struct {
    char* todo_info;
    Status status;
} Todo;

void createTodo();
void readTodos();
void renameTodo();
void markDoneTodo();

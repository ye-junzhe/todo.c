#pragma once

#include <string.h>
#include "todo.h"

extern int todo_count;
extern Todo* todos;

typedef struct {
    int previous_cursor;
    int next_cursor;
} Cursor;

extern Cursor cursor;

void concatArrow(int target);
void arrowUp();
void arrowDown();

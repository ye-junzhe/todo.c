#include "cursor.h"
#include "todo.h"

void concatArrow(int target) {
    for(int i = 0; i < strlen(todos[target].todo_info); i++) {
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


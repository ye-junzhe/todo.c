#include <stdlib.h>
#include <string.h>

#include "file.h"

extern TodoFilePaths paths;

#ifdef PATH
TodoFilePaths createFilePath(char* base) {
    base = PATH;

    // Print path for debugging
    printf("Using path: %s\n", base);

    paths.todoFilePath = (char *) malloc(1 + strlen(base) + strlen("/todos.txt"));
    paths.tempFilePath = (char *) malloc(1 + strlen(base) + strlen("/todos_tmp.txt"));

    if (!paths.todoFilePath || !paths.tempFilePath) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    strcpy(paths.todoFilePath, base);
    strcat(paths.todoFilePath, "/todos.txt");
    strcpy(paths.tempFilePath, base);
    strcat(paths.tempFilePath, "/todos_tmp.txt");

    // Print complete file paths for debugging
    printf("Todos file path: %s\n", paths.todoFilePath);
    printf("Temp file path: %s\n", paths.tempFilePath);

    return paths;
}
#endif /* ifdef PATH */

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

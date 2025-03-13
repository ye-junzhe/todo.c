#pragma once

#include <stdio.h>

typedef struct{
    char* todoFilePath;
    char* tempFilePath;
} TodoFilePaths ;

TodoFilePaths createFilePath(char* base);

void removeLine(FILE* src, FILE* temp, int cursor_pointing);
void replaceLine(FILE* src, FILE* temp, int cursor_pointing, char* input_line);

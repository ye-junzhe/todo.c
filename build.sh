#!/bin/bash

set -xe

# Modify here
# Example: ./
todos_txt_dir=~/Dev/Software/todo.c/

# Check if the path exists
mkdir -p "$todos_txt_dir"

cc -Wall main.c cursor.c file.c log.c thread.c todo.c util.c -o todo -DPATH=\"$todos_txt_dir\" -DRELEASE -pthread

./todo

#!bin/bash

set -xe

# Modify here
# Example: ./
todos_txt_dir=./

clang todo.c -o todo -DPATH=\"$todos_txt_dir\" -DRELEASE

./todo

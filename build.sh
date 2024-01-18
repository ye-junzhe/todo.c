#!bin/bash

set -xe

# Modify here
# Example: ./
todos_txt_dir=./

cc todo.c -o todo -DPATH=\"$todos_txt_dir\" -DRELEASE

./todo

#!bin/bash

set -xe

clang todo.c -o todo -DPATH=\"/Users/mcf4r/Dev/Software/todo.c\" -DRELEASE

./todo

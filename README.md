# todo.c

A simple CLI todo app, nothing fancy but really useful, it fits perfectly into your workflow if you are a CLI person.

![todo.c GIF](https://github.com/ye-junzhe/Images/blob/main/todo.c/todo.c.gif?raw=true)

## Install

1. Set the path in build.sh of todos.txt for storing todos(The absolute/relative path to that Dir)

2. `sh build.sh`

## Keybindings

```
j => Move down
k => Move up
c => Create todo
x => Done todo
r => Rename todo
s => Refresh
h => Help
q / ctrl+c => Quit
```

> Logging messages will clear themselves after 5 seconds, achieved by applying multithreading

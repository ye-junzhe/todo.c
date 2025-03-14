#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "./"
#define SRC_FOLDER "./"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
    nob_cmd_append(
        &cmd,
        "cc",
        "-Wall",
        "-Wextra",
        "-DPATH=\"$todos_txt_dir\"",
        "-DRELEASE",
        "-pthread",
        "-o",
        BUILD_FOLDER"todo",
        SRC_FOLDER"main.c", "file.c", "cursor.c", "log.c", "thread.c", "todo.c", "util.c"
    );
    if (!nob_cmd_run_sync(cmd)) return 1;

    return 0;
}

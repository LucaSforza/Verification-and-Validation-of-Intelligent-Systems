#define NOB_IMPLEMENTATION
#include "nob.h"

#if defined(__linux__)
#define GSL_INCLUDE_PATH "/usr/include/gsl"
#elif defined(__APPLE__)
#define GSL_INCLUDE_PATH "/opt/homebrew/include"
#else
#error "Unsupported platform"
#endif

#if defined(__linux__)
#define GSL_LIB_PATH "/usr/lib"
#elif defined(__APPLE__)
#define GSL_LIB_PATH "/opt/homebrew/lib"
#else
#error "Unsupported platform"
#endif

#define BUILD_FOLDER "build/"
#define THREAD_COUNT 2

typedef struct {
    const char *src_path;
    const char *exe_name;
} Target;

char *program_name = NULL;
bool debug = false;
Nob_Cmd cmd = {0};
Target targets[2] = {
    {
        .src_path = "minimize.c",
        .exe_name = "minimize"
    },
    {
        .src_path = "ode.c",
        .exe_name = "ode"
    }
};

bool parse_args(int argc, char **argv) {
    if(argc > 0) {
        char *arg = nob_shift(argv, argc);
        debug = true;
        if(strcmp(arg, "debug") != 0) {
            nob_log(NOB_ERROR, "Unrecognized argument %s, please use 'debug' instead.", arg);
            return false;
        }
    }
    return true;
}

Nob_Proc build_file(const char *file_path, const char *exe_name) {
    nob_cmd_append(&cmd, "gcc");
    nob_cmd_append(&cmd,"-Wall", "-Wextra", "-I" GSL_INCLUDE_PATH, "-L" GSL_LIB_PATH, "-I", "Utils/");
    if(debug) {
        nob_cmd_append(&cmd, "-ggdb");
    } else {
        nob_cmd_append(&cmd, "-O3", "-DHAVE_INLINE");
    }
    nob_cmd_append(&cmd, "-o", nob_temp_sprintf(BUILD_FOLDER"%s",exe_name));
    nob_cmd_append(&cmd, file_path);  // Spostato prima delle librerie
    nob_cmd_append(&cmd, "-lgsl", "-lgslcblas", "-lm");  // Librerie dopo il file sorgente

    return nob_cmd_run_async_and_reset(&cmd);
}

int main(int argc, char **argv) {

    NOB_GO_REBUILD_URSELF(argc, argv);
    program_name = nob_shift(argv, argc);

    if(!parse_args(argc, argv)) return EXIT_FAILURE;

    if(!nob_mkdir_if_not_exists(BUILD_FOLDER)) return EXIT_FAILURE;
    
    Nob_Procs procs = {0};
    nob_da_resize(&procs, THREAD_COUNT);
    procs.count = 0;

    for(int i = 0; i < NOB_ARRAY_LEN(targets); i++) {
        nob_procs_append_or_wait_and_reset(&procs, build_file(targets[i].src_path, targets[i].exe_name));
    }

    if (!nob_procs_wait(procs)) return EXIT_FAILURE;

    nob_cmd_free(cmd);

    return EXIT_SUCCESS;
}
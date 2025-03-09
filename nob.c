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

char *program_name = NULL;

int main(int argc, char **argv) {

    NOB_GO_REBUILD_URSELF(argc, argv);
    program_name = nob_shift(argv, argc);

    bool debug = false;
    if(argc > 0) {
        char *arg = nob_shift(argv, argc);
        debug = true;
        if(strcmp(arg, "debug") != 0) {
            nob_log(NOB_ERROR, "Unrecognized argument %s, please use 'debug' instead.", arg);
            return EXIT_FAILURE;
        }
    }

    if(!nob_mkdir_if_not_exists(BUILD_FOLDER)) return EXIT_FAILURE;
    Nob_Cmd cmd = {0};

    nob_cmd_append(&cmd, "gcc");
    nob_cmd_append(&cmd,"-Wall", "-Wextra", "-I" GSL_INCLUDE_PATH, "-L" GSL_LIB_PATH, "-I", "Utils/");
    if(debug) {
        nob_cmd_append(&cmd, "-ggdb");
    } else {
        nob_cmd_append(&cmd, "-O3", "-DHAVE_INLINE");
    }
    nob_cmd_append(&cmd, "-o", BUILD_FOLDER "minimize");
    nob_cmd_append(&cmd, "minimize.c");  // Spostato prima delle librerie
    nob_cmd_append(&cmd, "-lgsl", "-lgslcblas", "-lm");  // Librerie dopo il file sorgente

    if(!nob_cmd_run_sync(cmd)) return EXIT_FAILURE;

    nob_cmd_free(cmd);

    return EXIT_SUCCESS;
}
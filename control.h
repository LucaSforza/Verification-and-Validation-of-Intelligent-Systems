#ifndef CONTROL_H_
#define CONTROL_H_

#include <gsl/gsl_errno.h>

#include "utils/logging.h"
#undef ERR_MGS

void error_gsl_panic(int code, const char *file, int line) {
    eprintf("%s:%d: gsl error: %s", file, line, gsl_strerror(code));
    exit(0);
}

#define ERR_MGS error_gsl_panic(__result, __FILE__, __LINE__)

#endif // CONTROL_H_
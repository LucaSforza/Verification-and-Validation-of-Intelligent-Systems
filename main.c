#include <stdio.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_multimin.h>

#include "utils/logging.h"

#undef ERR_MGS

char *program_name;

void error_gsl_panic(int code, const char *file, int line) {
    eprintf("%s:%d: gsl error: %s", program_name, line, gsl_strerror(code));
    exit(EXIT_FAILURE);
}

#define ERR_MGS error_gsl_panic(__result, __FILE__, __LINE__)

#define N 2

double f(const gsl_vector *v, void *params) {
    (void) params;
    double x, y;
  
    x = gsl_vector_get(v, 0);
    y = gsl_vector_get(v, 1); 
  
    return gsl_pow_2(x) + gsl_pow_2(y);
}

void df(const gsl_vector *x, void *params, gsl_vector *g) {
    (void) params;
    gsl_vector_set(g, 0, 2*gsl_vector_get(x, 0));
    gsl_vector_set(g, 1, 2*gsl_vector_get(x, 1));
}

void fdf(const gsl_vector *x, void *params, double *r, gsl_vector *y) {
    *r = f(x,params);
    df(x,params,y);
}

int main(int argc, char **args) {
    program_name = *args;
    
    gsl_multimin_fdfminimizer *workspace;
    workspace = gsl_multimin_fdfminimizer_alloc(gsl_multimin_fdfminimizer_steepest_descent, N);
    fatal_if(workspace == NULL, "%s:%d: failed to allocate workspace\n", __FILE__, __LINE__);
    gsl_multimin_function_fdf func;

    func.n = N;
    func.df = df;
    func.fdf = fdf;
    func.f = f;
    func.params = NULL;

    gsl_vector *x = gsl_vector_alloc(N);
    fatal_if(x == NULL, "%s:%d: failed to allocate vector x\n", __FILE__, __LINE__);
    gsl_vector_set(x, 0, 1.0);
    gsl_vector_set(x, 1, 0.5);

    Control(
        gsl_multimin_fdfminimizer_set(workspace, &func, x, 0.01, 1e-4)
    );

    int iter = 0;
    int status = 0;
    do {
        iter++;
        Control(
            gsl_multimin_fdfminimizer_iterate(workspace)
        );

        status = gsl_multimin_test_gradient(workspace->gradient, 1e-3);

        if (status == GSL_SUCCESS)
            printf("Minimum found at:\n");

        printf("%5d %.5f %.5f %10.5f\n", iter,
            gsl_vector_get(workspace->x, 0),
            gsl_vector_get(workspace->x, 1),
        workspace->f);

    } while (status == GSL_CONTINUE && iter < 100);

    gsl_vector_free(x);
    gsl_multimin_fdfminimizer_free(workspace);

    return 0;
}
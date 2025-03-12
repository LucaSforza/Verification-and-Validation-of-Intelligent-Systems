#include <stdio.h>

#include <gsl/gsl_linalg.h>

#include "utils/logging.h"
#include "utils/matrix.h"

#include "control.h"

#define N 4
#define PRECISION 0.01

bool test_matrix_data(double *data, size_t n, size_t m, double expected) {

    static int test_id = 1;

    eprintf("\ttest number: %d... ", test_id);
    test_id++;

    gsl_permutation *p = gsl_permutation_alloc(N);
    control_mem_err(p);
    gsl_matrix_view view = gsl_matrix_view_array(data, n, m);

    int signum;

    Control(gsl_linalg_LU_decomp(&view.matrix ,p, &signum));

    gsl_permutation_free(p);

    double det = gsl_linalg_LU_det(&view.matrix, signum);
    if(fabs(det - expected) > PRECISION) {
        log_error("Expected det: %lf, actual: %lf",expected, det);
        return false;
    }

    eprintf(" test passed!\n");

    return true;
}

int main(void) {
    double a_data[] = // det(a_data) = 0 
        {1, 2, 3, 4,
         0, 2, 5, 2,
         0, 0, 3, 3,
         1, 0, 0, 4 };
    
    if(!test_matrix_data(a_data, N, N, 0.0)) return 1;

    log_info("All tests passed");
    return 0;
}
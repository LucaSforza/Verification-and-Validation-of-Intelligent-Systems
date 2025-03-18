#include <stdio.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

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
        eprintf("NOT PASSED: Expected det: %lf, actual: %lf\n",expected, det);
        return false;
    }

    eprintf("test passed!\n");

    return true;
}

int geometric_multiplicity(gsl_matrix *A, gsl_complex lamda) {
    assert(A->size1 == A->size2);
    size_t n = A->size1;
    gsl_matrix_complex *B = gsl_matrix_complex_alloc(n, n);
    gsl_matrix_complex_view A_complex = gsl_matrix_complex_view_array((double *)A->data, n, n);
    gsl_matrix_complex_memcpy(B, &A_complex.matrix);
    for (int i = 0; i < n; i++) {
        gsl_complex z = gsl_matrix_complex_get(B, i, i);
        gsl_complex diff = gsl_complex_sub(z, lamda);
        gsl_matrix_complex_set(B, i, i, diff);
    }
    // TODO
}

void calculate_jordan_normal_form(gsl_matrix *m) {
    if (m->size1 != m->size2) {
        log_error("Matrix is not square, cannot compute Jordan normal form.\n");
        exit(1);
    }

    size_t n = m->size1;

    gsl_vector_complex *eigenvectors = gsl_vector_complex_alloc(n);
    gsl_matrix_complex *evec = gsl_matrix_complex_alloc(n, n);

    gsl_eigen_nonsymmv_workspace *w = gsl_eigen_nonsymmv_alloc(n);

    Control(gsl_eigen_nonsymmv(m, eigenvectors, evec, w));

    gsl_eigen_nonsymmv_free(w);

    // Print eigenvalues
    for (size_t i = 0; i < n; i++) {
        gsl_complex lamda = gsl_vector_complex_get(eigenvectors, i);
        double eval_real = GSL_REAL(lamda);
        double eval_imag = GSL_IMAG(lamda);
        printf("Eigenvalue %zu: %g + %gi\n", i, eval_real, eval_imag);
    }

    gsl_matrix_complex_free(evec);
    gsl_vector_complex_free(eigenvectors);
}

int main(void) {
    double a_data[] = // det(a_data) = 0 
        {1, 2, 3, 4,
         0, 2, 5, 2,
         0, 0, 3, 3,
         1, 0, 0, 4 };
    gsl_matrix_view a_matrix = gsl_matrix_view_array(a_data, N, N);

    double b_data[] = // det(b_data) = -12
        {1, 2, 3, 4,
         0, 2, 5, 2,
         0, 0, 3, 3,
         1, 0, 0, 2 };
    gsl_matrix_view b_matrix = gsl_matrix_view_array(b_data, N, N);

    double c_data[] = // det(c_data) = -18
         {1, 2, 3, 4,
          0, 2, 6, 2,
          0, 0, 3, 3,
          1, 0, 0, 2 };
    gsl_matrix_view c_matrix = gsl_matrix_view_array(c_data, N, N);

    double *datas[] = {
        a_data, b_data, c_data
    };

    gsl_matrix *matrixs[] = {
        &a_matrix.matrix, &b_matrix.matrix, &c_matrix.matrix
    };

    size_t failed = 0;

    double expected[] = {0.0, -12.0, -18.0};

    static_assert(ARRAY_LEN(datas) == ARRAY_LEN(expected));

    /*for(size_t i=0; i < ARRAY_LEN(datas); ++i)
        if(!test_matrix_data(datas[i], N, N, expected[i]))
            failed++;

    if(failed != 0) {
        log_error("%zu tests failed", failed);
        return 1;
    }

    log_info("All tests passed"); */

    for(size_t i = 0; i < ARRAY_LEN(matrixs); i++) {
        calculate_jordan_normal_form(matrixs[i]);
    }

    return 0;
}
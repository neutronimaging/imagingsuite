#include "lminvert.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void show_matrix(const char* name, double *A, int n) {
    printf("%s:\n", name);
    for (int j=0; j < n; j++) {
        for (int i=0; i < n; i++)
            printf("%12.5f ", A[j*n+i]);
        printf("\n");
    }
}

int test(int n, double* A, int expect_singular)
{
    double* ws = (double*)calloc(n*n, sizeof(double));
    double* inv = (double*)calloc(n*n, sizeof(double));
    int*    P = (int*)calloc(n, sizeof(int));
    if (!ws || !inv) {
        printf("unexpected error !ws or !inv\n");
        exit(1);
    }
    show_matrix("A", A, n);
    int failure = 0;
    lm_invert(A, n, P, ws, inv, &failure);
    if (failure==0) {
        ; // ok
    } else if (failure==21) {
        if (expect_singular) {
            printf("singular as expected\n");
            return 0;
        }
        printf("ERROR: unexpectedly singular\n");
        return 1;
    } else if (failure==22) {
        printf("ERROR: inaccurate\n");
        return 1;
    } else {
        printf("ERROR: unexpected error %i\n", failure);
        return 1;
    }
    if (expect_singular) {
        printf("ERROR: unexpectedly nonsingular\n");
        return 0;
    }
    show_matrix("Ainv", inv, n);

    // compute inv*A and A*inv
    double* UL = (double*)calloc(n*n, sizeof(double));
    double* UR = (double*)calloc(n*n, sizeof(double));
    for (int i=0; i<n; ++i) {
        for (int j=0; j<n; ++j) {
            UL[i*n+j] = 0;
            UR[i*n+j] = 0;
            for (int k=0; k<n; ++k) {
                UL[i*n+j] += inv[n*i+k] * A  [n*k+j];
                UR[i*n+j] += A  [n*i+k] * inv[n*k+j];
            }
        }
    }
    show_matrix("Ainv*A", UL, n);
    show_matrix("A*Ainv", UR, n);
    int err = 0;
    const double eps0 = 1e-13; // confirmed to work on amd64, ppc64le, aarch64, s390x
    const double eps1 = eps0;
    for (int i=0; i<n; ++i) {
        for (int j=0; j<n; ++j) {
            if (i==j ? fabs(UL[i*n+j]-1)>eps1 : fabs(UL[i*n+j])>eps0) {
                printf("inaccurate element UL[%i,%i]=%g\n", i, j, UL[i*n+j]);
                err += 1;
            }
            if (i==j ? fabs(UR[i*n+j]-1)>eps1 : fabs(UR[i*n+j])>eps0) {
                printf("inaccurate element UR[%i,%i]=%g\n", i, j, UR[i*n+j]);
                err += 1;
            }
        }
    }
    return err;
}

int main() {
    int err = 0;

    double m0[] = {1, 1.,
                   0, 1};
    err += test(2, m0, 0);

    double ms[] = {1, 3.,
                   1, 3};
    err += test(2, ms, 1);

    double m1[] = {25, 15, -5,
                   15, 18,  0,
                   -5,  0, 11};
    err += test(3, m1, 0);

    double m2[] = {18, 22,  54,  42,
                   22, 70,  86,  62,
                   54, 86, 174, 134,
                   42, 62, 134, 106};
    err += test(4, m2, 0);

    if (err) {
        printf("OVERALL FAILURE, %i errors\n", err);
        return 1;
    }
    printf("OVERALL SUCCESS, no errors\n");
    return 0;
}

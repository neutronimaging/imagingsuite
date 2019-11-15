/*
 * Library:   lmfit (Levenberg-Marquardt least squares fitting)
 *
 * File:      lminvert.c
 *
 * Contents:  Inverts a square matrix, using the standard LUP algorithm.
 *
 * Copyright: Joachim Wuttke, Forschungszentrum Juelich GmbH (2018-)
 *
 * License:   see ../COPYING (FreeBSD)
 *
 * Homepage:  apps.jcns.fz-juelich.de/lmfit
 */

#include "lminvert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void lm_decompose_LUP(double *const A, const int n, const double Tol,
                      int *const P, int*const  failure);
void lm_invert_LUP(double *const A, const int n, int *const P, double *const IA);
void lm_check_inverse(double *const A, double *const IA, const int n,
                      const double Tol, int*const  failure);

/* INPUT:
 *   A   - square matrix of size n*n, stored as one-dimensional array
 *   n   - matrix dimension
 *   Tol - small number, tolerance used to detect when the matrix is
 *         near degenerate
 *   P   - array of size n
 * OUTPUT:
 *   A   - now contains both matrices L-E and U as A=(L-E)+U such that P*A=L*U
 *   P   - now contains column indexes where the permutation matrix has "1".
 *   failure - set to a nonzero value if matrix cannot be inverted.
 */
void lm_decompose_LUP(double *const A, const int n, const double Tol,
                      int *const P, int*const  failure)
{
    int i, j, k, imax;
    double maxA, absA, tmp;

    for (i = 0; i < n; i++)
        P[i] = i; //Unit permutation matrix, P[n] initialized with n

    for (i = 0; i < n; i++) {
        maxA = 0.0;
        imax = i;

        for (k = i; k < n; k++)
            if ((absA = fabs(A[k*n+i])) > maxA) {
                maxA = absA;
                imax = k;
            }

        if (maxA < Tol) {
            *failure = 21; // matrix is degenerate
            return;
        }

        if (imax != i) {
            //pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            //pivoting rows of A
            for (k=0; k<n; ++k) {
                tmp = A[i*n+k];
                A[i*n+k] = A[imax*n+k];
                A[imax*n+k] = tmp;
            }
        }

        for (j = i + 1; j < n; j++) {
            A[j*n+i] /= A[i*n+i];
            for (k = i + 1; k < n; k++)
                A[j*n+k] -= A[j*n+i] * A[i*n+k];
        }
    }
}

/*  Computes inverse matrix IA for given decomposition A,P.  */

void lm_invert_LUP(double *const A, const int n, int *const P, double *const IA)
{
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            if (P[i] == j)
                IA[i*n+j] = 1.0;
            else
                IA[i*n+j] = 0.0;

            for (int k = 0; k < i; k++)
                IA[i*n+j] -= A[i*n+k] * IA[k*n+j];
        }

        for (int i = n - 1; i >= 0; i--) {
            for (int k = i + 1; k < n; k++)
                IA[i*n+j] -= A[i*n+k] * IA[k*n+j];

            IA[i*n+j] = IA[i*n+j] / A[i*n+i];
        }
    }
}

/* Verifies that IA is the inverse of A.
 *
 * INPUT:
 *   A   - square matrix of size n*n, stored as one-dimensional array
 *   IA  - square matrix of size n*n, stored as one-dimensional array
 *   n   - matrix dimension
 *   Tol - small number, maximum deviation from 0 or 1
 * OUTPUT:
 *   failure - set to a nonzero value if a deviation is larger than Tol.
 */
void lm_check_inverse(double *const A, double *const IA, const int n,
                      const double Tol, int*const  failure)
{
    for (int j=0; j<n; j++) {
        for (int i=0; i<n; i++) {
            double s = 0;
            for (int k=0; k<n; k++)
                s += IA[j*n+k] * A[k*n+i];
            double expected = (i==j) ? 1. : 0.;
            if (fabs(s-expected)>Tol) {
                fprintf(stderr, "lm_check_inverse found %g instead of %g at %i,%i \n",
                        s, expected, j, i);
                *failure = 22;
                return;
            }
        }
    }
}

/* Inverts matrix A.
 *
 * INPUT:
 *   A   - square matrix of size n*n, stored as one-dimensional array
 *   n   - matrix dimension
 *   P   - workspace, array of integers of size n
 *   ws  - workspace, array of double of size n*n
 * OUTPUT:
 *   IA   - the inverse matrix A^-1
 *   failure - set to a nonzero value if matrix cannot be inverted.
 */
 void lm_invert(double *const A, const int n, int *const P, double *const ws,
                double *const IA, int*const  failure)
{
    memcpy(ws, A, n*n*sizeof(double));
    lm_decompose_LUP(ws, n, 1e-12, P, failure);
    if (*failure)
        return;
    lm_invert_LUP(ws, n, P, IA);
    lm_check_inverse(IA, A, n, 1e-6, failure);
    lm_check_inverse(A, IA, n, 1e-6, failure);
    if (*failure)
        return;
}

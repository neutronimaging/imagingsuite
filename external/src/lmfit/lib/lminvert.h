/*
 * Library:   lmfit (Levenberg-Marquardt least squares fitting)
 *
 * File:      lminvert.h
 *
 * Contents:  Declares square matrix inversion function.
 *
 * Copyright: Joachim Wuttke, Forschungszentrum Juelich GmbH (2018-)
 *
 * License:   see ../COPYING (FreeBSD)
 *
 * Homepage:  apps.jcns.fz-juelich.de/lmfit
 */

void lm_invert(double *const A, const int n, int *const P, double *const ws,
               double *const IA, int*const failure);

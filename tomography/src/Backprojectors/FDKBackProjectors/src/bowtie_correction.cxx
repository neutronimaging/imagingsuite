/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */
#include "plmreconstruct_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if FFTW_FOUND
#include "fftw3.h"
#endif

#include "bowtie_correction.h"
#include "fdk.h"
#include "mha_io.h"
#include "plm_int.h"
#include "plm_math.h"
#include "volume.h"

#ifndef DEGTORAD
static const double DEGTORAD = 3.14159265 / 180.0;
#endif

#if FFTW_FOUND
static void
LowPass (int n, double * v)
{
    int i;
    fftw_complex* in, * fft, * ifft;
    fftw_plan fftp, ifftp;
    in = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * n);
    fft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * n);
    ifft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * n);
    for (i = 0; i < n; i++) {
        in[i][0] = v[i];
        in[i][1] = 0.0;
    }
    fftp = fftw_plan_dft_1d (n, in, fft, FFTW_FORWARD, FFTW_ESTIMATE);
    ifftp = fftw_plan_dft_1d (n, fft, ifft, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute (fftp);
    for (i = 0; i < n; i++) {
        double tmp = pow ((cos (i * DEGTORAD * 360 / n) + 1) / 2, 20);
        fft[i][0] *= tmp;
        fft[i][1] *= tmp;
    }
    fftw_execute (ifftp);
    for (i = 0; i < n; i++)
        v[i] = ifft[i][0] / (float) n;
}

static void
process_norm_CBCT (Volume * norm_CBCT, Fdk_parms* parms)
{
    plm_long ni, nj, nk;
    double norm_radius;
    double radius;
    int radius_r;
    double average;
    double *average_r;
    int *pixels_r;
    int pixels;
    float *norm;
    average_r = (double *) malloc (norm_CBCT->dim[0] * sizeof(double));
    if (average_r == NULL) {
        printf ("Malloc error for *average");
        exit (1);
    }

    for (ni = 0; ni < norm_CBCT->dim[0]; ni++)
        average_r[ni] = 0.0f;

    pixels_r = (int *) malloc (norm_CBCT->dim[0] * sizeof(int));
    if (pixels_r == NULL) {
        printf ("Malloc error for *pixels_r");
        exit (1);
    }

    for (ni = 0; ni < norm_CBCT->dim[0]; ni++)
        pixels_r[ni] = 0.0f;

    pixels = 0;
    average = 0;

    norm = (float *) norm_CBCT->img;

    if (parms->full_fan) {
        printf ("Processing %s\n", parms->Full_normCBCT_name);
        norm_radius = parms->Full_radius;
    } else  {
        printf ("Processing %s\n", parms->Half_normCBCT_name);
        norm_radius = parms->Half_radius;
    }

    for (nj = 0; nj < norm_CBCT->dim[1]; nj++) {
        for (ni = 0; ni < norm_CBCT->dim[0]; ni++) {
            float x = (float) ni * norm_CBCT->spacing[0] + norm_CBCT->offset[0] - norm_CBCT->spacing[0];
            float y = (float) nj * norm_CBCT->spacing[1] + norm_CBCT->offset[1] - norm_CBCT->spacing[1];
            radius = sqrt (x * x + y * y);
            radius_r = (int)(radius / norm_CBCT->spacing[0] + 0.5);
            for (nk = 0; nk < norm_CBCT->dim[2]; nk++) {
                if (radius > norm_radius - 20 && radius < norm_radius) {
                    plm_long pi = volume_index (norm_CBCT->dim, ni, nj, nk);
                    average += norm[pi];
                    pixels++;
                }
                average_r[radius_r] += norm[volume_index (norm_CBCT->dim, ni, nj, nk)];
                pixels_r[radius_r]++;
            }
        }
    }

    average /= (float) pixels;
    printf ("average(radius %d-%d mm)=%f\n", (int) norm_radius - 20, (int) norm_radius, average);

    for (ni = 0; ni < norm_CBCT->dim[0]; ni++)
        average_r[ni] /= pixels_r[ni];
    for (ni = 0; ni <= 2; ni++)
        average_r[ni] = average_r[3];

    LowPass (140, average_r);

    for (nj = 0; nj < norm_CBCT->dim[1]; nj++) {
        for (ni = 0; ni < norm_CBCT->dim[0]; ni++) {
            float x = (float) ni * norm_CBCT->spacing[0] + norm_CBCT->offset[0] - norm_CBCT->spacing[0];
            float y = (float) nj * norm_CBCT->spacing[1] + norm_CBCT->offset[1] - norm_CBCT->spacing[1];
            radius = sqrt (x * x + y * y);
            radius_r = (int)(radius / norm_CBCT->spacing[0] + 0.5);
            for (nk = 0; nk < norm_CBCT->dim[2]; nk++) {
                if (radius > norm_radius)
                    norm[volume_index (norm_CBCT->dim, ni, nj, nk)] = 0.0f;
                else{
                    norm[volume_index (norm_CBCT->dim, ni, nj, nk)] = average - average_r[radius_r];
                }
            }
        }
    }
    printf ("The norm mha is processed\n");
    free (average_r);
    free (pixels_r);
}
#endif /* FFTW_FOUND */

void
bowtie_correction (Volume *vol, Fdk_parms *parms)
{
    Volume *norm_CBCT;
    float *img, *norm;
    plm_long ni, nj, nk;
    plm_long i, j, k;

    if (parms->full_fan) {
        norm_CBCT = read_mha (parms->Full_normCBCT_name);
    } else   {
        norm_CBCT = read_mha (parms->Half_normCBCT_name);
    }

#if FFTW_FOUND
    process_norm_CBCT (norm_CBCT, parms);
#endif
    img = (float *) vol->img;
    norm = (float *) norm_CBCT->img;

    for (k = 0; k < vol->dim[2]; k++) {
        nk = (int) floor ((k * vol->spacing[2] + vol->offset[2] - vol->spacing[2] - (norm_CBCT->offset[2] - norm_CBCT->spacing[2])) / norm_CBCT->spacing[2]);
        if ((nk < 0) || (nk >= norm_CBCT->dim[2]))
            continue;
        for (j = 0; j < vol->dim[1]; j++) {
            nj = (int) floor ((j * vol->spacing[1] + vol->offset[1] - vol->spacing[1] - (norm_CBCT->offset[1] - norm_CBCT->spacing[1])) / norm_CBCT->spacing[1]);
            if ((nj < 0) || (nj >= norm_CBCT->dim[1]))
                continue;
            for (i = 0; i < vol->dim[0]; i++) {
                ni = (int) floor ((i * vol->spacing[0] + vol->offset[0] - vol->spacing[0] - (norm_CBCT->offset[0] - norm_CBCT->spacing[0])) / norm_CBCT->spacing[0]);
                if ((ni < 0) || (ni >= norm_CBCT->dim[0]))
                    continue;
                img[volume_index (vol->dim, i, j, k)] += norm[volume_index (norm_CBCT->dim, ni, nj, nk)];
            }
        }
    }
    free (norm_CBCT->img);
    free (norm_CBCT);
}

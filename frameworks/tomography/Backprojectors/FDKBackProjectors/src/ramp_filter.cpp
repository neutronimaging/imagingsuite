/* -----------------------------------------------------------------------
   See COPYRIGHT.TXT and LICENSE.TXT for copyright and license information
   ----------------------------------------------------------------------- */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fftw3.h"

#include "ramp_filter.h"


#ifndef PI
static const double PI = 3.14159265;
#endif

#ifndef DEGTORAD
static const double DEGTORAD = 3.14159265 / 180.0;
#endif

#ifndef MARGIN
static const unsigned int MARGIN = 5;
#else
#error "MARGIN IS DEFINED"
#endif

/* GCS: Dec 12, 2009 
   Change from unsigned short to float,
   Change to destructive update
*/
//! In-place ramp filter for greyscale images
//! \param data The pixel data of the image
//! \param width The width of the image
//! \param height The height of the image
//! \template_param T The type of pixel in the image
void
ramp_filter (
    kipl::base::TImage <float,2> &img
)
{
    unsigned int i, r, c;
    unsigned int N;

    fftw_complex *in;
    fftw_complex *fft;
    fftw_complex *ifft;
    fftw_plan fftp;
    fftw_plan ifftp;
    double *ramp;
    ramp = (double*) malloc (width * sizeof(double));
    if (!ramp) {
        print_and_exit ("Error allocating memory for ramp\n");
    }
    N = width * height;
    in = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);
    fft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);
    ifft = (fftw_complex*) fftw_malloc (sizeof(fftw_complex) * N);
    if (!in || !fft || !ifft) {
        print_and_exit ("Error allocating memory for fft\n");
    }

    for (r = 0; r < MARGIN; ++r)
        memcpy (data + r * width, data + MARGIN * width,
        width * sizeof(float));

    for (r = height - MARGIN; r < height; ++r)
        memcpy (data + r * width, data + (height - MARGIN - 1) * width,
        width * sizeof(float));

    for (r = 0; r < height; ++r) {
        for (c = 0; c < MARGIN; ++c)
            data[r * width + c] = data[r * width + MARGIN];
        for (c = width - MARGIN; c < width; ++c)
            data[r * width + c] = data[r * width + width - MARGIN - 1];
    }

    /* Fill in ramp filter in frequency space */
    for (i = 0; i < N; ++i) {
        in[i][0] = (double)(data[i]);
        //in[i][0] /= 65535;
        //in[i][0] = (in[i][0] == 0 ? 1 : in[i][0]);
        //in[i][0] = -log (in[i][0]);
        in[i][1] = 0.0;
    }

    /* Add padding */
    for (i = 0; i < width / 2; ++i)
        ramp[i] = i;

    for (i = width / 2; i < (unsigned int) width; ++i)
        ramp[i] = width - i;

    /* Roll off ramp filter */
    for (i = 0; i < width; ++i)
        ramp[i] *= (cos (i * DEGTORAD * 360 / width) + 1) / 2;

    for (r = 0; r < height; ++r)
    {
    fftp = fftw_plan_dft_1d (width, in + r * width, fft + r * width,
        FFTW_FORWARD, FFTW_ESTIMATE);
    if (!fftp) {
        print_and_exit ("Error creating fft plan\n");
    }
    ifftp = fftw_plan_dft_1d (width, fft + r * width, ifft + r * width,
        FFTW_BACKWARD, FFTW_ESTIMATE);
    if (!ifftp) {
        print_and_exit ("Error creating ifft plan\n");
    }

        fftw_execute (fftp);

        // Apply ramp
        for (c = 0; c < width; ++c) {
            fft[r * width + c][0] *= ramp[c];
            fft[r * width + c][1] *= ramp[c];
        }

        fftw_execute (ifftp);

    fftw_destroy_plan (fftp);
    fftw_destroy_plan (ifftp);
    }

    for (i = 0; i < N; ++i)
        ifft[i][0] /= width;

    for (i = 0; i < N; ++i)
        data[i] = (float)(ifft[i][0]);

    fftw_free (in);
    fftw_free (fft);
    fftw_free (ifft);
    free (ramp);
}

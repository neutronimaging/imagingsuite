//<LICENSE>

#ifndef FDKBP_SINGLE_H
#define FDKBP_SINGLE_H
#include "fftw3.h"
#include "fdkbackproj_global.h"
#include "fdkreconbase.h"
#include <ParameterHandling.h>

#include <interactors/interactionbase.h>

//namespace reconstructor{ namespace UnitTests {
//    class testBasicReconstructor;
//}}

class FDKbp_single : public FdkReconBase
{
public:
    FDKbp_single(kipl::interactors::InteractionBase *interactor=nullptr);
    ~FDKbp_single();

    virtual int Initialize();

protected:
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj); ///< Compute the geometry matrix for each projection and passes it to the backprojector
    float m_fAlpha;
    void ramp_filter (kipl::base::TImage<float,2>  &img);
    void ramp_filter_tuned(kipl::base::TImage<float, 2> &img);
    void project_volume_onto_image_reference (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, float *nrm);///< Reference FDK implementation is the most straightforward implementation, also it is the slowest
    void project_volume_onto_image_c (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, size_t nProj);///< Multi core accelerated FDK implementation
    float get_pixel_value_b (kipl::base::TImage<float,2> &cbi, float r, float c);
    float get_pixel_value_c (kipl::base::TImage<float,2> &cbi, float r, float c);
    void getProjMatrix(float angles, float* nrm, float *proj_matrix);
    void multiplyMatrix (float *mat1, float *mat2, float *result, int rows, int columns, int columns1);
    void prepareFFT(int width,int height);
    void cleanupFFT();
    virtual int InitializeBuffers(int width, int height);
    virtual int FinalizeBuffers();

    unsigned int padwidth;
    fftwf_complex *in;
    fftwf_complex *in_buffer;
    fftwf_complex *fft;
    fftwf_complex *ifft;
    fftwf_complex *ifft_buffer;
    fftwf_plan fftp;
    fftwf_plan ifftp;
};

#endif // FDKBP_H

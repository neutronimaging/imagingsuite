//<LICENSE>

#ifndef FDKBP_H
#define FDKBP_H

#include "fdkbackproj_global.h"
#include "fdkreconbase.h"
#include <ParameterHandling.h>

#include <interactors/interactionbase.h>

//namespace reconstructor{ namespace UnitTests {
//    class testBasicReconstructor;
//}}

class FDKBACKPROJSHARED_EXPORT FDKbp : public FdkReconBase
{
public:
    FDKbp(kipl::interactors::InteractionBase *interactor=nullptr);
    ~FDKbp();

    virtual int Initialize();

protected:
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj); ///< Compute the geometry matrix for each projection and passes it to the backprojector
    float m_fAlpha;
    void ramp_filter (kipl::base::TImage<float,2>  &img);
    void ramp_filter_tuned(kipl::base::TImage<float, 2> &img);
    void project_volume_onto_image_reference (kipl::base::TImage<float,2>  &cbi, double *proj_matrix, double *nrm);///< Reference FDK implementation is the most straightforward implementation, also it is the slowest
    void project_volume_onto_image_c (kipl::base::TImage<float,2>  &cbi, double *proj_matrix, size_t nProj);///< Multi core accelerated FDK implementation
    float get_pixel_value_b (kipl::base::TImage<float,2> &cbi, double r, double c);
    float get_pixel_value_c (kipl::base::TImage<float,2> &cbi, double r, double c);
    void getProjMatrix(float angles, double* nrm, double *proj_matrix);
    void multiplyMatrix (double *mat1, double *mat2, double *result, int rows, int columns, int columns1);
};

#endif // FDKBP_H

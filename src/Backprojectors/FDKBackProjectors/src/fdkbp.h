#ifndef FDKBP_H
#define FDKBP_H

#include "fdkbackproj_global.h"
#include "fdkreconbase.h"
#include <ParameterHandling.h>

#include <interactors/interactionbase.h>

//namespace reconstructor{ namespace UnitTests {
//    class testBasicReconstructor;
//}}

class FDKbp : public FdkReconBase
{
public:
    FDKbp(kipl::interactors::InteractionBase *interactor=nullptr);
    ~FDKbp();


protected:
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles); ///< Compute the geometry matrix for each projection and passes it to the backprojector
    float m_fAlpha; // was ist das?
    void ramp_filter (kipl::base::TImage<float,2>  &img);
    void project_volume_onto_image_reference (kipl::base::TImage<float,2>  &cbi, double *proj_matrix, double *nrm);///< Reference FDK implementation is the most straightforward implementation, also it is the slowest
    void project_volume_onto_image_c (kipl::base::TImage<float,2>  &cbi, double *proj_matrix);///< Multi core accelerated FDK implementation
    void project_volume_onto_image_rtk (kipl::base::TImage<float,2>  &cbi, double *proj_matrix);/// attempt with FDK-RTK matrices
    float get_pixel_value_b (kipl::base::TImage<float,2> &cbi, double r, double c);
    float get_pixel_value_c (kipl::base::TImage<float,2> &cbi, double r, double c);
    void getProjMatrix(float angles, double* nrm, double *proj_matrix);
    void computeProjMatrix(float angles, double* nrm, double *proj_matrix);
    void compRTKProjMatrix(float angles, double* nrm, double *proj_matrix);
    void multiplyMatrix (double *mat1, double *mat2, double *result, int rows, int columns, int columns1);
};

#endif // FDKBP_H

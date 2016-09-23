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
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles); // virtual or not virtual?
    float m_fAlpha; // was ist das?
    void ramp_filter (kipl::base::TImage<float,2>  &img);
    void project_volume_onto_image_reference (kipl::base::TImage<float,2>  &cbi, float scale, double *proj_matrix, double *nrm);
    void project_volume_onto_image_c (kipl::base::TImage<float,2>  &cbi, float scale, double *proj_matrix, double *nrm);
    float get_pixel_value_b (kipl::base::TImage<float,2> &cbi, double r, double c);
    float get_pixel_value_c (kipl::base::TImage<float,2> &cbi, double r, double c);
};

#endif // FDKBP_H

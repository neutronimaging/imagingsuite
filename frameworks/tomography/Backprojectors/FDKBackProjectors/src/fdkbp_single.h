//<LICENSE>

#ifndef FDKBP_SINGLE_H
#define FDKBP_SINGLE_H
#include "fftw3.h"
#include "fdkbackproj_global.h"
#include "fdkreconbase.h"
#include <ParameterHandling.h>
#include <iostream>

#include <interactors/interactionbase.h>
#include <utilities/threadpool.h>

//namespace reconstructor{ namespace UnitTests {
//    class testBasicReconstructor;
//}}

enum class eFDKbp_singleAlgorithms
{
    reference,
    c,
    c2,
    stl
};

enum class eFDKbp_interpolation
{
    nearest,
    bilinear
};

class FDKBACKPROJSHARED_EXPORT FDKbp_single : public FdkReconBase
{
public:
    FDKbp_single(kipl::interactors::InteractionBase *interactor=nullptr);
    ~FDKbp_single();

    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

    virtual int Initialize() override;

    std::map<std::string, std::string> GetParameters() override;

protected:
    virtual size_t reconstruct(kipl::base::TImage<float,2> &proj, float angles, size_t nProj) override; ///< Compute the geometry matrix for each projection and passes it to the backprojector
    float m_fAlpha;
    eFDKbp_singleAlgorithms m_algorithm;
    eFDKbp_interpolation m_interpolation;
    void  project_volume_onto_image_reference (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, float *nrm);///< Reference FDK implementation is the most straightforward implementation, also it is the slowest
    void  project_volume_onto_image_c (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, size_t nProj);///< Multi core accelerated FDK implementation
    void  project_volume_onto_image_c2 (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, size_t nProj);///< Multi core accelerated FDK implementation cleanups
    void  project_volume_onto_image_stl (kipl::base::TImage<float,2>  &cbi, float *proj_matrix, size_t nProj);///< Multi core accelerated FDK implementation cleanups
    
    float get_pixel_value_b (kipl::base::TImage<float,2> &cbi, float r, float c);
    float get_pixel_value_c (kipl::base::TImage<float,2> &cbi, float r, float c);
    float get_pixel_value_interp (kipl::base::TImage<float,2> &cbi, float r, float c);
    void  getProjMatrix(float angles, float* nrm, float *proj_matrix);
    void  multiplyMatrix (float *mat1, float *mat2, float *result, int rows, int columns, int columns1);

    kipl::utilities::ThreadPool *m_threadPool;
};

FDKBACKPROJSHARED_EXPORT void string2enum(const std::string &str, eFDKbp_singleAlgorithms &alg);
FDKBACKPROJSHARED_EXPORT std::string enum2string(eFDKbp_singleAlgorithms alg);
FDKBACKPROJSHARED_EXPORT ostringstream& operator<<(ostringstream& os, eFDKbp_singleAlgorithms alg);

FDKBACKPROJSHARED_EXPORT void string2enum(const std::string &str, eFDKbp_interpolation &interp);
FDKBACKPROJSHARED_EXPORT std::string enum2string(const eFDKbp_interpolation &interp);
FDKBACKPROJSHARED_EXPORT ostringstream& operator<<(ostringstream& os, const eFDKbp_interpolation &interp);

#endif // FDKBP_H

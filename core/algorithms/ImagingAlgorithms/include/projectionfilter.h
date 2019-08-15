//<LICENSE>
#ifndef PROJECTIONFILTER_H
#define PROJECTIONFILTER_H

#include "ImagingAlgorithms_global.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <logging/logger.h>
#include <base/timage.h>
#include <fft/fftbase.h>
#include <interactors/interactionbase.h>

namespace ImagingAlgorithms {
enum ProjectionFilterType {
    ProjectionFilterNone,
    ProjectionFilterRamLak,
    ProjectionFilterSheppLogan,
    ProjectionFilterHanning,
    ProjectionFilterHamming,
    ProjectionFilterButterworth,
    ProjectionFilterParzen
};

class IMAGINGALGORITHMSSHARED_EXPORT ProjectionFilterBase
{
protected:
    kipl::logging::Logger logger;
    kipl::interactors::InteractionBase *m_Interactor;
public:
    ProjectionFilterBase(std::string name="ProjectionFilterBase", kipl::interactors::InteractionBase *interactor=nullptr);
    virtual ~ProjectionFilterBase(void) {}

    virtual std::map<std::string, std::string> parameters();
    void setParameters(const std::map<std::string, std::string> &params);
    void  setFilter(ImagingAlgorithms::ProjectionFilterType ft, float cutOff, float _order=0.0f);
    ImagingAlgorithms::ProjectionFilterType filterType();
    float cutOff();
    float order();

    void  setBiasBehavior(bool _useBias, float biasWeight);
    bool  useBias();
    float biasWeight();

    void   setPaddingDoubler(size_t N);
    size_t paddingDoubler();

    size_t currentFFTSize();
    size_t currentImageSize();

    int process(kipl::base::TImage<float,2> & img);
    int process(kipl::base::TImage<float,3> & img);
protected:
    virtual void buildFilter(const size_t N) = 0;
    virtual void filterProjection(kipl::base::TImage<float,2> & img) = 0;
    size_t ComputeFilterSize(size_t len);
    bool   updateStatus(float val, const std::string & msg);

    ProjectionFilterType m_FilterType;

    float  m_fCutOff;
    float  m_fOrder;

    bool   m_bUseBias;
    float  m_fBiasWeight;

    size_t m_nPaddingDoubler;

    size_t nFFTsize;
    size_t nImageSize;

    bool bParametersChanged;
};

class IMAGINGALGORITHMSSHARED_EXPORT ProjectionFilter :
    public ProjectionFilterBase
{
public:
    ProjectionFilter(kipl::interactors::InteractionBase *interactor);
    virtual ~ProjectionFilter(void);

    const std::vector<float> & filterWeights() { return mFilter; }
private:
    virtual void buildFilter(const size_t N);
    virtual void PreparePadding(const size_t nImage, const size_t nFilter);
    virtual void filterProjection(kipl::base::TImage<float,2> & img);
    size_t Pad(float const * const pSrc, const size_t nSrcLen, float *pDest, const size_t nDestLen);

    std::vector<float> mFilter;
    kipl::base::TImage<float,1> mPadData;

    kipl::math::fft::FFTBaseFloat *fft;

    size_t nInsert;

};
}

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(std::ostream & s, ImagingAlgorithms::ProjectionFilterType ft);
void         IMAGINGALGORITHMSSHARED_EXPORT   string2enum(const std::string str, ImagingAlgorithms::ProjectionFilterType &ft);
std::string  IMAGINGALGORITHMSSHARED_EXPORT   enum2string(const ImagingAlgorithms::ProjectionFilterType &ft);

#endif // PROJECTIONFILTER_H

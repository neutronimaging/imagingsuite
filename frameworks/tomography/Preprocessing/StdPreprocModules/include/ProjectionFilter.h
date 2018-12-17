//<LICENSE>

#ifndef _PROJECTIONFILTER_H_
#define _PROJECTIONFILTER_H_

#include "StdPreprocModules_global.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <PreprocModuleBase.h>

#include <logging/logger.h>
#include <base/timage.h>
#include <fft/fftbase.h>
#include <interactors/interactionbase.h>


class STDPREPROCMODULESSHARED_EXPORT ProjectionFilterBase:
    public PreprocModuleBase
{
public:
    ProjectionFilterBase(std::string name="ProjectionFilterBase", kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~ProjectionFilterBase(void) {};
	enum FilterType {
        FilterRamLak=0,
		FilterSheppLogan,
		FilterHanning,
		FilterHamming,
        FilterButterworth
	};


	virtual std::map<std::string, std::string> GetParameters();
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
	virtual void BuildFilter(const size_t N) = 0;

	virtual void FilterProjection(kipl::base::TImage<float,2> & img) = 0;
	FilterType m_FilterType;
	
	float m_fCutOff;
	float m_fOrder;
	size_t nFFTsize;
	size_t nImageSize;

	bool m_bUseBias;
    float m_fBiasWeight;
    size_t m_nPaddingDoubler;
	size_t ComputeFilterSize(size_t len);
};



class STDPREPROCMODULESSHARED_EXPORT ProjectionFilter :
	public ProjectionFilterBase
{
public:
    ProjectionFilter(kipl::interactors::InteractionBase *interactor);
	virtual ~ProjectionFilter(void);

	const kipl::base::TImage<double,1> GetFilterArray() {return mFilter;}
private:
	virtual void BuildFilter(const size_t N);
	virtual void FilterProjection(kipl::base::TImage<float,2> & img);
	FilterType mFilterType;
	kipl::base::TImage<double,1> mFilter;
	kipl::math::fft::FFTBase *fft;
};

class STDPREPROCMODULESSHARED_EXPORT ProjectionFilterSingle :
	public ProjectionFilterBase
{
public:
    ProjectionFilterSingle(kipl::interactors::InteractionBase *interactor);
	virtual ~ProjectionFilterSingle(void);

	const kipl::base::TImage<float,1> GetFilterArray() {return mFilter;}
private:
	virtual void BuildFilter(const size_t N);
	virtual void PreparePadding(const size_t nImage, const size_t nFilter);
	virtual void FilterProjection(kipl::base::TImage<float,2> & img);
	size_t Pad(float const * const pSrc, const size_t nSrcLen, float *pDest, const size_t nDestLen);
	
	FilterType mFilterType;
	kipl::base::TImage<float,1> mFilter;
	kipl::base::TImage<float,1> mPadData;

	kipl::math::fft::FFTBaseFloat *fft;

	size_t nInsert;
};

ostream STDPREPROCMODULESSHARED_EXPORT & operator<<(ostream & s, ProjectionFilterBase::FilterType ft);
void STDPREPROCMODULESSHARED_EXPORT string2enum(const std::string str, ProjectionFilterBase::FilterType &ft);
std::string STDPREPROCMODULESSHARED_EXPORT enum2string(const ProjectionFilterBase::FilterType &ft);

#endif

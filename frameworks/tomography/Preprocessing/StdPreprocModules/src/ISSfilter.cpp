/*
 * ISSfilter.cpp
 *
 *  Created on: Aug 5, 2011
 *      Author: kaestner
 */

#include "../include/StdPreprocModules_global.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#include <scalespace/ISSfilter2D.h>

#include <ParameterHandling.h>
#include <math/image_statistics.h>
#include <strings/miscstring.h>

#include "../include/ISSfilter.h"

ISSfilter::ISSfilter() :
	PreprocModuleBase("ISSfilter"),
	m_fTau(0.01f),
	m_nN(10),
	m_fLambda(0.01f),
	m_fAlpha(0.0025f),
	m_bAutoScale(false),
	m_bScaleData(false),
	m_fSlope(1.0f),
	m_fIntercept(0.0f),
	m_bErrorCurve(false),
	m_fErrorCurve(NULL)
{


}

ISSfilter::~ISSfilter() {

}

int ISSfilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	m_fTau    = GetFloatParameter(parameters,"tau");
	m_nN      = GetIntParameter(parameters,"N");
	m_fLambda = GetFloatParameter(parameters,"lambda");
	m_fAlpha  = GetFloatParameter(parameters,"alpha");
	m_bScaleData = kipl::strings::string2bool(GetStringParameter(parameters,"scaledata"));
	m_bAutoScale = kipl::strings::string2bool(GetStringParameter(parameters,"autoscale"));
	m_fSlope     = GetFloatParameter(parameters,"slope");
	m_fIntercept = GetFloatParameter(parameters,"intercept");

	return 0;
}

std::map<std::string, std::string> ISSfilter::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["tau"]=kipl::strings::value2string(m_fTau);
	parameters["N"]=kipl::strings::value2string(m_nN);
	parameters["lambda"]=kipl::strings::value2string(m_fLambda);
	parameters["alpha"]=kipl::strings::value2string(m_fAlpha);

	parameters["autoscale"]=kipl::strings::bool2string(m_bAutoScale);
	parameters["slope"]=kipl::strings::value2string(m_fSlope);
	parameters["intercept"]=kipl::strings::value2string(m_fIntercept);
	parameters["scaledata"]=kipl::strings::bool2string(m_bScaleData);

	return parameters;
}

bool ISSfilter::SetROI(size_t *roi)
{
	return false;
}

int ISSfilter::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
	akipl::scalespace::ISSfilter<float> filter;
	filter.ErrorCurve(m_bErrorCurve);

	ScaleData(img);
	filter.Process(img,m_fTau,m_fLambda,m_fAlpha,m_nN);
	RescaleData(img);
	if (m_bErrorCurve) {
		if (m_fErrorCurve!=NULL)
			delete [] m_fErrorCurve;
		m_fErrorCurve=new float[m_nN];
		for (int i=0; i<m_nN; i++)
			m_fErrorCurve[i]=static_cast<float>(filter.ErrorCurve()[i]);

	}
	return 0;
}

int ISSfilter::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    #pragma omp parallel
	{
		akipl::scalespace::ISSfilter<float> filter;

		kipl::base::TImage<float,2> slice(img.Dims());

		int N=static_cast<int>(img.Size(2));
		#pragma omp for
		for (int i=0; i<N; i++) {
			memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i),sizeof(float)*slice.Size());
			ScaleData(slice);
			filter.Process(slice,m_fTau,m_fLambda,m_fAlpha,m_nN);
			RescaleData(slice);

			memcpy(img.GetLinePtr(0,i),slice.GetDataPtr(),sizeof(float)*slice.Size());
		}
	}

	return 0;
}

int ISSfilter::ScaleData(kipl::base::TImage<float,2> &img)
{
	if (m_bScaleData) {
		float *pImg=img.GetDataPtr();
		if (m_bAutoScale) {
			std::pair<double,double> stats=kipl::math::statistics(pImg, img.Size());
			m_fIntercept=-static_cast<float>(stats.first);
			m_fSlope=1.0f/static_cast<float>(stats.second);
		}

		for (size_t i=0; i<img.Size(); i++) {
			pImg[i]=pImg[i]*m_fSlope+m_fIntercept;
		}

		return 1;
	}
	else
		return 0;
}

int ISSfilter::RescaleData(kipl::base::TImage<float,2> &img)
{
	if (m_bScaleData) {
		float *pImg=img.GetDataPtr();
		float invslope=1.0f/m_fSlope;
		for (size_t i=0; i<img.Size(); i++) {
			pImg[i]=(pImg[i]-m_fIntercept)*invslope;
		}

		return 1;
	}
	else
		return 0;
}

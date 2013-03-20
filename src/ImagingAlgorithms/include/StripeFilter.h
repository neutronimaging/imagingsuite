/*
 * StripeFilter.h
 *
 *  Created on: Aug 13, 2011
 *      Author: anders
 */

#ifndef STRIPEFILTER_H_
#define STRIPEFILTER_H_

#include <wavelets/wavelets.h>

#include <base/timage.h>
#include <math/mathconstants.h>
#include <fft/fftbase.h>
#include <logging/logger.h>
#ifdef _MSC_VER
#define DLL_EXPORT __declspec(dllexport)
#include <windows.h>
#else
#define DLL_EXPORT
#endif


class DLL_EXPORT StripeFilter {
private:
	kipl::logging::Logger logger;
public:
	StripeFilter(size_t const * const dims, std::string wname, size_t scale, float sigma);
	virtual ~StripeFilter();
	void Process(kipl::base::TImage<float,2> &img);

private:
	void CreateFilterWindow();
	void FilterStripes(kipl::base::TImage<float,2> &img);

	kipl::wavelets::WaveletTransform<float> m_wt;
	size_t m_nScale;
	float m_fSigma;
	size_t m_nFFTlength;

	float *m_pDamping;
	float *m_pLine;
	std::complex<float> *m_pCLine;

	size_t wdims[2];
	kipl::math::fft::FFTBaseFloat *fft;
};

#endif /* STRIPEFILTER_H_ */

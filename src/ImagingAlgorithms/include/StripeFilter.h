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
#include <iostream>
#ifdef _MSC_VER
#define DLL_EXPORT __declspec(dllexport)
#include <windows.h>
#else
#define DLL_EXPORT
#endif

namespace ImagingAlgorithms {

enum StripeFilterOperation {
	VerticalComponentZero = 0,
	VerticalComponentFFT
};

class DLL_EXPORT StripeFilter {
private:
	kipl::logging::Logger logger;
public:
	StripeFilter(size_t const * const dims, std::string wname, size_t scale, float sigma);
	virtual ~StripeFilter();
	void Process(kipl::base::TImage<float,2> &img, StripeFilterOperation op=VerticalComponentFFT);

private:
	void CreateFilterWindow();
	void FilterVerticalStripes(kipl::base::TImage<float,2> &img, size_t level=0);
	void VerticalStripesZero(kipl::base::TImage<float,2> &img);
	void GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len);
	void SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len);

	kipl::wavelets::WaveletTransform<float> m_wt;
	size_t m_nScale;
	float m_fSigma;
	size_t m_nFFTlength[16];

	float *m_pDamping[16];
	float *m_pLine;
	std::complex<float> *m_pCLine;

	size_t wdims[2];
	kipl::math::fft::FFTBaseFloat *fft[16];
};
}

std::string DLL_EXPORT enum2string(ImagingAlgorithms::StripeFilterOperation op);
void DLL_EXPORT string2enum(std::string str, ImagingAlgorithms::StripeFilterOperation &op);
std::ostream & operator<<(std::ostream & s, ImagingAlgorithms::StripeFilterOperation op);

#endif /* STRIPEFILTER_H_ */

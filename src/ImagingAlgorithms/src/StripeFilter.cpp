/*
 * StripeFilter.cpp
 *
 *  Created on: Aug 13, 2011
 *      Author: anders
 */
#include "stdafx.h"

#include "../include/StripeFilter.h"
#include "../include/ImagingException.h"

#include <io/io_tiff.h>
#include <fft/zeropadding.h>
#include <fft/fftbase.h>
#include <base/trotate.h>
#include <base/tpermuteimage.h>


#include <sstream>
#include <iostream>

namespace ImagingAlgorithms {


StripeFilter::StripeFilter(size_t const * const dims, std::string wname, size_t scale, float sigma) :
	logger("StripeFilter"),
	m_wt(wname),
	m_nScale(scale),
	m_fSigma(sigma),
	m_nFFTlength(kipl::math::fft::NextPower2(1.25*dims[1])),
	m_pDamping(NULL),
	m_pLine(NULL),
	m_pCLine(NULL),
	fft(NULL)
{
	std::ostringstream msg;

	size_t N=2*m_nFFTlength;
	m_pDamping = new float[N];
	m_pLine    = new float[N];
	m_pCLine   = new complex<float>[N];

	
	fft=new kipl::math::fft::FFTBaseFloat(&N,1);

	CreateFilterWindow();
}

StripeFilter::~StripeFilter() {
	if (m_pDamping!=0)
		delete [] m_pDamping;

	if (m_pLine!=0)
		delete [] m_pLine;

	if (m_pCLine!=0)
		delete [] m_pCLine;

	if (fft!=NULL) {
		delete fft;
	}
}


void StripeFilter::CreateFilterWindow()
{
	const float s=-1.0f/(2.0f*m_fSigma*m_fSigma);
	const float scale=1.0f/(2.0f*m_nFFTlength);

	for (size_t i=0; i<2*m_nFFTlength; i++) {
		float w=static_cast<float>(i)/static_cast<float>(2*m_nFFTlength);
		m_pDamping[i]=(1.0f-exp(w*w*s))*scale;
	}
}

void StripeFilter::Process(kipl::base::TImage<float,2> &img)
{
	std::ostringstream msg;

	m_wt.transform(img,m_nScale);


	list< kipl::wavelets::WaveletQuad<float> >::iterator it;
	for (it=m_wt.data.begin(); it!=m_wt.data.end(); it++) {
		FilterVerticalStripes(it->v);
	}

	img=m_wt.synthesize();
}

void StripeFilter::FilterVerticalStripes(kipl::base::TImage<float,2> &img)
{

	const size_t nLines=img.Size(0);

	float *pData=img.GetDataPtr();

	for (size_t line=0; line<nLines; line++) {
		memset(m_pLine,0,sizeof(float)*2*m_nFFTlength);
		GetVerticalLine(pData,m_pLine,line,nLines,img.Size(1));

		(*fft)(m_pLine,m_pCLine);

		for (size_t i=0; i< 2*m_nFFTlength; i++) {
			m_pCLine[i]*=m_pDamping[i];
		}

		(*fft)(m_pCLine, m_pLine);
		SetVerticalLine(m_pLine,pData,line,nLines,img.Size(1));
	}
}

void StripeFilter::GetVerticalLine(float *pSrc, float *pLine, size_t pos, size_t stride, size_t len)
{
	float *pData=pSrc+pos;

	for (size_t i=0; i<len; i++, pData+=stride) {
		pLine[i]=*pData;
	}
}

void StripeFilter::SetVerticalLine(float *pLine, float *pDest, size_t pos, size_t stride, size_t len)
{
	float *pData=pDest+pos;

	for (size_t i=0; i<len; i++, pData+=stride) {
		*pData=pLine[i];
	}
}

}

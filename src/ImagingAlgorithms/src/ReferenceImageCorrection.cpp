/*
 * ReferenceImageCorrection.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

//#include "stdafx.h"
#include "../include/ReferenceImageCorrection.h"
#include <cstring>

namespace ImagingAlgorithms {

ReferenceImageCorrection::ReferenceImageCorrection() : logger("ReferenceImageCorrection") {
}

ReferenceImageCorrection::~ReferenceImageCorrection() {
}

void ReferenceImageCorrection::LoadReferenceImages(std::string obname, size_t obcnt,
		std::string dcname, size_t dccnt,
		std::string bbname, size_t bbcnt,
		size_t *doseroi,
		size_t *bbroi)
{

}

void ReferenceImageCorrection::SetReferenceImages(kipl::base::TImage<float,2> *ob,
		kipl::base::TImage<float,2> *dc,
		kipl::base::TImage<float,2> *bb,
		size_t *doseroi,
		size_t *bbroi)
{
	m_bHaveOpenBeam=m_bHaveDarkCurrent=m_bHaveBlackBody=false;
	m_bHaveDoseROI=m_bHaveBlackBodyROI=false;

	if (ob!=NULL) {
		m_bHaveOpenBeam=true;
		m_OpenBeam=*ob;
	}

	if (dc!=NULL) {
		m_bHaveDarkCurrent=true;
		m_DarkCurrent=*dc;
	}

	if (bb!=NULL) {
		m_bHaveBlackBody=true;
		m_BlackBody=*bb;
	}

	if (doseroi!=NULL) {
		m_bHaveDoseROI=true;
		memcpy(m_nDoseROI,doseroi,4*sizeof(size_t));
	}

	if (bbroi!=NULL) {
		m_bHaveBlackBodyROI=true;
		memcpy(m_nBlackBodyROI,bbroi,4*sizeof(size_t));
	}

	PrepareReferences();
	PrepareBlackBody(10.0f);
}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,2> &img, float dose)
{
	if (m_bComputeLogarithm)
		ComputeLogNorm(img,dose);
	else
		ComputeNorm(img,dose);
}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,3> &img, float *dose)
{
	kipl::base::TImage<float, 2> slice(img.Dims());

	for (size_t i=0; i<img.Size(2); i++) {
		memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i), sizeof(float)*slice.Size());

		Process(slice,dose[i]);
	}
}

void ReferenceImageCorrection::PrepareBlackBody(float w)
{

}

void ReferenceImageCorrection::PrepareReferences()
{
	if (m_bComputeLogarithm) {

	}
	else {

	}
}

void ReferenceImageCorrection::ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose)
{

}

void ReferenceImageCorrection::ComputeNorm(kipl::base::TImage<float,2> &img, float dose)
{

}

}

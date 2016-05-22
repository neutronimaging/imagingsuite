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

ReferenceImageCorrection::ReferenceImageCorrection() :
    logger("ReferenceImageCorrection"),
    m_bHaveOpenBeam(false),
    m_bHaveDarkCurrent(false),
    m_bHaveBlackBody(false),
    m_bComputeLogarithm(true),
    m_fOpenBeamDose(1.0f),
    m_bHaveDoseROI(false),
    m_bHaveBlackBodyROI(false),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage)
{
    m_nDoseROI[0]=0;
    m_nDoseROI[1]=0;
    m_nDoseROI[2]=0;
    m_nDoseROI[3]=0;

    m_nBlackBodyROI[0]=0;
    m_nBlackBodyROI[1]=0;
    m_nBlackBodyROI[2]=0;
    m_nBlackBodyROI[3]=0;
}

ReferenceImageCorrection::~ReferenceImageCorrection()
{

}

void ReferenceImageCorrection::LoadReferenceImages(std::string obname, size_t firstob, size_t obcnt,
        std::string dcname, size_t firstdc, size_t dccnt,
        std::string bbname, size_t firstbb, size_t bbcnt, size_t *roi,
        size_t *doseroi,
        size_t *bbroi)
{
    m_bHaveBlackBody   = false;
    m_bHaveOpenBeam    = false;
    m_bHaveDarkCurrent = false;
    m_bHaveDoseROI = doseroi != nullptr ? true : false;
    m_bHaveBlackBodyROI = bbroi != nullptr ? true : false;

    ImagingAlgorithms::AverageImage avg;

    size_t ndose=max(obcnt,max(dccnt,bbcnt));
    ndose = 0<ndose ? ndose : 1;

    float *doselist=new float[ndose];

    if ((!obname.empty()) && 0<obcnt) {
        kipl::base::TImage<float,3> img;

        // Load images

        if (m_bHaveDoseROI) {
            // load dose
            m_OpenBeam=avg(img,m_AverageMethod,doselist);
        }
        else {
            m_OpenBeam=avg(img,m_AverageMethod,nullptr);
        }

        m_bHaveOpenBeam=true;
    }

    if ((!dcname.empty()) && 0<dccnt) {
        kipl::base::TImage<float,3> img;

        // Load dark current

        if (m_bHaveDoseROI) {
            // load dose
            m_DarkCurrent=avg(img,m_AverageMethod,doselist);
        }
        else {
            m_DarkCurrent=avg(img,m_AverageMethod,nullptr);
        }

        m_bHaveDarkCurrent=true;
    }

    if ((!bbname.empty()) && 0<bbcnt) {
        kipl::base::TImage<float,3> img;
        // Load bb images

        if (m_bHaveDoseROI) {
            // load dose
            m_BlackBody=avg(img,m_AverageMethod,doselist);
        }
        else {
            m_BlackBody=avg(img,m_AverageMethod,nullptr);
        }

        m_bHaveBlackBody=true;
    }

    delete [] doselist;
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

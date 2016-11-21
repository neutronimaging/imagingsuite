/*
 * ReferenceImageCorrection.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

//#include "stdafx.h"
#include "../include/ReferenceImageCorrection.h"
#include "../include/ImagingException.h"
#include <strings/miscstring.h>

//#include <cstring>
#include <map>
#include <sstream>
#include <math/median.h>

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
    std::cout << "ciao image corrector" << std::endl;
}

ReferenceImageCorrection::~ReferenceImageCorrection()
{

}

void ReferenceImageCorrection::LoadReferenceImages(std::string path, std::string obname, size_t firstob, size_t obcnt,
        std::string dcname, size_t firstdc, size_t dccnt,
        std::string bbname, size_t firstbb, size_t bbcnt, size_t *roi,
        size_t *doseroi,
        size_t *bbroi)
{

    std::cout << "ciao process " << std::endl;

}

void ReferenceImageCorrection::SetReferenceImages(kipl::base::TImage<float,2> *ob,
        kipl::base::TImage<float,2> *dc,
        kipl::base::TImage<float,2> *bb,
        float dose_OB,
        float dose_DC,
        float dose_BB)
{
	m_bHaveOpenBeam=m_bHaveDarkCurrent=m_bHaveBlackBody=false;
	m_bHaveDoseROI=m_bHaveBlackBodyROI=false;

	if (ob!=NULL) {
		m_bHaveOpenBeam=true;
		m_OpenBeam=*ob;
        std::cout << "have OB!" << std::endl;
	}

	if (dc!=NULL) {
		m_bHaveDarkCurrent=true;
		m_DarkCurrent=*dc;
        std::cout << "have DC!" << std::endl;
	}

	if (bb!=NULL) {
		m_bHaveBlackBody=true;
		m_BlackBody=*bb;
        std::cout << "have BB!" << std::endl;
	}

    if (dose_OB!=NULL) {
		m_bHaveDoseROI=true;
        m_fOpenBeamDose = dose_OB;
                std::cout << " have dose roi!  " << std::endl;
        //        std::cout << dose_OB << std::endl;


	}

    if (dose_DC!=NULL){
       m_fDarkDose = dose_DC;
    }

    if (dose_BB!=NULL) {
		m_bHaveBlackBodyROI=true;
//		memcpy(m_nBlackBodyROI,dose_BB,4*sizeof(size_t));
	}

	PrepareReferences();
	PrepareBlackBody(10.0f);
}

kipl::base::TImage<float,2> ReferenceImageCorrection::Process(kipl::base::TImage<float,2> &img, float dose)
{
    if (m_bComputeLogarithm) {
//        std::cout << "ComputeLogNorm" << std::endl;
        ComputeLogNorm(img,dose);
    }
	else
		ComputeNorm(img,dose);

    return img;

}

void ReferenceImageCorrection::Process(kipl::base::TImage<float,3> &img, float *dose)
{
	kipl::base::TImage<float, 2> slice(img.Dims());

	for (size_t i=0; i<img.Size(2); i++) {
        memcpy(slice.GetDataPtr(),img.GetLinePtr(0,i), sizeof(float)*slice.Size());
        Process(slice,dose[i]);
        memcpy(img.GetLinePtr(0,i), slice.GetDataPtr(), sizeof(float)*slice.Size()); // and copy the result back
	}
}

void ReferenceImageCorrection::PrepareBlackBody(float w)
{

}

void ReferenceImageCorrection::PrepareReferences()
{
    float dose=1.0f/(m_fOpenBeamDose-m_fDarkDose);
//    if (dose!=dose)
//        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    const int N=static_cast<int>(m_OpenBeam.Size());
    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

    if (m_bHaveDarkCurrent) {
        #pragma omp parallel for
        for (int i=0; i<N; i++) {
            float fProjPixel=pFlat[i]-pDark[i];
            if (fProjPixel<=0)
                pFlat[i]=0;
            else

                pFlat[i]=log(fProjPixel*dose);

        }
    }
    else {
        #pragma omp parallel for
        for (int i=0; i<N; i++) {
            float fProjPixel=pFlat[i];
            if (fProjPixel<=0)
                pFlat[i]=0;
            else
               pFlat[i]=log(fProjPixel*dose);
        }
    }

// let's assume for now that I do compute the logarithm
//	if (m_bComputeLogarithm) {

//	}
//	else {

//	}

}

int ReferenceImageCorrection::ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose)
{
    dose = dose - m_fDarkDose;
    dose = log(dose<1 ? 1.0f : dose);

    int N=static_cast<int>(img.Size(0)*img.Size(1));

    float *pFlat=m_OpenBeam.GetDataPtr();
    float *pDark=m_DarkCurrent.GetDataPtr();

//    std::cout << m_bHaveDarkCurrent << " " << m_bHaveOpenBeam << std::endl;
//    std::cout << "number of pixels: " << N << std::endl;

    if (m_bHaveDarkCurrent) {
        if (m_bHaveOpenBeam) {
//                #pragma omp parallel for firstprivate(pFlat,pDark)

                    float *pImg=img.GetDataPtr();

                    #pragma omp parallel for
                    for (int i=0; i<N; i++) {

//                        if (i==0) std::cout<< "sono dentro" << std::endl; // ci entra..

                        float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                            pImg[i]=pFlat[i]-log(fProjPixel)+dose;

                    }

        }
        else {
//            #pragma omp parallel for firstprivate(pDark)

              float *pImg=img.GetDataPtr();

                #pragma omp parallel for
                for (int i=0; i<N; i++) {
                    float fProjPixel=(pImg[i]-pDark[i]);
                    if (fProjPixel<=0)
                        pImg[i]=0;
                    else
                       pImg[i]=-log(fProjPixel)+dose;
                }

        }
    }

    return 1;
}

void ReferenceImageCorrection::ComputeNorm(kipl::base::TImage<float,2> &img, float dose)
{

}



}




void string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod> methods;
    methods["LogNorm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm;
    methods["BBLogNorm"] = ImagingAlgorithms::ReferenceImageCorrection::ReferenceBBLogNorm; // to be updated with more options

    if (methods.count(str)==0)
        throw ImagingException("The key string does not exist for eReferenceMethod",__FILE__,__LINE__);

    erm=methods[str];


}

std::string enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm)
{
    std::string str;

    switch (erm) {
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceLogNorm: str="LogNorm"; break;
        case ImagingAlgorithms::ReferenceImageCorrection::ReferenceBBLogNorm: str="BBLogNorm"; break;
        default: throw ImagingException("Unknown reference method in enum2string for eReferenceMethod",__FILE__,__LINE__);
    }
    return  str;

}

std::ostream & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm)
{
    s<<enum2string(erm);

    return s;
}



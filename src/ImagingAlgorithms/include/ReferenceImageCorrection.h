/*
 * ReferenceImageCorrection.h
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

#ifndef IMGALG_REFERENCEIMAGECORRECTION_H_
#define IMGALG_REFERENCEIMAGECORRECTION_H_
#include "ImagingAlgorithms_global.h"

//#include "stdafx.h"
#include <base/timage.h>
#include <logging/logger.h>
#include <string>
#include "../include/averageimage.h"


namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT ReferenceImageCorrection {
protected:
	kipl::logging::Logger logger;
public:
    enum eReferenceMethod {
            ReferenceLogNorm,
            ReferenceBBLogNorm // to be updated with more options, as we decided in the BB meeting
    };

	ReferenceImageCorrection();
	virtual ~ReferenceImageCorrection();

    void LoadReferenceImages(std::string path, std::string obname, size_t firstob, size_t obcnt,
            std::string dcname, size_t firstdc, size_t dccnt,
            std::string bbname, size_t firstbb, size_t bbcnt,
            size_t *roi,
            size_t *doseroi);

    void SetReferenceImages(kipl::base::TImage<float,2> *ob,
            kipl::base::TImage<float,2> *dc,
            kipl::base::TImage<float,2> *bb,
            float dose_OB,
            float dose_DC,
            float dose_BB);

//    void SetReferenceImages(kipl::base::TImage<float,3> *ob,
//            kipl::base::TImage<float,3> *dc,
//            kipl::base::TImage<float,3> *bb,
//            size_t *doseroi,
//            size_t *bbroi);

	void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;}

    kipl::base::TImage<float,2>  Process(kipl::base::TImage<float,2> &img, float dose);
	void Process(kipl::base::TImage<float,3> &img, float *dose);

    kipl::base::TImage<float,2> PrepareBlackBodyOpenBeam(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb);



protected:
	void PrepareReferences();
    kipl::base::TImage<float,2> PrepareBlackBodySample(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb);
    void SegmentBlackBody(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask);
    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img, kipl::base::TImage<float,2>&interpolated_img); // remove interpolated_img at the end
    void InterpolateBlackBodyImage(float *parameters, kipl::base::TImage<float,2> &interpolated_img);
    float ComputeInterpolationError(kipl::base::TImage<float,2>&interpolated_img, kipl::base::TImage<float,2>&mask, kipl::base::TImage<float, 2> &img);

    int ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose);
    void ComputeNorm(kipl::base::TImage<float,2> &img, float dose);
//    void GetDose(kipl::base::TImage<float,2> &img, size_t *roi);
    int* repeat_matrix(int* source, int count, int expand);


	bool m_bHaveOpenBeam;
	bool m_bHaveDarkCurrent;
	bool m_bHaveBlackBody;

	bool m_bComputeLogarithm;

	kipl::base::TImage<float,2> m_OpenBeam;
	kipl::base::TImage<float,2> m_DarkCurrent;
	kipl::base::TImage<float,2> m_BlackBody;
    kipl::base::TImage<float,2> m_OB_BB_Interpolated;


    kipl::base::TImage<float,2> m_OpenBeam_all;
    kipl::base::TImage<float,2> m_DarkCurrent_all;
    kipl::base::TImage<float,2> m_BlackBody_all;
    kipl::base::TImage<float,2> m_OB_BB_Interpolated_all;

	float m_fOpenBeamDose;
    float m_fDarkDose;
    float m_fBlackDose;
	bool m_bHaveDoseROI;
	bool m_bHaveBlackBodyROI;

    ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod;

    size_t m_nDoseROI[4]; // probably don't need this
    size_t m_nBlackBodyROI[4]; // neither this

};

}

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm);

#endif /* REFERENCEIMAGECORRECTION_H_ */

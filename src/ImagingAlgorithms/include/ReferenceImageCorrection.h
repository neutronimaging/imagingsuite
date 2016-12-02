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
            bool useBB,
            float dose_OB,
            float dose_DC,
            bool normBB);

    void SetInterpParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj); /// set interpolation parameters to be used for BB image computation
    void SetBBInterpRoi(size_t *roi); ///set roi to be used for computing interpolated values

	void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;}
    void SetRadius(size_t x) {radius=x;}
    void SetTau (float x) {tau=x;}
    void setDiffRoi (int *roi) {memcpy(m_diffBBroi, roi, sizeof(int)*4);}
//    void setBBflatdose (float dose) {m_fBlackDose=dose;}
//    void setBBsampledose (float *doselist) {m_fBlackDoselist=doselist;}


    kipl::base::TImage<float,2>  Process(kipl::base::TImage<float,2> &img, float dose);
	void Process(kipl::base::TImage<float,3> &img, float *dose);

    float* PrepareBlackBodyImage(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask); /// segments normalized image (bb-dark)/(flat-dark) to create mask and then call ComputeInterpolationParameter
    float* PrepareBlackBodyImagewithMask(kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2>&mask); /// uses a predefined mask and then call ComputeInterpolationParameter
    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img); /// compute interpolation parameters from img and mask

protected:
	void PrepareReferences();
    kipl::base::TImage<float,2> PrepareBlackBodySample(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb);
    kipl::base::TImage<float,2>  InterpolateBlackBodyImage(float *parameters, size_t *roi); /// compute interpolated image from parameters
    float ComputeInterpolationError(kipl::base::TImage<float,2>&interpolated_img, kipl::base::TImage<float,2>&mask, kipl::base::TImage<float, 2> &img); /// compute interpolation error from interpolated image, original imae and mask that highlights the pixels to be considered
    void SegmentBlackBody(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask); /// apply Otsu segmentation to img and create mask, it also performs some image cleaning:

    float *InterpolateParameters(float *param, size_t n, size_t step);

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
    kipl::base::TImage<float,2> m_BB_sample_Interpolated; // computed in process 3d every time. but in the right way!


    // do i need those here?
    kipl::base::TImage<float,2> m_OpenBeam_all;
    kipl::base::TImage<float,2> m_DarkCurrent_all;
    kipl::base::TImage<float,2> m_BlackBody_all;
//    kipl::base::TImage<float,2> m_OB_BB_Interpolated_all; // don't need this

	float m_fOpenBeamDose;
    float m_fDarkDose;
//    float m_fBlackDose;
//    float *m_fBlackDoselist;
	bool m_bHaveDoseROI;
    bool m_bHaveBBDoseROI;
	bool m_bHaveBlackBodyROI;

    float *ob_bb_parameters; // they could be double ?
    float *sample_bb_parameters;
    float *sample_bb_interp_parameters;

    ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod;

    size_t m_nDoseROI[4]; // probably don't need this, because roi is computed in the pre-processing module..
    size_t m_nBlackBodyROI[4]; /// roi to be used for computing interpolated BB images, roi position is relative to the image projection roi, on which interpolation parameters are computed
    int m_diffBBroi[4]; /// difference between BB roi and projection roi, important when computing interpolation parameters
    size_t m_nBBimages; /// number of images with BB sample that are available
    size_t radius; /// radius value for BB mask creation
    float tau; /// mean pattern transmission
};

}

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm);

#endif /* REFERENCEIMAGECORRECTION_H_ */

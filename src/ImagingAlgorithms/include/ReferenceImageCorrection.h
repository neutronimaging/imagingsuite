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
#include <map>


namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT ReferenceImageCorrection {
protected:
	kipl::logging::Logger logger;
public:
    enum eReferenceMethod {
            ReferenceLogNorm,
            ReferenceBBLogNorm
    }; /// Options for Referencing method. not yet used

    enum eBBOptions {
      Interpolate,
      Average,
      OneToOne
    }; /// Options for BB image handling

    enum eInterpMethodX{
        SecondOrder_x,
        FirstOrder_x,
        ZeroOrder_x
    }; /// Options for BB mask interpolation, x direction

    enum eInterpMethodY{
        SecondOrder_y,
        FirstOrder_y,
        ZeroOrder_y
    }; /// Options for BB mask interpolation, y direction



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
            bool normBB,
            size_t *roi);

    void SetInterpParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj, eBBOptions ebo); /// set interpolation parameters to be used for BB image computation
    void SetBBInterpRoi(size_t *roi); ///set roi to be used for computing interpolated values

    void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;} /// set bool value for computing -logarithm
    void SetRadius(size_t x) {radius=x;} /// set the radius used to define subset of segmented BBs
    void SetTau (float x) {tau=x;} /// set value of tau
    void setDiffRoi (int *roi) {memcpy(m_diffBBroi, roi, sizeof(int)*4);} /// set diffroi, which is the difference between BBroi and the Projection roi
    void SetPBvariante (bool x) {bPBvariante = x; } /// set bool value for computation of pierre's variante. at the moment it is hidden from the Gui and it is intended to be set as dafault true

    void SetInterpolationOrderX(eInterpMethodX eim_x);
    void SetInterpolationOrderY(eInterpMethodY eim_y);

    void SetAngles(float *ang, size_t nProj, size_t nBB); /// set angles and number of proj and images with BB, to be used for more general interpolation
    void SetDoseList(float *doselist); /// set dose list for sample images in the BB dose roi, it has to be called after SetAngles for the right definition of m_nProj

    kipl::base::TImage<float,2>  Process(kipl::base::TImage<float,2> &img, float dose); /// 2D process
    void Process(kipl::base::TImage<float,3> &img, float *dose); /// 3D process

    float* PrepareBlackBodyImage(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask); /// segments normalized image (bb-dark)/(flat-dark) to create mask and then call ComputeInterpolationParameter
    float* PrepareBlackBodyImage(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask, float &error); /// segments normalized image (bb-dark)/(flat-dark) to create mask and then call ComputeInterpolationParameter, finally computes interpolation error
    float* PrepareBlackBodyImagewithMask(kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2>&mask); /// uses a predefined mask and then call ComputeInterpolationParameter
    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img); /// compute interpolation parameters from img and mask
    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask, kipl::base::TImage<float,2>&img, float &error); /// compute interpolation parameters from img and mask and give as output interpolation error
    kipl::base::TImage<float,2>  InterpolateBlackBodyImage(float *parameters, size_t *roi); /// compute interpolated image from parameters
    float ComputeInterpolationError(kipl::base::TImage<float,2>&interpolated_img, kipl::base::TImage<float,2>&mask, kipl::base::TImage<float, 2> &img); /// compute interpolation error from interpolated image, original image and mask that highlights the pixels to be considered


protected:
    void PrepareReferences(); /// old version with references image preparation, without BB
    void PrepareReferencesBB(); /// prepare reference images in case of BB

    float *SolveLinearEquation(std::map<std::pair<size_t,size_t>, float> &values, float &error);

    void SegmentBlackBody(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask); /// apply Otsu segmentation to img and create mask, it also performs some image cleaning:

    float *InterpolateParameters(float *param, size_t n, size_t step); /// Interpolate parameters assuming the BB sample image are acquired uniformally with some step over the n Projection images
    float *InterpolateParametersGeneric(float *param); /// Interpolate parameters for generic configuration of number of BB sample images and projection data, it assumes first SetAngle is called
    float *ReplicateParameters(float *param, size_t n); /// Replicate interpolation parameter, to be used for the Average method

    int ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose);
    void ComputeNorm(kipl::base::TImage<float,2> &img, float dose);
    int* repeat_matrix(int* source, int count, int expand); /// repeat matrix. not used.
    float computedose(kipl::base::TImage<float,2>&img); /// duplicate.. to move in timage probably or something like this


	bool m_bHaveOpenBeam;
	bool m_bHaveDarkCurrent;
	bool m_bHaveBlackBody;
	bool m_bComputeLogarithm;

    kipl::base::TImage<float,2> m_OpenBeam;
	kipl::base::TImage<float,2> m_DarkCurrent;
	kipl::base::TImage<float,2> m_BlackBody;
    kipl::base::TImage<float,2> m_OB_BB_Interpolated;
    kipl::base::TImage<float,2> m_BB_sample_Interpolated; // computed in process 3d every time. but in the right way!
    kipl::base::TImage<float,2> m_DoseBBsample_image;
    kipl::base::TImage<float,2> m_DoseBBflat_image;



	float m_fOpenBeamDose;
    float m_fDarkDose;
	bool m_bHaveDoseROI;
    bool m_bHaveBBDoseROI;
	bool m_bHaveBlackBodyROI;
    bool bPBvariante;

    float *ob_bb_parameters; /// interpolation parameters for the OB BB image
    float *sample_bb_parameters; /// interpolation parameters for the sample image with BB
    float *sample_bb_interp_parameters; /// interpolation parameters for the sample image for each projection angle

    ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod; /// method used for image averaging (options: sum, mean, max, weightedmean, median and average)

    eInterpMethodX m_IntMeth_x;
    eInterpMethodY m_IntMeth_y;


    int a,b,c,d,e,f; /// weights for interpolation scheme, used to set different combined order

    size_t m_nDoseROI[4]; /// roi to be used for dose computation on BB images ("big roi" I would say..)
    size_t m_nBlackBodyROI[4]; /// roi to be used for computing interpolated BB images, roi position is relative to the image projection roi, on which interpolation parameters are computed
    int m_diffBBroi[4]; /// difference between BB roi and projection roi, important when computing interpolation parameters
    size_t m_nBBimages; /// number of images with BB sample that are available
    size_t m_nProj; /// number of images that are needed after interpolation
    float angles[4]; /// first and last angles of nProj and nBBImages, respectively
    size_t radius; /// radius value for BB mask creation
    float tau; /// mean pattern transmission
    float *dosesamplelist; /// list of doses for sample images computed at dose BB roi, to be used in the Interpolate BBOption
};

}

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions ebo);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX &eim_x);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX &eim_x);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodX eim_x);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY &eim_y);

std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY &eim_y);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethodY eim_y);


#endif /* REFERENCEIMAGECORRECTION_H_ */

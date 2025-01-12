//<LICENSE>

#ifndef REFERENCEIMAGECORRECTION_H
#define REFERENCEIMAGECORRECTION_H
#include "ImagingAlgorithms_global.h"

#include <map>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>
#include "../include/averageimage.h"
#include <interactors/interactionbase.h>


namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT ReferenceImageCorrection {
protected:
	kipl::logging::Logger logger;
public:
    enum eReferenceMethod {
            ReferenceLogNorm,
            ReferenceNorm
    }; /// Options for Referencing method. not yet used

    enum eBBOptions {
      noBB,
      Interpolate,
      Average,
      OneToOne,
      ExternalBB
    }; /// Options for BB image handling

    enum eInterpOrderX{
        SecondOrder_x,
        FirstOrder_x,
        ZeroOrder_x
    }; /// Options for BB mask interpolation, x direction

    enum eInterpOrderY{
        SecondOrder_y,
        FirstOrder_y,
        ZeroOrder_y
    }; /// Options for BB mask interpolation, y direction

    enum eInterpMethod{
        Polynomial,
        ThinPlateSplines
    }; /// Options for background interpolation


    enum eMaskCreationMethod
    {
        otsuMask,
        manuallyThresholdedMask,
        userDefinedMask,
        referenceFreeMask
    };

    ReferenceImageCorrection(kipl::interactors::InteractionBase *interactor = nullptr);
    ~ReferenceImageCorrection();

    void SetReferenceImages(kipl::base::TImage<float,2> *ob,
            kipl::base::TImage<float,2> *dc,
            bool  useBB,
            bool  useExtBB,
            bool  useSingleExtBB,
            float dose_OB,
            float dose_DC,
            bool  normBB,
            const std::vector<size_t> &roi,
            const std::vector<size_t> &doseroi);

    void SetMaskCreationMethod(eMaskCreationMethod eMaskMethod, kipl::base::TImage<float,2> &mask);

    void SetInterpParameters(float *ob_parameter,
                             float *sample_parameter,
                             size_t nBBSampleCount,
                             size_t nProj,
                             eBBOptions ebo); /// set interpolation parameters to be used for BB image computation
    void SetSplinesParameters(float *ob_parameter, float *sample_parameter, size_t nBBSampleCount, size_t nProj, eBBOptions ebo, int nBBs); /// set interpolation parameters to be used for BB image computation in the case of thin plate splines
    void SetBBInterpRoi(const std::vector<size_t> &roi); ///set roi to be used for computing interpolated values, it is a roi relative to the projection_roi, because the interpolation parameters are computed with respect to the projection_roi
    void SetBBInterpDoseRoi(const std::vector<size_t> &roi); /// set dose roi to be used for computing interpolated values. it is the dose roi relative the projection_roi, because the interpolation parameters are computed with respect to the projection_roi

    void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;} ///< set bool value for computing -logarithm
    void SetRadius(size_t x) {radius=x;} ///< set the radius used to define subset of segmented BBs
    void SetTau (float x) {tau=x;} ///< set value of tau
    void setDiffRoi (const std::vector<int> &roi) { m_diffBBroi = roi;} ///< set diffroi, which is the difference between BBroi and the Projection roi -- it should be now only the BBroi
    void SetPBvariante (bool x) {bPBvariante=x; } ///< set bool value for computation of pierre's variante. at the moment it is hidden from the Gui and it is intended to be set as dafault true
    void SetMinArea (size_t x) {min_area=x;} ///< set min area for BB segmentation

    void SetInterpolationOrderX(eInterpOrderX eim_x);
    void SetInterpolationOrderY(eInterpOrderY eim_y);
    void SetSplineObValues(std::map<std::pair<int, int>, float> &values) {spline_ob_values = values;} /// set map to be used for spline interpolation of ob images
    void SetSplineSampleValues(std::map<std::pair<int, int>, float> &values){spline_sample_values = values; } /// set map to used for spline interpolation of sample images
    void SetInterpolationMethod(eInterpMethod eint_meth) {m_InterpMethod = eint_meth;} /// set method for background interpolation, polynomial or splines
    void SetManualThreshold(bool bThresh, float value) {{bUseManualThresh=bThresh; thresh=value;}}

    void SetAngles(float *ang, size_t nProj, size_t nBB); ///< set angles and number of proj and images with BB, to be used for more general interpolation
    void SetDoseList(const std::vector<float> &doselist); /// set dose list for sample images in the BB dose roi, it has to be called after SetAngles for the right definition of m_nProj

    kipl::base::TImage<float,2>  Process(kipl::base::TImage<float,2> &img, float dose); ///< 2D process
    void Process(kipl::base::TImage<float,3> &img, float *dose); ///< 3D process

    float* PrepareBlackBodyImage(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask); /// segments normalized image (bb-dark)/(flat-dark) to create mask and then call ComputeInterpolationParameter
    float* PrepareBlackBodyImage(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask, float &error); /// segments normalized image (bb-dark)/(flat-dark) to create mask and then call ComputeInterpolationParameter, finally computes interpolation error
    float* PrepareBlackBodyImagewithSplines(kipl::base::TImage<float,2> &flat, kipl::base::TImage<float,2> &dark, kipl::base::TImage<float,2> &bb, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values); /// segments normalized image to create mask and then call interpolation with splines
    float* PrepareBlackBodyImagewithSplinesAndMask(kipl::base::TImage<float,2> &dark,
                                                   kipl::base::TImage<float,2> &bb,
                                                   kipl::base::TImage<float,2>&mask,
                                                   std::map<std::pair<int, int>, float> &values); /// uses a predefined mask and then call the thin plates spline interpolation
    float* PrepareBlackBodyImagewithMask(kipl::base::TImage<float,2> &dark,
                                         kipl::base::TImage<float,2> &bb,
                                         kipl::base::TImage<float,2>&mask); /// uses a predefined mask and then call ComputeInterpolationParameter
    float* PrepareBlackBodyImagewithMask(kipl::base::TImage<float,2> &dark,
                                         kipl::base::TImage<float,2> &bb,
                                         kipl::base::TImage<float,2>&mask,
                                         float &error); /// uses a predefined mask and returns interpolation parameters and interpolation error

    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask,
                                          kipl::base::TImage<float,2>&img); /// compute interpolation parameters from img and mask
    float* ComputeInterpolationParameters(kipl::base::TImage<float,2>&mask,
                                          kipl::base::TImage<float,2>&img,
                                          float &error); /// compute interpolation parameters from img and mask and give as output interpolation error
    kipl::base::TImage<float,2>  InterpolateBlackBodyImage(float *parameters,
                                                           const std::vector<size_t> &roi); /// compute interpolated image from polynomial parameters
    kipl::base::TImage<float,2> InterpolateBlackBodyImagewithSplines(float *parameters, std::map<std::pair<int,int>,float> &values, const std::vector<size_t> &roi); /// compute interpolated image from splines parameters
    float ComputeInterpolationError(kipl::base::TImage<float,2>&interpolated_img, kipl::base::TImage<float,2>&mask, kipl::base::TImage<float, 2> &img); /// compute interpolation error from interpolated image, original image and mask that highlights the pixels to be considered

    void SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 3> &bb_sample_ext, float &dose, const std::vector<float> &doselist); /// set the BB externally computed images and corresponding doses
    void SetExternalBBimages(kipl::base::TImage<float, 2> &bb_ext, kipl::base::TImage<float, 2> &bb_sample_ext, float &dose, float &dose_s); /// set the BB externally computed images and corresponding doses, case of single file also for the sample background
    void SetComputeMinusLog(bool value) {m_bComputeLogarithm = value;}
    void SaveBG(bool value, string path, string obname, string filemask);
    void SetInteractor(kipl::interactors::InteractionBase *interactor);


protected:
    void PrepareReferences(); /// old version with references image preparation, without BB
    void PrepareReferencesBB(); /// prepare reference images in case of BB
    void PrepareReferencesExtBB(); /// prepare reference images in case of externally created BB

    float *SolveLinearEquation(std::map<std::pair<int, int>, float> &values, float &error);
    float *SolveThinPlateSplines(std::map<std::pair<int,int>,float> &values);

    void SegmentBlackBody(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask); /// apply Otsu segmentation to img and create mask, it also performs some image cleaning
    void SegmentBlackBody(kipl::base::TImage<float,2> &normimg, kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values); /// apply Otsu segmentation to img and create mask, it also performs some image cleaning and creates a list with position of BB centroids
    void ComputeBlackBodyCentroids(kipl::base::TImage<float,2> &img, kipl::base::TImage<float,2> &mask, std::map<std::pair<int, int>, float> &values); /// uses the same mask to create centroids values list

    float *InterpolateParameters(float *param, size_t n, size_t step); /// Interpolate parameters assuming the BB sample image are acquired uniformally with some step over the n Projection images
    float *InterpolateParametersGeneric(float *param); /// Interpolate parameters for generic configuration of number of BB sample images and projection data, it assumes first SetAngle is called
    float *InterpolateSplineGeneric(float *param, int nBBs); /// Interpolate parameters in the case of thin plates splines
    float *ReplicateParameters(float *param, size_t n); /// Replicate interpolation parameter, to be used for the Average method
    float *ReplicateSplineParameters(float *param, size_t n, int nBBs); /// Replicate interpolation parameters, to be used for the Average method and Spline option

    int ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose);
    void ComputeNorm(kipl::base::TImage<float,2> &img, float dose);
    int* repeat_matrix(int* source, int count, int expand); /// repeat matrix. not used.
    float computedose(kipl::base::TImage<float,2>&img); /// duplicate.. to move in timage probably or something like this

	bool m_bHaveOpenBeam;
	bool m_bHaveDarkCurrent;
	bool m_bHaveBlackBody;
    bool m_bHaveExternalBlackBody;
	bool m_bComputeLogarithm;
    bool bUseManualThresh;
    bool bSaveBG;

    std::string pathBG; /// path for saving BGs
    std::string flatname_BG; /// filename for saving the open beam BG
    std::string filemask_BG; /// filemask for saving the computed sample BGs

    kipl::base::TImage<float,2> m_OpenBeam;
    kipl::base::TImage<float,2> m_OpenBeamforBB;
	kipl::base::TImage<float,2> m_DarkCurrent;
	kipl::base::TImage<float,2> m_BlackBody;
    kipl::base::TImage<float,2> m_OB_BB_Interpolated;
    kipl::base::TImage<float,2> m_BB_sample_Interpolated;
    kipl::base::TImage<float,2> m_DoseBBsample_image;
    kipl::base::TImage<float,2> m_DoseBBflat_image;
    kipl::base::TImage<float,2> m_OB_BB_ext; /// externally computed OB image with BBs
    kipl::base::TImage<float,3> m_BB_sample_ext; /// externally computed sample image with BBs
    kipl::base::TImage<float,2> m_BB_slice_ext; /// externally computed slice of sample image with BBs

	float m_fOpenBeamDose;
    float m_fDarkDose;
	bool m_bHaveDoseROI;
    bool m_bHaveBBDoseROI;
	bool m_bHaveBlackBodyROI;
    bool bPBvariante;
    bool bExtSingleFile;                /// boolean value on the use of a single file for sample background correction

    float fdoseOB_ext;                  /// dose value in externally computed background for open beam image
    float fdoseS_ext;                   /// dose value in externally computed background for single sample image
    std::vector<float> fdose_ext_list;  /// dose value list in externally computed sample with BB image in dose roi
    float fdose_ext_slice;              /// dose value of current slice from fdose_ext_list

    float *ob_bb_parameters;            /// interpolation parameters for the OB BB image
    float *sample_bb_parameters;        /// interpolation parameters for the sample image with BB
    float *sample_bb_interp_parameters; /// interpolation parameters for the sample image for each projection angle

    ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod m_MaskMethod;
    kipl::base::TImage<float,2> m_MaskImage;
    ImagingAlgorithms::AverageImage::eAverageMethod m_AverageMethod; /// method used for image averaging (options: sum, mean, max, weightedmean, median and average)

    eInterpOrderX m_IntMeth_x;
    eInterpOrderY m_IntMeth_y;

    eInterpMethod m_InterpMethod;


    int a,b,c,d,e,f;                     /// weights for interpolation scheme, used to set different combined order

    std::vector<size_t> m_nDoseROI;      /// roi to be used for dose computation on BB images ("big roi" I would say..)
    std::vector<size_t> m_nROI;          /// actual roi onto compute the background images
    std::vector<size_t> m_nBlackBodyROI; /// roi to be used for computing interpolated BB images, roi position is relative to the image projection roi, on which interpolation parameters are computed
    std::vector<int>    m_diffBBroi;     /// difference between BB roi and projection roi, important when computing interpolation parameters
    std::vector<size_t> m_nDoseBBRoi;    /// "big roi" to be used for dose computation on BB images, this is now relative to projection roi coordinates, due to the interpolation scheme
    std::vector<float> angles;           /// first and last angles of nProj and nBBImages, respectively
    std::vector<float> dosesamplelist;   /// list of doses for sample images computed at dose BB roi, to be used in the Interpolate BBOption

    size_t m_nBBimages;                  /// number of images with BB sample that are available
    size_t m_nProj;                      /// number of images that are needed after interpolation
    size_t radius;                       /// radius value for BB mask creation
    size_t min_area;                     /// min area for BB segmentation

    float tau;                           /// mean pattern transmission
    float thresh;                        /// manual threshold

    std::map<std::pair<int, int>, float> spline_ob_values;     /// map to be used for interpolation with splines and ob image. should be in principle the same number of images with BB, i start now with 1
    std::map<std::pair<int, int>, float> spline_sample_values; /// map to be used for interpolation with splines and sample image

    kipl::interactors::InteractionBase *m_Interactor;
    bool updateStatus(float val, std::string msg);
};

}

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod &erm);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eReferenceMethod erm);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eBBOptions &ebo);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eBBOptions ebo);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX &eim_x);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX &eim_x);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderX eim_x);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY &eim_y);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY &eim_y);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpOrderY eim_y);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod &eint);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod &eint);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eInterpMethod eint);

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(const std::string &str, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod &emask);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(const ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod &emask);
std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod emask);


#endif /* REFERENCEIMAGECORRECTION_H_ */

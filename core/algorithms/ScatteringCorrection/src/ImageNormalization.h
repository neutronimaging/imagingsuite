//<LICENSE>
#ifndef IMAGENORMALIZATION_H_
#define IMAGENORMALIZATION_H_

#include "ScatteringCorrection_global.h"

#include <map>
#include <vector>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>

class SCATTERINGCORRECTIONSHARED_EXPORT ImageNormalization
{
    kipl::logging::Logger logger;

    public:
        /// @brief Enum to select the normalization method
        enum eNormalizationMethod 
        {
            basicNormalization,
            scatterNormalization,
            interpolateScatterNormalization,
            one2oneScatterNormalization
        };

        /// @brief Base C'tor
        ImageNormalization();

        /// @brief Sets the dark current image. Mostly only once in a processing.
        /// @param img The DC image
        /// @param dose The dose factor of the image
        void setDarkCurrent(const kipl::base::TImage<float,2> &img, float dose=0.0);

        /// @brief Sets the open beam image. Mostly only once in a processing.
        /// @param img The OB image
        /// @param dose The dose factor of the image
        /// @param subtractDC Switch to subtract the DC contribution
        void setOpenBeam(const kipl::base::TImage<float,2> &img, float dose, bool subtractDC = true);

        /// @brief Sets the BB open beam image. Mostly only once in a processing.
        /// @param img The BBOB image
        /// @param dose The dose factor of the image
        /// @param subtractDC Switch to subtract the DC contribution        
        void setBBOpenBeam(const kipl::base::TImage<float,2> &img, float dose, bool subtractDC = true);
        
        /// @brief Sets the BB image for the sample. May be updated for every project depending on processing mode.
        /// @param img The BB image
        /// @param dose The dose factor of the image
        void setBBSample(  const kipl::base::TImage<float,2> &img, float dose);
        
        /// @brief Switch to select if the normalized image should be logarithm
        /// @param useLog The choice
        void useLogarithm(bool useLog);

        /// @brief Executes the actual normalization for a single image
        /// @param img Projection image
        /// @param dose The dose factor
        /// @param normMethod Select which method to normalize the image
        /// @param tau Weight factor to compensate the intensity loss by the bb grid
        /// @return The normalized image
        kipl::base::TImage<float,2> process(kipl::base::TImage<float,2> &img, 
                                                  float                        dose, 
                                                  eNormalizationMethod         normMethod=basicNormalization,
                                                  float                        tau=0.0f);

        /// @brief Inplace normalization of a stack of images
        /// @param img Projection images (3D image with slices along the z-axis)
        /// @param dose The dose factor
        /// @param normMethod Select which method to normalize the image
        /// @param tau Weight factor to compensate the intensity loss by the bb grid
        void process(   kipl::base::TImage<float,3> &img, 
                        const std::vector<float>          &dose, 
                        eNormalizationMethod               normMethod=basicNormalization,
                        float                              tau=0.0f);

    private: 
        bool useLog;
        kipl::base::TImage<float,2> dc;
        kipl::base::TImage<float,2> ob;
        kipl::base::TImage<float,2> bbob;
        kipl::base::TImage<float,2> bbs;
        float obDose;
        float bbobDose;
        float bbsDose;

        void checkImageDims(kipl::base::TImage<float,2> &imgA, kipl::base::TImage<float,2> &imgB);
        void applyLogarithm(kipl::base::TImage<float,2> &img);

        void processBasicNormalization(kipl::base::TImage<float,2> &img, float dose);
        void processScatterNormalization(kipl::base::TImage<float,2> &img, float dose, float tau);


};

#endif
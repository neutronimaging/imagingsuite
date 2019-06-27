//<LICENSE>

#ifndef AVERAGEPROJECTION_H
#define AVERAGEPROJECTION_H
#include "ImagingAlgorithms_global.h"
#include <string>

#include <base/timage.h>

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
namespace py = pybind11;
#endif

namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT AverageImage
{
public:
    enum eAverageMethod {
        ImageSum,               ///< Compute the sum of the XY-images
        ImageAverage,           ///< Compute the average of the XY-images
        ImageMedian,            ///< Compute the median of the XY-images
        ImageWeightedAverage,   ///< Compute the weighted average of the images
        ImageMin,               ///< Compute the minimum pixel value of the images
        ImageMax                ///< Compute the maximum pixel value of the images
    };

    AverageImage();

    ~AverageImage();

    void setWindowSize(int n);
    int windowSize();
    /// \brief Computes a 2D image from a set of images
    /// \param img A 3D image with the input 2D images in the XY-plane.
    /// \param method Method selector
    /// \param weights Additional weights per slice, can be used for dose correction
    kipl::base::TImage<float,2> operator()(kipl::base::TImage<float,3> &img, eAverageMethod method, float *weights=nullptr);
#ifdef HAVEPYBIND11
    py::array_t<float> process(py::array_t<float> &x, ImagingAlgorithms::AverageImage::eAverageMethod method);
#endif
protected:
    kipl::base::TImage<float,3> WeightImages(kipl::base::TImage<float,3> &img, float *weights);
    kipl::base::TImage<float,2> ComputeSum(kipl::base::TImage<float,3> &img);
    kipl::base::TImage<float,2> ComputeAverage(kipl::base::TImage<float,3> & img);
    kipl::base::TImage<float,2> ComputeMedian(kipl::base::TImage<float,3> & img);
    kipl::base::TImage<float,2> ComputeWeightedAverage(kipl::base::TImage<float,3> & img);
    kipl::base::TImage<float,2> ComputeMin(kipl::base::TImage<float,3> & img);
    kipl::base::TImage<float,2> ComputeMax(kipl::base::TImage<float,3> & img);

    void GetColumn(kipl::base::TImage<float,3> &img, size_t idx, float *data);

    int WindowSize;
};

}

void IMAGINGALGORITHMSSHARED_EXPORT string2enum(std::string str, ImagingAlgorithms::AverageImage::eAverageMethod &eam);
std::string IMAGINGALGORITHMSSHARED_EXPORT enum2string(ImagingAlgorithms::AverageImage::eAverageMethod eam);

std::ostream IMAGINGALGORITHMSSHARED_EXPORT & operator<<(ostream & s, ImagingAlgorithms::AverageImage::eAverageMethod eam);
#endif // AVERAGEPROJECTION_H

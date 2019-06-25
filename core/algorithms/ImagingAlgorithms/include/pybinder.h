#ifndef PYBINDER_H
#define PYBINDER_H

#ifdef HAVEPYBIND11
#include "averageimage.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(imagalg,m)
{
    py::class_<ImagingAlgorithms::AverageImage> avgClass(m, "AverageImage");
    avgClass.def(py::init());
 //   avgClass.def(kipl::base::TImage<float,2> operator()(kipl::base::TImage<float,3> &img, eAverageMethod method, float *weights=nullptr));
    avgClass.def_readwrite("windowSize", &ImagingAlgorithms::AverageImage::WindowSize);

    py::enum_<ImagingAlgorithms::AverageImage::eAverageMethod>(avgClass,"eAverageMethod")
            .value("ImageSum",             ImagingAlgorithms::AverageImage::ImageSum)
            .value("ImageAverage",         ImagingAlgorithms::AverageImage::ImageAverage)
            .value("ImageMedian",          ImagingAlgorithms::AverageImage::ImageMedian)
            .value("ImageWeightedAverage", ImagingAlgorithms::AverageImage::ImageWeightedAverage)
            .value("ImageMin",             ImagingAlgorithms::AverageImage::ImageMin)
            .value("ImageMax",             ImagingAlgorithms::AverageImage::ImageMax)
            .export_values();


};

#endif

#endif // PYBINDER_H

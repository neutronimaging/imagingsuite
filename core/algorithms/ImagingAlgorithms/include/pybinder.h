#ifndef PYBINDER_H
#define PYBINDER_H

#ifdef HAVEPYBIND11
#include "averageimage.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(imagalg,m)
{
    py::class_<ImagingAlgorithms::AverageImage> avgClass(m, "AverageImage");
    avgClass.def(py::init());
 //   avgClass.def(kipl::base::TImage<float,2> operator()(kipl::base::TImage<float,3> &img, eAverageMethod method, float *weights=nullptr));
    avgClass.def("process", [](py::array_t<double> x, ImagingAlgorithms::AverageImage::eAverageMethod method, const std::vector<float> &weights) {
        auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable
        double sum = 0;
        for (ssize_t i = 0; i < r.shape(0); i++)
            for (ssize_t j = 0; j < r.shape(1); j++)
                for (ssize_t k = 0; k < r.shape(2); k++)
                    sum += r(i, j, k);
        return sum;
    });
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

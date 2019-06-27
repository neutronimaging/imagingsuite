#ifndef PYBINDER_H
#define PYBINDER_H

#ifdef HAVEPYBIND11
#include <iostream>

#include <base/timage.h>

#include "averageimage.h"


#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

//PYBIND11_MODULE(imagalg,m)
//{
//    py::class_<ImagingAlgorithms::AverageImage> avgClass(m, "AverageImage");
//    avgClass.def(py::init());
//    avgClass.def("windowSize", &ImagingAlgorithms::AverageImage::windowSize);
//    avgClass.def("setWindowSize", &ImagingAlgorithms::AverageImage::setWindowSize);
// //  avgClass.def("process", &ImagingAlgorithms::AverageImage::process);

//    py::enum_<ImagingAlgorithms::AverageImage::eAverageMethod>(avgClass,"eAverageMethod")
//            .value("ImageSum",             ImagingAlgorithms::AverageImage::ImageSum)
//            .value("ImageAverage",         ImagingAlgorithms::AverageImage::ImageAverage)
//            .value("ImageMedian",          ImagingAlgorithms::AverageImage::ImageMedian)
//            .value("ImageWeightedAverage", ImagingAlgorithms::AverageImage::ImageWeightedAverage)
//            .value("ImageMin",             ImagingAlgorithms::AverageImage::ImageMin)
//            .value("ImageMax",             ImagingAlgorithms::AverageImage::ImageMax)
//            .export_values();


//};


PYBIND11_MODULE(imagalg,m)
{
    py::class_<ImagingAlgorithms::AverageImage>(m, "AverageImage")
    .def(py::init())
    .def("windowSize", &ImagingAlgorithms::AverageImage::windowSize)
    .def("setWindowSize", &ImagingAlgorithms::AverageImage::setWindowSize);


};
#endif

#endif // PYBINDER_H


//    avgClass.def("process", [](py::array_t<float> x, ImagingAlgorithms::AverageImage::eAverageMethod method, size_t idx,const std::vector<float> &weights)
//    {
//        auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable

//        py::buffer_info buf1 = x.request();
//        py::print("input dims",buf1.shape[0],", ",buf1.shape[1],", ",buf1.shape[2]);
//        size_t dims[]={static_cast<size_t>(buf1.shape[2]),
//                       static_cast<size_t>(buf1.shape[1]),
//                       static_cast<size_t>(buf1.shape[0])};
//        kipl::base::TImage<float,3> stack(static_cast<float*>(buf1.ptr),dims);

//        py::array_t<float> avg = py::array_t<float>(stack.Size());
//        avg.resize({dims[1],dims[0]});

//        kipl::base::TImage<float,2> sum(static_cast<float*>(avg.request().ptr),dims);
//        sum=0.0f;

////        for (size_t z=0, i=0; z<dims[2]; ++z)
//            for (size_t y=0; y<dims[1]; ++y)
//                for (size_t x=0; x<dims[0]; ++x)
//                    sum(x,y)=stack(x,y,idx);

//        return avg;
//    });

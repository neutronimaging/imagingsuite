//<LICENSE>

#include <pixelinfo.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
void bindPixelInfo(py::module &m)
{
    py::class_<ImagingAlgorithms::PixelInfo> pixClass(m, "PixelInfo");
    pixClass.def(py::init());
    pixClass.def(py::init<int,float,float>());
    pixClass.def(py::init<const ImagingAlgorithms::PixelInfo &>());

    pixClass.def_readwrite("pos",       &ImagingAlgorithms::PixelInfo::pos);
    pixClass.def_readwrite("value",     &ImagingAlgorithms::PixelInfo::value);
    pixClass.def_readwrite("weight",    &ImagingAlgorithms::PixelInfo::weight);
}


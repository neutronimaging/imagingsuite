//<LICENSE>

#include <sstream>
#include <iostream>

#include <imagewriter.h>
#include <fileset.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindImageWriter(py::module &m)
{
    py::class_<ImageWriter> irClass(m, "ImageWriter");

    irClass.def(py::init<>());

}
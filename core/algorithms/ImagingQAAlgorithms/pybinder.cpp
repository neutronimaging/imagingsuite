#include "pybinder.h"

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindProfileExtractor(py::module &m);

PYBIND11_MODULE(imagingqa,m)
{
    bindProfileExtractor(m);
}

#endif
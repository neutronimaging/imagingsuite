#include "advancedfilters.h"

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindISSfilter(py::module &m)
{
    py::class_<advancedfilters::ISSfilterQ3Df> issClass(m, "ISSfilter3D");

    issClass.def();
}

#endif

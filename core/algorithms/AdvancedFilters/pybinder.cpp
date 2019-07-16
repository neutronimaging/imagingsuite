//<LICENSE>

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "advancedfilterexception.h"

namespace py = pybind11;

void bindISSfilter(py::module &m);
void bindFilterEnums(py::module &m);
void bindAdvancedFiltersException(py::module &m);

PYBIND11_MODULE(advancedfilters,m)
{
    bindISSfilter(m);
    bindFilterEnums(m);

    py::register_exception<AdvancedFiltersException>(m, "AdvancedFiltersException");
}

#endif

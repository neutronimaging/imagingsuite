//<LICENSE>

#include <sstream>
#include "../AdvancedFilters/advancedfilterexception.h"

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindException(py::module &m)
{
    py::register_exception<AdvancedFiltersException>(m, "AdvancedFiltersException");
}
#endif

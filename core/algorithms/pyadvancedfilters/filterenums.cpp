//<LICENCE>

#include "../AdvancedFilters/filterenums.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
void bindFilterEnums(py::module &m)
{

    py::enum_<advancedfilters::eInitialImageType>(m,"eInitialImageType")
            .value("InitialImageOriginal",    advancedfilters::InitialImageOriginal)
            .value("InitialImageZero",        advancedfilters::InitialImageZero)
            .export_values();

    py::enum_<advancedfilters::eRegularizationType>(m,"eRegularizationType")
            .value("RegularizationTV1",       advancedfilters::RegularizationTV1)
            .value("RegularizationTV2",       advancedfilters::RegularizationTV2)
            .export_values();

    py::enum_<advancedfilters::eFilterType>(m,"eFilterType")
            .value("FilterISS",               advancedfilters::FilterISS)
            .value("FilterISSp",              advancedfilters::FilterISSp)
            .value("FilterDiffusion",         advancedfilters::FilterDiffusion)
            .value("FilterIteratedMedian",    advancedfilters::FilterIteratedMedian)
            .value("FilterGauss",             advancedfilters::FilterGauss)
            .export_values();

    py::enum_<advancedfilters::eGradientType>(m,"eGradientType")
            .value("Simple_Grad_Centered",  advancedfilters::Simple_Grad_Centered)
            .value("Jahne_Grad",            advancedfilters::Jahne_Grad)
            .value("Simple_Grad_Minus",     advancedfilters::Simple_Grad_Minus)
            .value("Diff_Grad_Minus",       advancedfilters::Diff_Grad_Minus)
            .value("BinDiff_Grad",          advancedfilters::BinDiff_Grad)
            .export_values();
}


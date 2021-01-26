#include "pyBinder.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <ImagingException.h>

namespace py = pybind11;

void bindNormalizeImage(py::module &m);
void bindAverageImage(py::module &m);
void bindPolynomialCorrection(py::module &m);
void bindMorphSpotClean(py::module &m);
void bindStripeFilter(py::module &m);
void bindPixelInfo(py::module &m);
void bindException(py::module &m);
void bindProjectionFilter(py::module &m);

PYBIND11_MODULE(imgalg,m)
{
    bindAverageImage(m);
    bindPolynomialCorrection(m);
    bindMorphSpotClean(m);
    bindStripeFilter(m);
    bindProjectionFilter(m);
    bindNormalizeImage(m);
//    bindPixelInfo(m);
//    bindException(m);

    py::register_exception<ImagingException>(m, "ImagingException");
}


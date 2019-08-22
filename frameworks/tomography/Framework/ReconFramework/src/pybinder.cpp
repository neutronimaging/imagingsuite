#include <iostream>

#ifdef HAVEPYBIND11
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;
void bindReconFactory(py::module &m);
//void bindPolynomialCorrection(py::module &m);
//void bindMorphSpotClean(py::module &m);
//void bindStripeFilter(py::module &m);
//void bindPixelInfo(py::module &m);
//void bindException(py::module &m);

PYBIND11_MODULE(reconframework, m)
{
    bindReconFactory(m);

    //    bindPixelInfo(m);
    //    bindException(m);

    //    py::register_exception<ImagingException>(m, "ImagingException");
}

#endif

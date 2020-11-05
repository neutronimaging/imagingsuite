//<LICENSE>
#ifdef HAVEPYBIND11
#include <ReconException.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindReconFactory(py::module& m);
void bindReconEngine(py::module& m);
void bindReconstructor(py::module& m);

PYBIND11_MODULE(muhrectomo, m)
{
//    bindReconFactory(m);
//    bindReconEngine(m);
    bindReconstructor(m);

    py::register_exception<ReconException>(m, "ReconException");
}

#endif

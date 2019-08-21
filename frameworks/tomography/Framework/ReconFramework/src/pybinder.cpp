#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "../include/ReconException.h"

namespace py = pybind11;

void bindReconFactory(py::module &m);
void bindReconEngine(py::module &m);

PYBIND11_MODULE(reconframework,m)
{
    bindReconFactory(m);
    bindReconEngine(m);

    py::register_exception<ReconException>(m, "ReconException");
}

#endif

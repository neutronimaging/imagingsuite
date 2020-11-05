//<LICENSE>
#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reconstructor.h"

namespace py = pybind11;

void bindReconstructor(py::module &m)
{
    py::class_<Reconstructor> reClass(m, "Reconstructor");

    reClass.def(py::init<eBackProjectors>());

    reClass.def("name",
                [](Reconstructor &recon)
                {
                    return recon.backProjector->Name();
                }
    );

    py::enum_<eBackProjectors>(m,"eBackprojectors")
            .value("bpMultiProj",            bpMultiProj)
            .value("bpMultiProjParallel",    bpMultiProjParallel)
            .value("bpNearestNeighbor",      bpNearestNeighbor)
            .export_values();

}

#endif

#include "pyBinder.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "../ReaderConfig/readerexception.h"

namespace py = pybind11;

//void bindAverageImage(py::module &m);
void bindMiscFunctions(py::module &m);
void bindFileSet(py::module &m);
void bindImageReader(py::module &m);
void bindImageWriter(py::module &m);

PYBIND11_MODULE(imgio,m)
{
    // bindAverageImage(m);
    bindMiscFunctions(m);
    bindFileSet(m);
    bindImageReader(m);
    bindImageWriter(m);
    
    py::register_exception<ReaderException>(m, "ReaderException");
}


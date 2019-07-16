//<LICENSE>

#include <sstream>
#include "advancedfilterexception.h"

AdvancedFiltersException::~AdvancedFiltersException()
{
}

AdvancedFiltersException::AdvancedFiltersException(std::string msg) :
    sExceptionName("AdvancedFiltersException"),
    sMessage(msg),
    nLineNumber(0)
{
}

AdvancedFiltersException::AdvancedFiltersException(std::string msg, std::string filename, size_t line) :
    sExceptionName("AdvancedFiltersException"),
    sMessage(msg),
    sFileName(filename),
    nLineNumber(line)
{
}

const char* AdvancedFiltersException::what() const
{

    if (nLineNumber==0) {
        return sMessage.c_str();
    }

    if (sFileName.empty())
        return sMessage.c_str();

    std::ostringstream str;

    str<<"An "<<sExceptionName<<" was thrown in file "<<sFileName<<" on line "<<nLineNumber<<": "<<std::endl<<sMessage;

    return str.str().c_str();
}

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

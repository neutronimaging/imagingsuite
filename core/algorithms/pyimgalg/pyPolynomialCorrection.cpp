//<LICENSE>

#include <cstring>
#include <cstddef>
#include <sstream>
#include <algorithm>
#ifdef _OPENMP
#include <omp.h>
#endif

#include <PolynomialCorrection.h>
#include <ImagingException.h>


#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindPolynomialCorrection(py::module &m)
{
    py::class_<ImagingAlgorithms::PolynomialCorrection> pcClass(m, "PolynomialCorrection");
    pcClass.def(py::init());
    pcClass.def("setup", static_cast<int (ImagingAlgorithms::PolynomialCorrection::*)(const std::vector<float> &)>(&ImagingAlgorithms::PolynomialCorrection::setup),
                "Configures the polynomial with a list of coefficients",
                py::arg("coeff"));
    pcClass.def("coefficients", &ImagingAlgorithms::PolynomialCorrection::coefficients,
                "Returns the current list of coefficients");
    pcClass.def("polynomialOrder", &ImagingAlgorithms::PolynomialCorrection::polynomialOrder,
                "Returns the polynomial order");
    pcClass.def("process", static_cast<std::vector<float> (ImagingAlgorithms::PolynomialCorrection::*)(const std::vector<float> &)>(&ImagingAlgorithms::PolynomialCorrection::process),
                "Applies the polynomial to the elements of the provided list",
                py::arg("x"));

    pcClass.def("processInplace",
                 [](ImagingAlgorithms::PolynomialCorrection &pc,
                 py::array_t<float> &x)
    {
        py::buffer_info buf1 = x.request();

        float *data=static_cast<float*>(buf1.ptr);

        pc.processInplace(data,x.size());
    },
    "Applies the polynomial inplace in the array.",
    py::arg("data"));

    pcClass.def("processInplace",
                 [](ImagingAlgorithms::PolynomialCorrection &pc,
                 py::array_t<double> &x)
    {
        py::buffer_info buf1 = x.request();

        double *data=static_cast<double*>(buf1.ptr);

        pc.processInplace(data,x.size());
    },
    "Applies the polynomial inplace in the array.",
    py::arg("data"));

}


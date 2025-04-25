//<LICENSE>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <tomocenter.h>

#include <algorithm>
#include <sstream>
#include <vector>
#include <map>
#include <numeric>

#include <base/KiplException.h>
#inlcude <math/thinplatespline.h>

namespace py = pybind11;

void bindTPSInterpolator(py::module &m)
{
    py::class_<kipl::math::ThinPlateSpline> tpsClass(m, "TPSInterpolator");

    tpsClass.def(py::init());

    tpsClass.def(py::init<>()); // Default constructor

    tpsClass.def(py::init<const std::vector<float>&, 
                          const std::vector<float>&, 
                          const std::vector<float>&>(),
         py::arg("x"), py::arg("y"), py::arg("values"),
         "Constructor that initializes ThinPlateSpline with x, y, and values.");

    tpsClass.def(py::init<const std::vector<double>&, 
                          const std::vector<double>&, 
                          const std::vector<double>&>(),
         py::arg("x"), py::arg("y"), py::arg("values"),
         "Constructor that initializes ThinPlateSpline with x, y, and values.");

    tpsClass.def("setPoints", 
         py::overload_cast<const std::vector<float>&, 
                           const std::vector<float>&, 
                           const std::vector<float>&>(&kipl::math::ThinPlateSpline::setPoints),
         py::arg("x"), py::arg("y"), py::arg("values"),
         "Set the control points for the ThinPlateSpline.");

    tpsClass.def("setPoints", 
        py::overload_cast<const std::vector<double>&, 
                          const std::vector<double>&, 
                          const std::vector<double>&>(&kipl::math::ThinPlateSpline::setPoints),
        py::arg("x"), py::arg("y"), py::arg("values"),
        "Set the control points for the ThinPlateSpline.");

    tpsClass.def("interpolate", 
        py::overload_cast<float, float>(&kipl::math::ThinPlateSpline::interpolate),
        py::arg("x"), py::arg("y"),
        "Interpolate a value at the given x, y coordinates.");

    tpsClass.def("interpolate", 
        py::overload_cast<double, double>(&kipl::math::ThinPlateSpline::interpolate),
        py::arg("x"), py::arg("y"),
        "Interpolate a value at the given x, y coordinates.");
    
    tpsClass.def("interpolate", 
        py::overload_cast<size_t, size_t>(&kipl::math::ThinPlateSpline::interpolate),
        py::arg("x"), py::arg("y"),
        "Interpolate a value at the given x, y coordinates.");

    tpsClass.def("render",
        py::overload_cast<const std::vector<size_t>&, const std::vector<size_t>&>(&kipl::math::ThinPlateSpline::render),
        py::arg("dims"), py::arg("fov") = std::vector<size_t>(),
        "Render the ThinPlateSpline to a TImage with the given dimensions and field of view.");

        
    tpsClass.def("size", &kipl::math::ThinPlateSpline::size, "Get the number of control points.");
    

}

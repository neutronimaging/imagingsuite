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

namespace py = pybind11;

void bindCenterEstimator(py::module &m)
{
    py::class_<ImagingAlgorithms::TomoCenter> tcClass(m, "TomoCenter");

    tcClass.def(py::init());

    tcClass.def("setFraction",&ImagingAlgorithms::TomoCenter::setFraction,
                "Set the mid quantile fraction of center estimates to use for the fit.",
                py::arg("fraction"));

    tcClass.def("estimate",[](ImagingAlgorithms::TomoCenter &tc,
                py::array_t<float> &x0, py::array_t<float> &x180,
                ImagingAlgorithms::TomoCenter::eEstimator est,
                bool bTilt)
    {
        py::buffer_info buf0 = x0.request();

        if (buf0.format!=py::format_descriptor<float>::format())
            throw std::runtime_error("Incompatible format: expected a float (32-bit) array!");

        if (buf0.shape.size() != 2UL)
            throw std::runtime_error("Incompatible format: expected a 3D array with projections!");

        std::vector<size_t> dims0={  static_cast<size_t>(buf0.shape[1]),
                                    static_cast<size_t>(buf0.shape[0])
                                 };

        kipl::base::TImage<float,2> img0(static_cast<float*>(buf0.ptr),dims0);

        py::buffer_info buf180 = x180.request();

        if (buf180.format!=py::format_descriptor<float>::format())
            throw std::runtime_error("Incompatible format: expected a float (32-bit) array!");

        if (buf180.shape.size() != 2UL)
            throw std::runtime_error("Incompatible format: expected a 3D array with projections!");

        std::vector<size_t> dims180={  static_cast<size_t>(buf180.shape[1]),
                                    static_cast<size_t>(buf180.shape[0])
                                 };
        kipl::base::TImage<float,2> img180(static_cast<float*>(buf180.ptr),dims180);

        double center=0.0;
        double tilt=0.0;
        double pivot=0.0;
        std::vector<size_t> roi={0,0,img0.Size(0),img0.Size(1)};
        tc.estimate(img0,
                  img180,
                  est,
                  roi,
                  bTilt,
                  center,
                  tilt,
                  pivot);

         return py::make_tuple(center,tilt,pivot);
    });

    tcClass.def("centers",&ImagingAlgorithms::TomoCenter::centers, "Returns a vector with all centers");

    tcClass.def("tiltParameters", [](ImagingAlgorithms::TomoCenter &tc)
    {
        double k=1;
        double mm=0;
        tc.tiltParameters(k,mm);
        return py::make_tuple(k,mm);
    },
    "Returns the linear fit coefficients for the center line");

    tcClass.def("center",&ImagingAlgorithms::TomoCenter::center,"Returns the center at vertical position y",py::arg("y"));
    tcClass.def("R2",&ImagingAlgorithms::TomoCenter::R2,"Returns R2 for the fit.");


    py::enum_<ImagingAlgorithms::TomoCenter::eEstimator>(m,"eCenterEstimator")
            .value("centerLeastSquare",            ImagingAlgorithms::TomoCenter::leastSquare)
            .value("centerCorrelation",            ImagingAlgorithms::TomoCenter::correlation)
            .value("centerCenterOfGravity",        ImagingAlgorithms::TomoCenter::centerOfGravity)
            .export_values();


}

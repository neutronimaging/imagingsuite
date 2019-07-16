#include "advancedfilters.h"

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindISSfilter(py::module &m)
{
    py::class_<advancedfilters::ISSfilterQ3Df> issClass(m, "ISSfilter3D");

    issClass.def(py::init());

//    int process(kipl::base::TImage<T,3> &img,
//            double dTau,
//            double dLambda,
//            double dAlpha,
//            int nN,
//            bool saveiterations=false,
//            std::string itpath="");

    issClass.def("process",
                 [](advancedfilters::ISSfilterQ3Df &iss,
                 py::array_t<float> &x,
                 double dTau,
                 double dLambda,
                 double dAlpha,
                 int nN,
                 bool saveiterations=false,
                 std::string itpath="")
    {
        auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable

        py::buffer_info buf1 = x.request();

        size_t dims[]={static_cast<size_t>(buf1.shape[2]),
                       static_cast<size_t>(buf1.shape[1]),
                       static_cast<size_t>(buf1.shape[0])};
        kipl::base::TImage<float,3> img(static_cast<float*>(buf1.ptr),dims);

        iss.process(img,dTau,dLambda,dAlpha,nN,saveiterations,itpath);

      //  std::copy_n(res.GetDataPtr(),res.Size(),avgimg.GetDataPtr());
    },
    "Computes a combined image using the selected method",
    py::arg("img"),
    py::arg("tau")=0.125,
    py::arg("lambda")=1.0,
    py::arg("alpha")=0.25,
    py::arg("N")=10,
    py::arg("saveIterations")=false,
    py::arg("itPath")="iteration_####.tif");

    issClass.def("errors",
                 &advancedfilters::ISSfilterQ3Df::errors,
                 "Returns a vector with the error of each filter iteration");
    issClass.def("entropies",
                 &advancedfilters::ISSfilterQ3Df::entropies,
                 "Returns a vector with the entropy of each filter iteration");

//    issClass.def("setNumThreads",
//                 &advancedfilters::ISSfilterQ3Df::numThreads,
//                 "Sets the number of threads. This should usually not be changed.");

    issClass.def("initialImageType",
                 &advancedfilters::ISSfilterQ3Df::initialImageType,
                 "Returns the type of the initial image using the eInitialImageType enum.");

    issClass.def("setInitialImageType",
                 &advancedfilters::ISSfilterQ3Df::setInitialImageType,
                 "Sets the type of the initial image using the eInitialImageType enum.");

    issClass.def("regularizationType",
                 &advancedfilters::ISSfilterQ3Df::regularizationType,
                 "Returns the regularization type.");

    issClass.def("setRegularizationType",
                 &advancedfilters::ISSfilterQ3Df::setRegularizationType,
                 "Sets the regularization type using eRegularizationType enum.");

}

#endif

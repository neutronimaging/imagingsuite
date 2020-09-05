//<LICENSE>
#include "../include/ImagingAlgorithms_global.h"

#include "../include/projectionfilter.h"
#include "../include/ImagingException.h"
//#include <ReconHelpers.h>

//#include <ParameterHandling.h>

#include <math/mathconstants.h>
#include <math/mathfunctions.h>
#include <strings/miscstring.h>
#include <math/compleximage.h>
#include <base/imagecast.h>
#include <visualization/GNUPlot.h>

#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

#include <projectionfilter.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <typeinfo>

namespace py = pybind11;

void bindProjectionFilter(py::module &m)
{
    py::class_<ImagingAlgorithms::ProjectionFilter> pfClass(m, "ProjectionFilter");
    pfClass.def(py::init());
    pfClass.def("filterWeights",&ImagingAlgorithms::ProjectionFilter::filterWeights,"Returns an array with the filter weights.");

    pfClass.def("parameters",
                &ImagingAlgorithms::ProjectionFilter::parameters,
                "Returns a list with the filter parameters");

    pfClass.def("setParameters",
                 [](ImagingAlgorithms::ProjectionFilter &pf,
                 py::dict & pars)
    {
        std::map<std::string, std::string> mpars;
        for (auto item : pars) {
            mpars.insert(std::make_pair(py::str(item.first),py::str(item.second)));
            py::print("key: ",typeid(item.first).name(),", value=",typeid(item.second).name());
        }
        pf.setParameters(mpars);
    }, "Sets the filter parameters using a dictionary",py::arg("pars"));

    pfClass.def("setFilter",
                &ImagingAlgorithms::ProjectionFilter::setFilter,
                "Set filter type and parameters",
                py::arg("ft"), py::arg("cutOff"), py::arg("order")=0.0f);
    pfClass.def("filterType",
                &ImagingAlgorithms::ProjectionFilter::filterType,
                "Returns the current filter type");
    pfClass.def("cutOff",
                &ImagingAlgorithms::ProjectionFilter::cutOff,
                "Returns the current filter cut-off frequency");

    pfClass.def("order",
                &ImagingAlgorithms::ProjectionFilter::order,
                "Returns the current filter order (only relevant for the Butterworth filter).");

    pfClass.def("setBiasBehavior",
                &ImagingAlgorithms::ProjectionFilter::setBiasBehavior,
                "Defines how the DC component should be handled.",
                py::arg("useBias"), py::arg("biasWeight"));
    pfClass.def("useBias",
                &ImagingAlgorithms::ProjectionFilter::useBias,
                "Returns true if the bias is used.");
    pfClass.def("biasWeight",
                &ImagingAlgorithms::ProjectionFilter::biasWeight,
                "Returns the fraction of the first non-DC frequency to be used as bias.");

    pfClass.def("setPaddingDoubler",
                &ImagingAlgorithms::ProjectionFilter::setPaddingDoubler,
                "Sets the how many time the spectrum length is doubled and filled with zeros as padding.",
                py::arg("N"));
    pfClass.def("paddingDoubler",
                &ImagingAlgorithms::ProjectionFilter::paddingDoubler,
                "Returns the padding doubler value.");

    pfClass.def("currentFFTSize",
                &ImagingAlgorithms::ProjectionFilter::currentFFTSize,
                "Returns the size of the current FFT buffer.");
    pfClass.def("currentImageSize",
                &ImagingAlgorithms::ProjectionFilter::currentImageSize,
                "Returns width of the current image.");


    pfClass.def("process",
                 [](ImagingAlgorithms::ProjectionFilter &pf,
                 py::array_t<float> &x)
    {
        py::buffer_info buf1 = x.request();

        float *data=static_cast<float*>(buf1.ptr);

        if (buf1.ndim==3)
        {
            std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[2]),
                                        static_cast<size_t>(buf1.shape[1]),
                                        static_cast<size_t>(buf1.shape[0])};
            kipl::base::TImage<float,3> img(data,dims);

            pf.process(img);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else if (buf1.ndim==2)
        {
            std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[1]),
                                        static_cast<size_t>(buf1.shape[0])};


            kipl::base::TImage<float,2> img(data,dims);
            pf.process(img);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
    },
    "Applies the projection filter inplace on the rows in the array.",
    py::arg("data"));

    pfClass.def("process",
                 [](ImagingAlgorithms::ProjectionFilter &pf,
                 py::array_t<double> &x)
    {
        py::buffer_info buf1 = x.request();

        double *data=static_cast<double*>(buf1.ptr);

        if (buf1.ndim==3)
        {
            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[2]),
                                            static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};
            kipl::base::TImage<float,3> img(dims);
            std::copy_n(data,img.Size(),img.GetDataPtr());

            pf.process(img);

            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else if (buf1.ndim==2)
        {
            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};


            kipl::base::TImage<float,2> img(dims);
            std::copy_n(data,img.Size(),img.GetDataPtr());

            pf.process(img);

            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
    },
    "Applies the projection filter inplace on the rows in the array.",
    py::arg("data"));

    py::enum_<ImagingAlgorithms::ProjectionFilterType>(m,"eProjectionFilterType")
            .value("ProjectionFilterNone",         ImagingAlgorithms::ProjectionFilterNone)
            .value("ProjectionFilterRamLak",       ImagingAlgorithms::ProjectionFilterRamLak)
            .value("ProjectionFilterSheppLogan",   ImagingAlgorithms::ProjectionFilterSheppLogan)
            .value("ProjectionFilterHanning",      ImagingAlgorithms::ProjectionFilterHanning)
            .value("ProjectionFilterHamming",      ImagingAlgorithms::ProjectionFilterHamming)
            .value("ProjectionFilterButterworth",  ImagingAlgorithms::ProjectionFilterButterworth)
            .value("ProjectionFilterParzen",       ImagingAlgorithms::ProjectionFilterParzen)
            .export_values();

}


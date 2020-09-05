//<LICENSE>
#include <algorithm>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>

#include <MorphSpotClean.h>
#include <ImagingException.h>

#if !defined(NO_QT)
#include <QDebug>
#endif

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindMorphSpotClean(py::module &m)
{
    py::class_<ImagingAlgorithms::MorphSpotClean> mscClass(m, "MorphSpotClean");

    //    void Process(kipl::base::TImage<float,2> &img, float th, float sigma);
    //    void Process(kipl::base::TImage<float,2> &img, float *th, float *sigma);


    mscClass.def(py::init());
//    mscClass.def("setConnectivity",
//                &ImagingAlgorithms::MorphSpotClean::setConnectivity), // kipl::morphology::MorphConnect conn = kipl::morphology::conn8
//                "Configures the polynomial with a list of coefficients",
//                py::arg("coeff"));

    mscClass.def("setCleanMethod",
                 &ImagingAlgorithms::MorphSpotClean::setCleanMethod,
                 "Returns the current list of coefficients",
                 py::arg("detectionMethod"),
                 py::arg("cleanMethod"));

    mscClass.def("cleanMethod",
                 &ImagingAlgorithms::MorphSpotClean::cleanMethod,
                 "Returns the current clean method.");

    mscClass.def("detectionMethod",
                 &ImagingAlgorithms::MorphSpotClean::detectionMethod,
                 "Returns the current detection method.");

    mscClass.def("setLimits",
                 &ImagingAlgorithms::MorphSpotClean::setLimits,
                 "Set limit on the image values and sizes of blobs",
                 py::arg("applyClamp"),
                 py::arg("vmin"),
                 py::arg("vmax"),
                 py::arg("maxarea"));

    mscClass.def("clampLimits",
                 &ImagingAlgorithms::MorphSpotClean::clampLimits,
                 "Returns a vector containing the data clamping lower and upper limits.");

    mscClass.def("clampActive",
                  &ImagingAlgorithms::MorphSpotClean::clampActive,
                  "Returns true if data clamping is active.");
    mscClass.def("maxArea",
                 &ImagingAlgorithms::MorphSpotClean::maxArea,
                 "Returns the max area of detected spots to be accepted for cleaning.");

    mscClass.def("cleanInfNan",
                 &ImagingAlgorithms::MorphSpotClean::cleanInfNan,
                 "Makes a check and replaces possible Inf and Nan values in the image before cleaning.");

    mscClass.def("setEdgeConditioning",
                 &ImagingAlgorithms::MorphSpotClean::setEdgeConditioning,
                 "Sets the length of the median filter used to precondition the image boundaries.",
                 py::arg("length"));

    mscClass.def("edgeConditionLength",
                 &ImagingAlgorithms::MorphSpotClean::edgeConditionLength,
                 "Returns the lenght of the edge conditioning filter.");

    // kipl::base::TImage<float,2> detectionImage(kipl::base::TImage<float,2> img);
    mscClass.def("detectionImage",
                 [](ImagingAlgorithms::MorphSpotClean &msc, py::array_t<float> &x)
    {
        auto r = x.unchecked<2>(); // x must have ndim = 2; can be non-writeable

        py::buffer_info buf1 = x.request();

        std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[1]),
                                    static_cast<size_t>(buf1.shape[0])};
        kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);

        kipl::base::TImage<float,2> res=msc.detectionImage(img);

        py::array_t<float> det = py::array_t<float>(res.Size());
        det.resize({res.Size(1),res.Size(0)});

        std::copy_n(res.GetDataPtr(),res.Size(), static_cast<float *>(det.request().ptr));
        return det;
    },
    "Computes the detection image from the provided image.",
    py::arg("img"));

    mscClass.def("process",
                 [](ImagingAlgorithms::MorphSpotClean &msc,
                 py::array_t<float> &x,
                 float th,
                 float sigma)
            {
                py::buffer_info buf1 = x.request();

                std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};
                kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);

                msc.process(img,th,sigma);
            },

            "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
            py::arg("data"),
            py::arg("th"),
            py::arg("sigma"));

    mscClass.def("process",
                         [](ImagingAlgorithms::MorphSpotClean &msc,
                         py::array_t<float> &x,
                         std::vector<float> &th,
                         std::vector<float> &sigma)
            {
                py::buffer_info buf1 = x.request();

                std::vector<size_t> dims={static_cast<size_t>(buf1.shape[1]),
                               static_cast<size_t>(buf1.shape[0])};
                kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);

                msc.process(img,th,sigma);
            },

            "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
            py::arg("data"),
            py::arg("th"),
            py::arg("sigma"));

    mscClass.def("process",
                 [](ImagingAlgorithms::MorphSpotClean &msc,
                 py::array_t<double> &x,
                 double th,
                 double sigma)
    {
        py::buffer_info buf1 = x.request();

        std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                        static_cast<size_t>(buf1.shape[0])};
        double *data=static_cast<double*>(buf1.ptr);

        kipl::base::TImage<float,2> img(dims);

        std::copy_n(data,img.Size(),img.GetDataPtr());

        msc.process(img,th,sigma);
        std::copy_n(img.GetDataPtr(),img.Size(),data);
    },

                "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
                py::arg("data"),
                py::arg("th"),
                py::arg("sigma"));


    mscClass.def("process",
                 [](ImagingAlgorithms::MorphSpotClean &msc,
                 py::array_t<double> &x,
                 std::vector<float> &th,
                 std::vector<float> &sigma)
    {
        py::buffer_info buf1 = x.request();

        std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                        static_cast<size_t>(buf1.shape[0])};

        double *data=static_cast<double*>(buf1.ptr);

        kipl::base::TImage<float,2> img(dims);

        std::copy_n(data,img.Size(),img.GetDataPtr());

        msc.process(img,th,sigma);
        std::copy_n(img.GetDataPtr(),img.Size(),data);
    },

                "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
                py::arg("data"),
                py::arg("th"),
                py::arg("sigma"));



    py::enum_<ImagingAlgorithms::eMorphCleanMethod>(m,"eMorphCleanMethod")
        .value("MorphCleanReplace", ImagingAlgorithms::MorphCleanReplace)
        .value("MorphCleanFill",    ImagingAlgorithms::MorphCleanFill)
        .export_values();


    py::enum_<ImagingAlgorithms::eMorphDetectionMethod>(m,"eMorphDetectionMethod")
            .value("MorphDetectHoles",         ImagingAlgorithms::MorphDetectHoles)
            .value("MorphDetectPeaks",         ImagingAlgorithms::MorphDetectPeaks)
            .value("MorphDetectBoth",          ImagingAlgorithms::MorphDetectBoth)
            .export_values();

}






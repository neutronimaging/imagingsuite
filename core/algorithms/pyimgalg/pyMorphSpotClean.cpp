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

    mscClass.def("useThreading",
                 &ImagingAlgorithms::MorphSpotClean::useThreading,
                 "Switches the use of threaded processing.",
                 py::arg("x"));

    mscClass.def("isThreaded",
                 &ImagingAlgorithms::MorphSpotClean::isThreaded,
                 "Tells if threaded processing is used");

    mscClass.def("setNumberOfThreads", &ImagingAlgorithms::MorphSpotClean::setNumberOfThreads,
                "Set the number of threads to use for the processing. Values less than one and greater than the number of availble cores will default to the number of physical cores",
                py::arg("N"));
                
    mscClass.def("numberOfThreads",  &ImagingAlgorithms::MorphSpotClean::numberOfThreads,
                "Tells how many threads are currently used for the processing");

    mscClass.def("setThresholdByFraction", &ImagingAlgorithms::MorphSpotClean::setThresholdByFraction,
                 "Select if threshold is absolute (false) or by quantile (true, default)",
                 py::arg("method"));

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
                 [](ImagingAlgorithms::MorphSpotClean &msc, py::array_t<float> &x, bool remove_bias)
    {
        // auto r = x.unchecked<2>(); // x must have ndim = 2; can be non-writeable

        py::buffer_info buf1 = x.request();
        if (buf1.ndim != 2) {
            throw std::runtime_error("Input array must have 2 dimensions");
        }

        std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[1]),
                                    static_cast<size_t>(buf1.shape[0])};
        kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);

        auto res=msc.detectionImage(img,remove_bias);

        py::dict detections;

        py::array_t<float> dark = py::array_t<float>(res.second.Size());
        dark.resize({res.second.Size(1),res.second.Size(0)});

        std::copy_n(res.second.GetDataPtr(),res.second.Size(), static_cast<float *>(dark.request().ptr));

        detections["dark"]=dark;

        py::array_t<float> bright = py::array_t<float>(res.first.Size());
        bright.resize({res.first.Size(1),res.first.Size(0)});

        std::copy_n(res.first.GetDataPtr(),res.first.Size(), static_cast<float *>(bright.request().ptr));

        detections["bright"]=bright;

        return detections;
       
    },
    "Computes the detection image from the provided image.",
    py::arg("img"),
    py::arg("remove_bias"));

    mscClass.def("process",
                 [](ImagingAlgorithms::MorphSpotClean &msc,
                 py::array_t<float> &x,
                 float th,
                 float sigma)
            {
                py::print("Process 2D: py::array_t<float> &x, float th, float sigma");
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

        if (buf1.ndim == 2)
        {
            py::print("Process 2D: py::array_t<float> &x, std::vector<float> &th, std::vector<float> &sigma");
            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};
            float *data=static_cast<float*>(buf1.ptr);

            kipl::base::TImage<float,2> img(dims);

            std::copy_n(data,img.Size(),img.GetDataPtr());

            msc.process(img,th,sigma);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else if (buf1.ndim==3)
        {
            py::print("Process 3D: py::array_t<float> &x, std::vector<float> &th, std::vector<float> &sigma");
            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[2]),
                                            static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};

            float *data=static_cast<float*>(buf1.ptr);

            kipl::base::TImage<float,3> img(dims);

            std::copy_n(data,img.Size(),img.GetDataPtr());

            msc.process(img,th,sigma);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else
            throw ImagingException("Morphspot clean only supports 2- and 3-D data",__FILE__,__LINE__);
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

        if (buf1.ndim == 2)
        {
            py::print("Process 2D: py::array_t<double> &x, double th, double sigma");
            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};
            double *data=static_cast<double*>(buf1.ptr);

            kipl::base::TImage<float,2> img(dims);

            std::copy_n(data,img.Size(),img.GetDataPtr());

            msc.process(img,th,sigma);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else if (buf1.ndim==3)
        {
            py::print("Process 3D: py::array_t<double> &x, double th, double sigma");

            std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[2]),
                                            static_cast<size_t>(buf1.shape[1]),
                                            static_cast<size_t>(buf1.shape[0])};

            double *data=static_cast<double*>(buf1.ptr);

            kipl::base::TImage<float,3> img(dims);

            std::copy_n(data,img.Size(),img.GetDataPtr());

            msc.process(img,th,sigma);
            std::copy_n(img.GetDataPtr(),img.Size(),data);
        }
        else
            throw ImagingException("Morphspot clean only supports 2- and 3-D data",__FILE__,__LINE__);

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
        py::print("Process 2D: py::array_t<double> &x, std::vector<float> &th, std::vector<float> & sigma");
        
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
            .value("MorphDetectDarkSpots",        ImagingAlgorithms::MorphDetectDarkSpots)
            .value("MorphDetectBrightSpots", ImagingAlgorithms::MorphDetectBrightSpots)
            .value("MorphDetectAllSpots",         ImagingAlgorithms::MorphDetectAllSpots)
            .value("MorphDetectHoles",            ImagingAlgorithms::MorphDetectHoles)
            .value("MorphDetectPeaks",            ImagingAlgorithms::MorphDetectPeaks)
            .value("MorphDetectBoth",             ImagingAlgorithms::MorphDetectBoth)
            .export_values();

}





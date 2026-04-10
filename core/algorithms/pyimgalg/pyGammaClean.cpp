//<LICENSE>
#include <algorithm>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>

#include <gammaclean.h>
#include <ImagingException.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindGammaClean(py::module &m)
{
    py::class_<ImagingAlgorithms::GammaClean> gcClass(m, "GammaClean");

    gcClass.def(py::init([](float sigma, float th3, float th5, float th7, size_t medsize, bool useThreads) {
        return ImagingAlgorithms::GammaClean(sigma, th3, th5, th7, medsize, useThreads, nullptr);
    }),
            py::arg("sigma") = 0.8f,
            py::arg("th3")   = 25.0f,
            py::arg("th5")   = 100.0f,
            py::arg("th7")   = 400.0f,
            py::arg("medsize") = 3UL,
            py::arg("useThreads")    = false);


    gcClass.def("process",
                 [](ImagingAlgorithms::GammaClean &gc,
                 py::array_t<float> &x)
            {
                py::buffer_info buf1 = x.request();

                std::vector<size_t> dims(buf1.shape.rbegin(), buf1.shape.rend());
                
                float *data=static_cast<float*>(buf1.ptr);
                
                if (dims.size() != 2 && dims.size() != 3)
                {
                    throw ImagingException("Only 2D and 3D images are supported in pyGammaClean::process");
                }

                if (dims.size() == 2)
                {
                        py::print("Processing 2D image with dimensions: ", dims);
  
                        kipl::base::TImage<float,2> img(data,dims);

                        gc.process(img);

                        std::copy_n(img.GetDataPtr(),img.Size(),data);
                }

                if (dims.size() == 3)
                {
                        py::print("Processing 3D image with dimensions: ", dims);

                        kipl::base::TImage<float,3> img(data,dims);
           
                        gc.process(img);

                        std::copy_n(img.GetDataPtr(),img.Size(),data);
                }

            },

            "Cleans spots from the image in place using th as threshold and sigma as mixing width. The processing is in-place, so the input image is modified and returned as output.",
            py::arg("x"));


        gcClass.def("configure", &ImagingAlgorithms::GammaClean::configure, "Configures the algorithm parameters",
            py::arg("sigma"), py::arg("th3"), py::arg("th5"), py::arg("th7"), py::arg("medsize"));

        gcClass.def("detection_image", 
                [](ImagingAlgorithms::GammaClean &gc) {
                        kipl::base::TImage<float,2> img = gc.detectionImage();
                        py::array_t<float> detection = py::array_t<float>(img.Size());
                        detection.resize({img.Size(1), img.Size(0)});
                        std::copy_n(img.GetDataPtr(), img.Size(), static_cast<float *>(detection.request().ptr));
                        return detection;
                },
                "Returns the detection image");

        gcClass.def("spot_mask", 
                [](ImagingAlgorithms::GammaClean &gc) {
                        kipl::base::TImage<unsigned short,2> img = gc.spotMask();
                        auto detection = py::array_t<unsigned short>(img.Size());
                        detection.resize({img.Size(1), img.Size(0)});
                        std::copy_n(img.GetDataPtr(), img.Size(), static_cast<unsigned short *>(detection.request().ptr));
                        return detection;
                }, 
                "Returns the spot mask");
        
        gcClass.def("numberOfThreads", 
                &ImagingAlgorithms::GammaClean::numberOfThreads, 
                "Returns the number of threads used in processing");

        gcClass.def("isThreaded", 
                &ImagingAlgorithms::GammaClean::isThreaded, 
                "Returns whether the algorithm is using multiple threads");
        
//     py::enum_<ImagingAlgorithms::eSortSpotQuantile>(m,"eSortSpotQuantile")
//             .value("SortQuantileAll",           ImagingAlgorithms::eSortSpotQuantile::All)
//             .value("SortQuantileBright",        ImagingAlgorithms::eSortSpotQuantile::BrightSpots)
//             .value("SortQuantileDark",          ImagingAlgorithms::eSortSpotQuantile::DarkSpots)
//             .value("SortQuantileBoth",          ImagingAlgorithms::eSortSpotQuantile::Both)
//             .export_values();

}





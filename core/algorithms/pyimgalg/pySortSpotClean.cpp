//<LICENSE>
#include <algorithm>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>

#include <sortspotclean.h>
#include <ImagingException.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindSortSpotClean(py::module &m)
{
    py::class_<ImagingAlgorithms::SortSpotClean> sscClass(m, "SortSpotClean");

    sscClass.def(py::init<bool, size_t, bool>(),
            py::arg("processPatches")= false,
            py::arg("patchSize")     = 32UL,
            py::arg("useThreads")    = false);

    sscClass.def("process",
                 [](ImagingAlgorithms::SortSpotClean &msc,
                 py::array_t<float> &x,
                 float quantile,
                 float th,
                 ImagingAlgorithms::eSortSpotQuantile esq,
                 ImagingAlgorithms::eSortSpotThresholdMethod estm)
            {
                py::buffer_info buf1 = x.request();

                std::vector<size_t> dims(buf1.shape.rbegin(), buf1.shape.rend());
                
                float *data=static_cast<float*>(buf1.ptr);
                
                if (dims.size() != 2 && dims.size() != 3)
                {
                    throw ImagingException("Only 2D and 3D images are supported in pySortSpotClean::process");
                }

                if (dims.size() == 2)
                {
                        py::print("Processing 2D image with dimensions: ", dims);
  
                        kipl::base::TImage<float,2> img(data,dims);

                        msc.process(img,quantile,th,esq,estm);

                        std::copy_n(img.GetDataPtr(),img.Size(),data);
                }

                if (dims.size() == 3)
                {
                        py::print("Processing 3D image with dimensions: ", dims);

                        kipl::base::TImage<float,3> img(data,dims);
           
                        msc.process(img,quantile,th,esq,estm);

                        std::copy_n(img.GetDataPtr(),img.Size(),data);
                }

            },

            "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
            py::arg("x"),
            py::arg("quantile"),
            py::arg("th"),
            py::arg("method"),
            py::arg("thresholdMethod"));

    py::enum_<ImagingAlgorithms::eSortSpotQuantile>(m,"eSortSpotQuantile")
            .value("SortQuantileAll",           ImagingAlgorithms::eSortSpotQuantile::All)
            .value("SortQuantileBright",        ImagingAlgorithms::eSortSpotQuantile::BrightSpots)
            .value("SortQuantileDark",          ImagingAlgorithms::eSortSpotQuantile::DarkSpots)
            .value("SortQuantileBoth",          ImagingAlgorithms::eSortSpotQuantile::Both)
            .export_values();

    py::enum_<ImagingAlgorithms::eSortSpotThresholdMethod>(m,"eSortSpotThresholdMethod")
            .value("SortThresholdConfidence",    ImagingAlgorithms::eSortSpotThresholdMethod::ConfidenceInterval)
            .value("SortThresholdAbsolute",      ImagingAlgorithms::eSortSpotThresholdMethod::Absolute)
            .export_values();

}





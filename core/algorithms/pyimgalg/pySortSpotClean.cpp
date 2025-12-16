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

    //    void Process(kipl::base::TImage<float,2> &img, float th, float sigma);
    //    void Process(kipl::base::TImage<float,2> &img, float *th, float *sigma);


    sscClass.def(py::init<bool, size_t, bool>(),
            py::arg("processPatches")= false,
            py::arg("patchSize")     = 32UL,
            py::arg("useThreads")    = false);

    sscClass.def("process",
                 [](ImagingAlgorithms::SortSpotClean &msc,
                 py::array_t<float> &x,
                 float quantile,
                 float th,
                 ImagingAlgorithms::eSortSpotQuantile esq)
            {
                py::print("Process 2D: py::array_t<float> &x, float th, float sigma");
                py::buffer_info buf1 = x.request();

                std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[1]),
                                                static_cast<size_t>(buf1.shape[0])};
                float *data=static_cast<float*>(buf1.ptr);

                kipl::base::TImage<float,2> img(dims);

                std::copy_n(data,img.Size(),img.GetDataPtr());

                msc.process(img,quantile,th,esq);

                std::copy_n(img.GetDataPtr(),img.Size(),data);
            },

            "Cleans spots from the image in place using th as threshold and sigma as mixing width.",
            py::arg("x"),
            py::arg("quantile"),
            py::arg("th"),
            py::arg("method"));

    py::enum_<ImagingAlgorithms::eSortSpotQuantile>(m,"eSortSpotQuantile")
            .value("SortQuantileAll",           ImagingAlgorithms::eSortSpotQuantile::All)
            .value("SortQuantileBright",        ImagingAlgorithms::eSortSpotQuantile::BrightSpots)
            .value("SortQuantileDark",          ImagingAlgorithms::eSortSpotQuantile::DarkSpots)
            .value("SortQuantileBoth",          ImagingAlgorithms::eSortSpotQuantile::Both)
            .export_values();

}





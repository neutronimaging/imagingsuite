//<LICENSE>

#include <sstream>
#include <map>

#include <math/median.h>
#include <filters/stddevfilter.h>

#include <ImagingException.h>
#include <averageimage.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindAverageImage(py::module &m)
{
    py::class_<ImagingAlgorithms::AverageImage> avgClass(m, "AverageImage");
    avgClass.def(py::init());
    avgClass.def("windowSize", &ImagingAlgorithms::AverageImage::windowSize,"Returns the size of the filter window used by the weighted average");
    avgClass.def("setWindowSize", &ImagingAlgorithms::AverageImage::setWindowSize,"Set the size of the filter window used by the weighted average",py::arg("size")=5);
    avgClass.def("process",
                 [](ImagingAlgorithms::AverageImage &a,
                 py::array_t<float> &x,
                 ImagingAlgorithms::AverageImage::eAverageMethod method,
                 std::vector<float> weights = {})
    {
        // auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable

        py::buffer_info buf1 = x.request();
        if (buf1.ndim != 3) {
            throw std::runtime_error("Input array must have 3 dimensions");
        }

        std::vector<size_t> dims = {    static_cast<size_t>(buf1.shape[2]),
                                        static_cast<size_t>(buf1.shape[1]),
                                        static_cast<size_t>(buf1.shape[0])};
                                        
        kipl::base::TImage<float,3> stack(static_cast<float*>(buf1.ptr),dims);

        kipl::base::TImage<float,2> res=a(stack,method,weights);

        py::array_t<float> avg = py::array_t<float>(res.Size());
        avg.resize({res.Size(1),res.Size(0)});
        kipl::base::TImage<float,2> avgimg(static_cast<float*>(avg.request().ptr),dims);
        std::copy_n(res.GetDataPtr(),res.Size(),avgimg.GetDataPtr());
        return avg;
    },"Computes a combined image using the selected method",py::arg("img"),py::arg("method"),py::arg("weights")=std::vector<float>());


    py::enum_<ImagingAlgorithms::AverageImage::eAverageMethod>(avgClass,"eAverageMethod")
            .value("ImageSum",             ImagingAlgorithms::AverageImage::ImageSum)
            .value("ImageAverage",         ImagingAlgorithms::AverageImage::ImageAverage)
            .value("ImageMedian",          ImagingAlgorithms::AverageImage::ImageMedian)
            .value("ImageWeightedAverage", ImagingAlgorithms::AverageImage::ImageWeightedAverage)
            .value("ImageMin",             ImagingAlgorithms::AverageImage::ImageMin)
            .value("ImageMax",             ImagingAlgorithms::AverageImage::ImageMax)
            .export_values();

}

//<LICENSE>

#include <sstream>
#include <map>

#include <math/median.h>

#include <ImagingException.h>
#include <normalizeimage.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindNormalizeImage(py::module &m)
{
    py::class_<ImagingAlgorithms::NormalizeImage> normClass(m, "NormalizeImage");
    normClass.def(py::init<bool>());
    
    normClass.def("setReferences",
                 [](ImagingAlgorithms::NormalizeImage &a,
                 py::array_t<float> &_ff,
                 py::array_t<float> &_dc)
    {
        // auto rff = _ff.unchecked<2>(); // x must have ndim = 2; can be non-writeable

        py::buffer_info buf_ff = _ff.request();
        if (buf_ff.ndim != 2) {
            throw std::runtime_error("Input array _ff must have 2 dimensions");
        }

        std::vector<size_t> dims_ff = { static_cast<size_t>(buf_ff.shape[1]),
                                        static_cast<size_t>(buf_ff.shape[0])};
                                        
        kipl::base::TImage<float,2> ff(static_cast<float*>(buf_ff.ptr),dims_ff);

        // auto rdc = _dc.unchecked<2>(); // x must have ndim = 2; can be non-writeable

        py::buffer_info buf_dc = _dc.request();
        if (buf_dc.ndim != 2) {
            throw std::runtime_error("Input array _dc must have 2 dimensions");
        }

        std::vector<size_t> dims_dc = {    static_cast<size_t>(buf_dc.shape[1]),
                                        static_cast<size_t>(buf_dc.shape[0])};
                                        
        kipl::base::TImage<float,2> dc(static_cast<float*>(buf_dc.ptr),dims_dc);

        a.setReferences(ff,dc);
    },"Set the references to the normalization",py::arg("ff"),py::arg("dc"));


    normClass.def("setDoseROI", &ImagingAlgorithms::NormalizeImage::setDoseROI,"Set the ROI used to compute the projection dose", py::arg("roi"));

    normClass.def("usingLog", &ImagingAlgorithms::NormalizeImage::usingLog,"Returns True or False to tell if logarithm is used");

    normClass.def("process",
                 [](ImagingAlgorithms::NormalizeImage &a,
                 py::array_t<float> &x)
    {
        py::buffer_info buf1 = x.request();

        if (buf1.format!=py::format_descriptor<float>::format())
            throw std::runtime_error("Incompatible format: expected a float (32-bit) array!");

        if ((buf1.shape.size() != 2UL) && (buf1.shape.size() != 3UL))
            throw std::runtime_error("Incompatible format: expected a 3D array with projections!");

        switch (buf1.ndim)
        {
            case 2: 
            {
                std::vector<size_t> dims = { static_cast<size_t>(buf1.shape[1]),
                                             static_cast<size_t>(buf1.shape[0])};

                kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);
                img.Clone();
                a.process(img);
                std::copy_n(img.GetDataPtr(),img.Size(),static_cast<float*>(buf1.ptr));
                break;
            }                                            

            case 3: 
            {
                std::vector<size_t> dims = { static_cast<size_t>(buf1.shape[2]),
                                             static_cast<size_t>(buf1.shape[1]),
                                             static_cast<size_t>(buf1.shape[0])};

                kipl::base::TImage<float,3> img(static_cast<float*>(buf1.ptr),dims);

                a.process(img);
                std::copy_n(img.GetDataPtr(),img.Size(),static_cast<float*>(buf1.ptr));
                break;
            }      
        }                            
    
    },"Normalizes a single or several images using the provided reference data.",py::arg("img"));


}

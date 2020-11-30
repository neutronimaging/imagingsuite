//<LICENSE>
#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reconstructor.h"

namespace py = pybind11;

void bindReconstructor(py::module &m)
{
    py::class_<Reconstructor> reClass(m, "Reconstructor");

    reClass.def(py::init<eBackProjectors>());

    reClass.def("name",
                [](Reconstructor &recon)
                {
                    return recon.backProjector->Name();
                }
    );

    reClass.def("process",
                [](Reconstructor &recon,
                   py::array_t<float> &x)
                {
                    py::buffer_info buf1 = x.request();

                    if (buf1.format!=py::format_descriptor<float>::format())
                        throw std::runtime_error("Incompatible format: expected a float (32-bit) array!");

                    if (buf1.shape.size() != 3UL)
                        throw std::runtime_error("Incompatible format: expected a 3D array with projections!");

                    std::vector<size_t> dims={  static_cast<size_t>(buf1.shape[2]),
                                                static_cast<size_t>(buf1.shape[1]),
                                                static_cast<size_t>(buf1.shape[0])
                                             };

                    kipl::base::TImage<float,3> img(static_cast<float*>(buf1.ptr),dims);


                   // recon.backProjector->process(img,op);

                   // std::copy_n(img.GetDataPtr(),img.Size(),static_cast<float*>(buf1.ptr));
                }
    );

    //    sfClass.def("process",
    //                 [](ImagingAlgorithms::StripeFilter &sf,
    //                 py::array_t<float> &x,
    //                 ImagingAlgorithms::eStripeFilterOperation op)
    //            {
    //                py::buffer_info buf1 = x.request();

    //                size_t dims[]={static_cast<size_t>(buf1.shape[1]),
    //                               static_cast<size_t>(buf1.shape[0])};

    //                sf.checkDims(dims);

    //                kipl::base::TImage<float,2> img(static_cast<float*>(buf1.ptr),dims);

    //                sf.process(img,op);

    //                std::copy_n(img.GetDataPtr(),img.Size(),static_cast<float*>(buf1.ptr));
    //            },

    //            "Applies the stripe filter on the image as in-place operation.",
    //            py::arg("x"),
    //            py::arg("op"));

    py::enum_<eBackProjectors>(m,"eBackprojectors")
            .value("bpMultiProj",            bpMultiProj)
            .value("bpMultiProjParallel",    bpMultiProjParallel)
            .value("bpNearestNeighbor",      bpNearestNeighbor)
            .export_values();

}

#endif

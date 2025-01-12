//<LICENSE>
#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <ReconEngine.h>

namespace py = pybind11;

void bindReconEngine(py::module &m)
{
    py::class_<ReconEngine> reClass(m, "ReconEngine");

    reClass.def(py::init());

    reClass.def("Run3D",&ReconEngine::Run3D,"Starts the reconstruction process.");

//    sfClass.def("configure",
//                &ImagingAlgorithms::StripeFilter::configure,
//                "Configures the stripe filter. Note: the dims are given in x,y instead of r,c."
//                );

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

//    // sfClass.def("process",
//    //              [](ImagingAlgorithms::StripeFilter &sf,
//    //              py::array_t<double> &x,
//    //              ImagingAlgorithms::eStripeFilterOperation op)
//    //         {
//    //             py::buffer_info buf1 = x.request();

//    //             size_t dims[]={static_cast<size_t>(buf1.shape[1]),
//    //                            static_cast<size_t>(buf1.shape[0])};
//    //             double *data=static_cast<double*>(buf1.ptr);

//    //             kipl::base::TImage<float,2> img(dims);

//    //             std::copy_n(data,img.Size(),img.GetDataPtr());

//    //             sf.process(img,op);
//    //             std::copy_n(img.GetDataPtr(),img.Size(),data);
//    //         },
//    //         "Applies the stripe filter on the image as in-place operation.",
//    //         py::arg("x"),
//    //         py::arg("op"));

//    sfClass.def("dims",                &ImagingAlgorithms::StripeFilter::dims);
//    sfClass.def("waveletName",         &ImagingAlgorithms::StripeFilter::waveletName);
//    sfClass.def("decompositionLevels", &ImagingAlgorithms::StripeFilter::decompositionLevels);
//    sfClass.def("sigma",               &ImagingAlgorithms::StripeFilter::sigma);
//    sfClass.def("filterWindow",        &ImagingAlgorithms::StripeFilter::filterWindow);

//    py::enum_<ImagingAlgorithms::eStripeFilterOperation>(m,"eStripeFilterOperation")
//        .value("VerticalComponentZero", ImagingAlgorithms::VerticalComponentZero)
//        .value("VerticalComponentFFT",    ImagingAlgorithms::VerticalComponentFFT)
//        .export_values();



}
#endif

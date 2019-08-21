//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"
#include "../include/ReconFactory.h"
#include "../include/ReconConfig.h"
#include "../include/PreprocModuleBase.h"
#include "../include/ReconException.h"
#include <iostream>

ReconFactory::ReconFactory(void) :
	logger("ReconFactory")
{

}

ReconFactory::~ReconFactory(void)
{

}

ReconEngine * ReconFactory::BuildEngine(ReconConfig &config, kipl::interactors::InteractionBase *interactor)
{
	ReconEngine * engine=new ReconEngine("ReconEngine",interactor);

    try {
        engine->SetConfig(config);
    }
    catch (ReconException &e) {
        logger(logger.LogError,"Failed to get image size while building recon engine.");
        throw ReconException(e.what());
    }
    catch (kipl::base::KiplException &e) {
        logger(logger.LogError,"Failed to get image size while building recon engine.");
        throw kipl::base::KiplException(e.what());
    }
    catch (exception &e) {
        logger(logger.LogError,"Failed to get image size while building recon engine.");
        throw std::runtime_error(e.what());
    }


	std::list<ModuleConfig>::iterator it;

	// Setting up the preprocessing modules
    int i=0;
    for (it=config.modules.begin(); it!=config.modules.end(); it++, i++) {
		if (it->m_bActive==true) {
            ModuleItem *module=nullptr;
			try {
                module=new ModuleItem("muhrec",it->m_sSharedObject,it->m_sModule,interactor);

				module->GetModule()->Configure(config,it->parameters);
				engine->AddPreProcModule(module);
			}
			catch (ReconException &e) {
				throw ReconException(e.what(),__FILE__,__LINE__);
			}
		}
	}

    SetBackProjector(config,engine,interactor);
	return engine;

}

void ReconFactory::SetBackProjector(ReconConfig &config, ReconEngine * engine, kipl::interactors::InteractionBase *interactor)
{
    if (config.backprojector.m_bActive==true) {
        BackProjItem *module=nullptr;
        try {
            module=new BackProjItem("muhrecbp",config.backprojector.m_sSharedObject,config.backprojector.m_sModule,interactor);

            module->GetModule()->Configure(config,config.backprojector.parameters);
            engine->SetBackProjector(module);
        }
        catch (ReconException &e) {
            throw ReconException(e.what(),__FILE__,__LINE__);
        }
    }
}

#ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindReconFactory(py::module &m)
{
    py::class_<ReconFactory> rfClass(m, "ReconFactory");

    rfClass.def(py::init());

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

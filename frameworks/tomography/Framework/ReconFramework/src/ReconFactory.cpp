//<LICENSE>
#include "stdafx.h"
#include "../include/ReconFactory.h"
#include "../include/ReconConfig.h"
#include "../include/PreprocModuleBase.h"
#include "../include/ReconException.h"
#include <iostream>
#include <memory>

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

// #ifdef HAVEPYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;
void fillUserInformation(ReconConfig& config, py::dict& userInformation)
{
    for (auto item : userInformation) {
        const auto key = item.first.cast<std::string>();
        const auto value = item.second.cast<std::string>();
        if (key == "operator") {
            config.UserInformation.sOperator = value;
        } else if (key == "instrument") {
            config.UserInformation.sInstrument = value;
        } else if (key == "project_number") {
            config.UserInformation.sProjectNumber = value;
        } else if (key == "sample") {
            config.UserInformation.sSample = value;
        } else if (key == "comment") {
            config.UserInformation.sComment = value;
        } else if (key == "date") {
            config.UserInformation.sDate = value;
        } else if (key == "version") {
            config.UserInformation.sVersion = value;
        } else {
            throw std::runtime_error("Unknown key " + key + "for UserInformation");
        }
        std::cout << "UserInformation -- key: " << item.first << ", value: " << item.second << '\n';
    }
}
void fillSystem(ReconConfig& config, py::dict& system)
{
    std::cout << "TODO for system\n";
}
void fillProjections(ReconConfig& config, py::dict& projections)
{
    std::cout << "TODO for proj\n";
}
void fillMatrix(ReconConfig& config, py::dict& matrix)
{
    std::cout << "TODO for matrix\n";
}
void makeEngineFromDict(py::dict userInformation, py::dict system, py::dict projections, py::dict matrix) // , py::dict proj, py::dict matrix, py::dict processchain
{
    std::cout << "got called with\n";
    ReconConfig config{};
    fillUserInformation(config, userInformation);
    fillSystem(config, system);
    fillProjections(config, projections);
    fillMatrix(config, matrix);
    // fillModuleConfig(config, ...);
}

std::unique_ptr<ReconEngine> makeEngineFromXML(const std::string xmlFile, const std::string projectName)
{
    std::cout << "Loading config file '" << xmlFile << "' with project name '" << projectName << "'\n";
    ReconConfig config{};
    try {
        config.LoadConfigFile(xmlFile, projectName);
        std::cout << "Loaded with information uhh: " << config.UserInformation.sOperator << '\n';
    } catch (kipl::base::KiplException& a) {
        std::cout << "Encountered exception: " << a.what() << '\n';
    }

    ReconFactory factory{};
    return std::unique_ptr<ReconEngine>(factory.BuildEngine(config));
}
void bindReconFactory(py::module &m)
{
    // py::class_<ReconFactory> rfClass(m, "ReconFactory");

    // rfClass.def(py::init());
    m.def("makeEngine", &makeEngineFromDict,
        py::arg("userinformation"),
        py::arg("system"),
        py::arg("projections"),
        py::arg("matrix"));

    m.def("makeEngine", &makeEngineFromXML, py::arg("xml_file"), py::arg("project_name"));
    // py::arg("processchain")
    // m.def("Potatoes", []() {
    //     std::cout << "I am here\n";
    // });
}

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



// #endif

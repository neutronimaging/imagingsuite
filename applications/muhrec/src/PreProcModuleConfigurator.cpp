//<LICENSE>

#include "PreProcModuleConfigurator.h"
#include <sstream>
#include <ConfigBase.h>
#include <ReconException.h>
#include <ModuleException.h>
#include <base/KiplException.h>
#include <ReconFactory.h>
#include <ReconEngine.h>
#include "moduleconfigprogressdialog.h"

#include <QDebug>

PreProcModuleConfigurator::PreProcModuleConfigurator(ReconConfig *config, kipl::interactors::InteractionBase *interactor) :
    ModuleConfigurator(interactor)
{
	ModuleConfigurator::m_Config=dynamic_cast<ConfigBase *>(config);
}

PreProcModuleConfigurator::~PreProcModuleConfigurator() {
}

int PreProcModuleConfigurator::GetImage(std::string sSelectedModule, kipl::interactors::InteractionBase *interactor)
{
    ReconEngine *engine=nullptr;
	ReconFactory factory;

	ReconConfig * config=dynamic_cast<ReconConfig *>(m_Config);
	std::ostringstream msg;
	try {
        engine=factory.BuildEngine(*dynamic_cast<ReconConfig *>(m_Config),m_Interactor);
	}
	catch (ReconException &e) {
		msg<<"Failed to build the configuration engine with a ReconException: "<<e.what();
	}
	catch (ModuleException &e){
		msg<<"Failed to build the configuration engine with a ModuleException: "<<e.what();
	}
	catch (kipl::base::KiplException &e) {
		msg<<"Failed to build the configuration engine with a KiplException: "<<e.what();
	}

	catch (std::exception &e) {
		msg<<"Failed to build the configuration engine with a STL Exception: "<<e.what();
	}
	catch (...) {
		msg<<"Failed to build the configuration engine with an unhandled ";
	}
	if (!msg.str().empty()) {
		throw ModuleException(msg.str(),__FILE__,__LINE__);
	}

    logger.message("Engine successfully built");

    auto roi = config->ProjectionInfo.roi;
    bool bRunFailure=false;
    try {
        if (interactor==nullptr)
        {
            m_Image=engine->RunPreproc(roi,sSelectedModule);
        }
        else
        {
            ModuleConfigProgressDialog dlg(interactor);


            if (engine!=nullptr) {
                int res=0;

                res=dlg.exec(engine,roi,sSelectedModule);

                if (res == QDialog::Accepted)
                {
                    m_Image = dlg.getImage();
                }
                else
                    bRunFailure = true;
            }
        }
    }
    catch (ReconException &e)
    {
        msg<<"ReconException with message: "<<e.what();
        bRunFailure =true;
    }
    catch (ModuleException &e) {
        msg<<"ModuleException with message: "<<e.what();
        bRunFailure = true;
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"KiplException with message: "<<e.what();
        bRunFailure = true ;
    }
    catch (std::exception &e)
    {
        msg<<"STL exception with message: "<<e.what();
        bRunFailure = true;
    }
    catch (...)
    {
        msg<<"An unknown exception";
        bRunFailure =true;
    }

    if (bRunFailure==true)
    {
        logger.message(msg.str());
        return 0;
    }
    logger.message("Reconstruction done");


    if (engine!=nullptr)
		delete engine;
    return 1;
}

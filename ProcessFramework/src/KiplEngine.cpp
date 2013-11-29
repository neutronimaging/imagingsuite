//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2008-11-11 21:09:49 +0100 (Di, 11 Nov 2008) $
// $Rev: 13 $
//


#include "stdafx.h"

#include <algorithm>

#include <strings/filenames.h>
#include <io/io_stack.h>
#include <base/KiplException.h>

#include "../include/KiplEngine.h"
#include "../include/KiplFrameworkException.h"
#include "../include/KiplProcessModuleBase.h"

#include <ModuleException.h>

KiplEngine::KiplEngine(std::string name) : 
 logger(name),
	 m_InputImage(NULL)
{
}

KiplEngine::~KiplEngine(void)
{
	while (!m_ProcessList.empty()) {
		if (m_ProcessList.front()!=NULL) {
			delete m_ProcessList.front();
		}
		m_ProcessList.pop_front();
	}
}

void KiplEngine::SetConfig(KiplProcessConfig &config)
{
	m_Config = config;
}

size_t KiplEngine::AddProcessingModule(KiplModuleItem *module) 
{
	if (module!=NULL) {
		if (module->Valid())
			m_ProcessList.push_back(module);
	}
	else
		throw KiplFrameworkException("Failed to add module",__FILE__,__LINE__);

	return m_ProcessList.size();

}

int KiplEngine::Run(kipl::base::TImage<float,3> * img)
{
	std::ostringstream msg;
	m_InputImage=img;
	m_ResultImage=*m_InputImage;
	m_ResultImage.Clone();

	std::map<std::string, std::string> parameters;

	std::list<KiplModuleItem *>::iterator it_Module;
	try {
		for (it_Module=m_ProcessList.begin(); it_Module!=m_ProcessList.end(); it_Module++) {
			msg.str("");
			msg<<"Module "<<(*it_Module)->GetModule()->ModuleName();
			logger(kipl::logging::Logger::LogMessage,msg.str());
			(*it_Module)->GetModule()->Process(m_ResultImage,parameters);
		}
		
		msg.str("");
		msg<<"Execution times :\n";
		for (it_Module=m_ProcessList.begin(); it_Module!=m_ProcessList.end(); it_Module++) {
			msg<<"Module "<<(*it_Module)->GetModule()->ModuleName()<<": "<<(*it_Module)->GetModule()->ExecTime()<<"s\n";
		}
		logger(kipl::logging::Logger::LogMessage,msg.str());
	}
	catch (KiplFrameworkException &e) {
		throw KiplFrameworkException(e.what(),__FILE__,__LINE__);
	}
	catch (ModuleException &e) {
		msg.str("");
		msg<<"Got a ModuleException during execution of the process chain\n"<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &e) {
		msg.str("");
		msg<<"Got a KiplException during execution of the process chain\n"<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		msg.str("");
		msg<<"Got a STL Exception during execution of the process chain\n"<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	catch (...) {
		msg.str("");
		msg<<"Got an unknown exception during execution of the process chain\n";
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	
	}

	return 0;
}

kipl::base::TImage<float,3> & KiplEngine::GetResultImage()
{
	return m_ResultImage;
}

bool KiplEngine::SaveImage(KiplProcessConfig::cOutImageInformation * info)
{
	std::ostringstream msg;
	std::string fname;

	KiplProcessConfig::cOutImageInformation *config= info==NULL ? &m_Config.mOutImageInformation : info;


	kipl::strings::filenames::CheckPathSlashes(config->sDestinationPath,true);
		fname=config->sDestinationPath+config->sDestinationFileMask;

	try {
		std::stringstream msg;

		std::stringstream str;

		float maxval=0.0f;
		float minval=0.0f;
		switch (config->eResultImageType) {
			case kipl::io::TIFF8bits : maxval=255.0f; minval=0.0f; break;
			case kipl::io::TIFF16bits : maxval=65535.0f; minval=0.0f; break;
			case kipl::io::TIFFfloat: maxval=65535.0f; minval=0.0f; break;
			default : throw KiplFrameworkException("Trying to save unsupported file type",__FILE__,__LINE__);
		}

		if (config->bRescaleResult) {
			maxval=*std::max_element(m_ResultImage.GetDataPtr(),m_ResultImage.GetDataPtr()+m_ResultImage.Size());
			minval=*std::min_element(m_ResultImage.GetDataPtr(),m_ResultImage.GetDataPtr()+m_ResultImage.Size());
		}
		m_ResultImage.info.SetMetricX(m_InputImage->info.GetMetricX());
		m_ResultImage.info.SetMetricY(m_InputImage->info.GetMetricY());
        m_ResultImage.info.sArtist=m_Config.UserInformation.sOperator;
        m_ResultImage.info.sCopyright=m_Config.UserInformation.sOperator;
		m_ResultImage.info.sSoftware="Kipl Processing Framework";
        m_ResultImage.info.sDescription=m_Config.UserInformation.sSample;

		kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY;
		msg.str("");
		msg<<"Serializing results to "<<fname
				<<"\nMin="<<minval<<" max="<<maxval;
		logger(kipl::logging::Logger::LogMessage,msg.str());

		// Todo Rescaling must be fixed
		kipl::io::WriteImageStack(m_ResultImage,
				fname,
				minval,maxval,
				0,m_ResultImage.Size(2),m_Config.mImageInformation.nFirstFileIndex,
				config->eResultImageType,plane);
	}
	catch (kipl::base::KiplException &e) {
		msg<<"KiplException with message: "<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		msg<<"STL Exception with message: "<<e.what();
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);	
	}
	catch (...) {
		msg<<"Unknown exception while saving image";
		throw KiplFrameworkException(msg.str(),__FILE__,__LINE__);
	}
	msg.str("");
	msg<<"Saved the image "<<m_ResultImage;
	logger(kipl::logging::Logger::LogVerbose,msg.str());

	return true;
}

std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > >  KiplEngine::GetPlots()
{
	std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > > plotlist;
	std::ostringstream msg;
	std::list<KiplModuleItem *>::iterator it_Module;
	std::string sName;

	KiplProcessModuleBase *module=NULL;

	for (it_Module=m_ProcessList.begin(); it_Module!=m_ProcessList.end(); it_Module++) {
		module=dynamic_cast<KiplProcessModuleBase *>((*it_Module)->GetModule());
		sName=module->ModuleName();
		msg.str("");
		size_t N=module->Plots().size();
		msg<<"Module "<<sName<<" has "<<N<<" plots";
		logger(kipl::logging::Logger::LogVerbose,msg.str());
		if (N!=0) {
			plotlist[sName]=module->Plots();			
		}
	}

	return plotlist;	 
}

std::map<std::string, kipl::containers::PlotData<float,size_t> > KiplEngine::GetHistograms()
{
	std::map<std::string, kipl::containers::PlotData<float,size_t> > histlist;

	std::ostringstream msg;
	std::list<KiplModuleItem *>::iterator it_Module;
	std::string sName;

	KiplProcessModuleBase *module=NULL;

	for (it_Module=m_ProcessList.begin(); it_Module!=m_ProcessList.end(); it_Module++) {
		module=dynamic_cast<KiplProcessModuleBase *>((*it_Module)->GetModule());
		sName=module->ModuleName();
		
		
		if (module->HaveHistogram()) {
			msg.str("");
			msg<<"Module "<<sName<<" has a histogram";
			logger(kipl::logging::Logger::LogVerbose,msg.str());
			histlist[sName]=module->Histogram();			
		}
		
	}

	return histlist;
}

//<LICENSE>

#ifndef PREPROCMODULEBASE_H
#define PREPROCMODULEBASE_H

#include "ProcessFramework_global.h"
//#include "../src/stdafx.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <profile/Timer.h>
#include <containers/PlotData.h>
#include <string>
#include <map>
#include <list>

#include <ProcessModuleBase.h>
#include "KiplProcessConfig.h"

class PROCESSFRAMEWORKSHARED_EXPORT KiplProcessModuleBase : public ProcessModuleBase
{
public:

    KiplProcessModuleBase(std::string name="KiplProcessModuleBase", bool bComputeHistogram=false, kipl::interactors::InteractionBase *interactor=nullptr);
	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual ~KiplProcessModuleBase(void);
//	virtual int Configure(std::map<std::string, std::string> parameters)=0;
    virtual int Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters)=0;
	bool HavePlots() { return !m_PlotList.empty(); }
    std::map<std::string, kipl::containers::PlotData<float,float> > & Plots() { return m_PlotList; }
	bool HaveHistogram() { return (m_bComputeHistogram && (m_Histogram.Size()!=0)); }
	kipl::containers::PlotData<float,size_t> & Histogram() { return m_Histogram; }

protected:
    /// Hides the Configure method in the base class
    /// \param parameters A list of parameters to configure the module.
    virtual int Configure(std::map<std::string, std::string> parameters);

	void ComputeHistogram(kipl::base::TImage<float,3> &img, size_t N=256);
	std::map<std::string, kipl::containers::PlotData<float,float> > m_PlotList;
	kipl::containers::PlotData<float,size_t> m_Histogram;
	bool m_bComputeHistogram;
    size_t m_nHistogramSize;

};


#endif

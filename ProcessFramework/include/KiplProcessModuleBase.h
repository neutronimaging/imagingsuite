//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2011-08-17 16:22:51 +0200 (Mi, 17 Aug 2011) $
// $Rev: 1020 $
// $Id: PreprocModuleBase.h 1020 2011-08-17 14:22:51Z kaestner $
//

#ifndef __PREPROCMODULEBASE_H
#define __PREPROCMODULEBASE_H
#include "../src/stdafx.h"

#include <logging/logger.h>
#include <base/timage.h>
#include <profile/Timer.h>
#include <containers/PlotData.h>
#include <string>
#include <map>
#include <list>

#include <ProcessModuleBase.h>

class  DLL_EXPORT KiplProcessModuleBase : public ProcessModuleBase
{
public:
	KiplProcessModuleBase(std::string name="KiplProcessModuleBase", bool bComputeHistogram=false);
	virtual int Process(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters);
	virtual ~KiplProcessModuleBase(void);
	virtual int Configure(std::map<std::string, std::string> parameters)=0;
	bool HavePlots() { return !m_PlotList.empty(); }
	std::map<std::string, kipl::containers::PlotData<float,float> > & Plots() { return m_PlotList; }
	bool HaveHistogram() { return (m_bComputeHistogram && (m_Histogram.Size()!=0)); }
	kipl::containers::PlotData<float,size_t> & Histogram() { return m_Histogram; }
protected:
	void ComputeHistogram(kipl::base::TImage<float,3> &img, size_t N=256);
	std::map<std::string, kipl::containers::PlotData<float,float> > m_PlotList;
	kipl::containers::PlotData<float,size_t> m_Histogram;
	bool m_bComputeHistogram;
	size_t m_nHistogramSize;
};


#endif

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

#ifndef __KIPLENGINE_H
#define __KIPLENGINE_H

#include "ProcessFramework_global.h"
#include <map>
#include <string>

#include <logging/logger.h>
#include <containers/PlotData.h>


#include "../include/KiplProcessConfig.h"
#include "../include/KiplModuleItem.h"

class PROCESSFRAMEWORKSHARED_EXPORT KiplEngine
{
protected:
	kipl::logging::Logger logger;

public:
	KiplEngine(std::string name="KiplEngine");

	~KiplEngine(void);

	size_t AddProcessingModule(KiplModuleItem *module);
	int Run(kipl::base::TImage<float,3> * img);
	void SetConfig(KiplProcessConfig & config) ;

	bool SaveImage(KiplProcessConfig::cOutImageInformation * info=NULL);

//	size_t GetHistogram(float *axis, size_t *hist,size_t nBins);
	void GetMatrixDims(size_t *dims) {dims[0]=m_ResultImage.Size(0); dims[1]=m_ResultImage.Size(1); dims[2]=m_ResultImage.Size(2);}
	kipl::base::TImage<float,3> & GetResultImage();
	std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > > GetPlots();
	std::map<std::string, kipl::containers::PlotData<float,size_t> > GetHistograms();

protected:
	int Process();

	KiplProcessConfig m_Config;

	size_t m_FirstSlice;
	
	std::list<KiplModuleItem *> m_ProcessList;

	kipl::base::TImage<float,3> * m_InputImage;
	kipl::base::TImage<float,3> m_ResultImage;

	std::map<std::string, float> m_ProcessingCoefficients;

	bool m_bCancel;									//!< Cancel flag if true the reconstruction process will terminate
};

#endif

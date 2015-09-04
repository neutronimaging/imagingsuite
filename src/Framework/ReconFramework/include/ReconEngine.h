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

#ifndef __RECONENGINE_H
#define __RECONENGINE_H
#include "ReconFramework_global.h"
//#include "stdafx.h"

#include <list>
#include "PreprocModuleBase.h"
#include "BackProjectorBase.h"
#include "ProjectionReader.h"
#include "ReconHelpers.h"
#include "ModuleItem.h"
#include "InteractionBase.h"

#include <logging/logger.h>
#include <base/kiplenums.h>
#include <string>

class RECONFRAMEWORKSHARED_EXPORT ProjectionBlock
{
public:
    ProjectionBlock();
    ProjectionBlock(kipl::base::TImage<float,3> & projections, size_t *roi,std::map<std::string, std::string> parameters);
    ProjectionBlock(const ProjectionBlock &b);
    ProjectionBlock & operator=(const ProjectionBlock &b);
    ~ProjectionBlock();

    kipl::base::TImage<float,3> projections;
    size_t roi[4];
    std::map<std::string, std::string> parameters;
};

class RECONFRAMEWORKSHARED_EXPORT ReconEngine
{
protected:
	kipl::logging::Logger logger;

public:
	ReconEngine(std::string name="ReconEngine", InteractionBase *interactor=NULL);

	size_t AddPreProcModule(ModuleItem *module);
	void SetBackProjector(BackProjItem *module);
	int Run();
    int Run3D(bool bRerunBackproj=false);
	kipl::base::TImage<float,3> RunPreproc(size_t * roi, std::string sLastModule);
	void SetConfig(ReconConfig & config) ;

	bool Serialize(ReconConfig::cMatrix *matrixconfig);
	bool Serialize(size_t *dims);

	size_t GetHistogram(float *axis, size_t *hist,size_t nBins);
	void GetMatrixDims(size_t *dims) {dims[0]=m_Volume.Size(0); dims[1]=m_Volume.Size(1); dims[2]=m_Volume.Size(2);}
    kipl::base::TImage<float,2> GetSlice(size_t index, kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY);

	virtual ~ReconEngine(void);
protected:
    int Run3DFull();
    int Run3DBackProjOnly();
	int Process(size_t *roi);
	int Process3D(size_t *roi);
    int ProcessExistingProjections3D(size_t *roi);
    int BackProject3D(kipl::base::TImage<float,3> & projections, size_t *roi,std::map<std::string, std::string> parameters);
	bool UpdateProgress(float val, std::string msg);
	void Done();

	bool TransferMatrix(size_t *dims);
	ReconConfig m_Config;

	size_t m_FirstSlice;
	ProjectionReader m_ProjectionReader;             //!< Instance of the projection reader
	
	std::list<ModuleItem *> m_PreprocList;
	BackProjItem * m_BackProjector;

	kipl::base::TImage<float,3> m_Volume;
	std::map<float,ProjectionInfo> m_ProjectionList;
	std::map<std::string, float> m_PreprocCoefficients;

    std::list<ProjectionBlock> m_ProjectionBlocks;

	size_t nProcessedBlocks;						//!< Counts the number of processed blocks for the progress monitor
	size_t nProcessedProjections;					//!< Counts the number of processed projections for the progress monitor
	size_t nTotalProcessedProjections;				//!< Counts the total number of processed projections for the progress monitor
	size_t nTotalBlocks;							//!< The total number of blocks to process
	bool m_bCancel;									//!< Cancel flag if true the reconstruction process will terminate
	//eReconstructorStatus status;
	InteractionBase *m_Interactor;
};

#endif

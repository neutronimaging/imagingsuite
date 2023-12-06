//<LICENSE>

#ifndef STDBACKPROJECTORBASE_H
#define STDBACKPROJECTORBASE_H
#include "StdBackProjectors_global.h"

#include <string>
#include <list>
#include <vector>
#include <base/timage.h>
#include <ReconConfig.h>
#include <BackProjectorModuleBase.h>
#include <interactors/interactionbase.h>
#include <logging/logger.h>
#include <projectionfilter.h>

class STDBACKPROJECTORS_EXPORT StdBackProjectorBase : public BackProjectorModuleBase
{
protected:
	struct ProjectionInfo {
		ProjectionInfo(const float _angle, const float _weight) : 
			angle(_angle), weight(_weight) {}
		ProjectionInfo(const ProjectionInfo & p) : angle(p.angle), weight(p.weight) {}
		const ProjectionInfo & operator=(const ProjectionInfo & p) {angle=p.angle; weight=p.weight; return *this;}
		float angle;
		float weight;
	};
public:
    StdBackProjectorBase(std::string name, BackProjectorModuleBase::eMatrixAlignment align, kipl::interactors::InteractionBase *interactor=nullptr);
	virtual ~StdBackProjectorBase(void);
	virtual size_t Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection);
	virtual size_t Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters);
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual int Initialize() { return 0;}
	virtual std::map<std::string, std::string> GetParameters();

    virtual void SetROI(const std::vector<size_t> &roi);

	
	void GetHistogram(float *axis, size_t *hist,size_t nBins);
	void GetHistogram(std::vector<float> &axis, std::vector<size_t> &hist, size_t nBins);
    std::vector<size_t> GetMatrixDims();

	void ChangeMaskValue(float x);
	float Min();
	float Max();
	const std::vector<std::pair<size_t,size_t> > & GetMask() {return mask;}
protected:
    virtual void ClearAll();

	virtual void BackProject()=0;
	std::vector<std::pair<ProjectionInfo, kipl::base::TImage<float, 2> > > ProjectionList;
	kipl::base::TImage<float,3> projections;
	size_t nProjCounter; //!< Counts the projections in the buffer
	size_t SizeU;
	size_t SizeV;
	size_t SizeProj;
	size_t MatrixCenterX;

	float ProjCenter;
	float fWeights[1024];
	float fSin[1024];
	float fCos[1024];
	float fStartU[1024];
	float fLocalStartU[1024];

	size_t nProjectionBufferSize;
	size_t nSliceBlock;
	size_t nSubVolume[2];
	float fRotation;

    ImagingAlgorithms::ProjectionFilter filter;

};

#endif

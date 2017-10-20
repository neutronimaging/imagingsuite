//
// This file is part of the recon library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2010-07-14 16:28:45 +0200 (Mi, 14 Jul 2010) $
// $Rev: 657 $
//

#ifndef _BACKPROJECTORBASE_H_
#define _BACKPROJECTORBASE_H_

#include <string>
#include <list>
#include <vector>
#include <base/timage.h>
#include <ReconConfig.h>
#include <BackProjectorBase.h>
#include <InteractionBase.h>
#include <logging/logger.h>

class VectorBackProjectorBase : public BackProjectorBase
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
	VectorBackProjectorBase(std::string name, eMatrixAlignment align, InteractionBase *interactor=NULL);
	virtual ~VectorBackProjectorBase(void);
	virtual size_t Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection);
	virtual size_t Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters);
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual int Initialize() { return 0;}
	virtual std::map<std::string, std::string> GetParameters();

	virtual void SetROI(size_t *roi);

	void GetHistogram(float *axis, size_t *hist,size_t nBins);
	void GetMatrixDims(size_t *dims);

	void ChangeMaskValue(float x);
	float Min();
	float Max();
	const std::vector<std::pair<size_t,size_t> > & GetMask() {return mask;}
protected:
	void ClearAll();

	std::vector<std::pair<size_t,size_t> > mask;
	
	void BuildCircleMask();
	virtual void BackProject()=0;
	std::vector<std::pair<ProjectionInfo, kipl::base::TImage<float, 2> > > ProjectionList;
	kipl::base::TImage<float,3> projections;
	size_t nProjCounter; //!< Counts the projections in the buffer
	size_t SizeU;
	size_t SizeV;
	size_t SizeProj;
	size_t MatrixCenterX;
	size_t MatrixDims[3];
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

};

#endif

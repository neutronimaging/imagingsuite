/*
 * ReferenceImageCorrection.h
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

#ifndef IMGALG_REFERENCEIMAGECORRECTION_H_
#define IMGALG_REFERENCEIMAGECORRECTION_H_
#include "ImagingAlgorithms_global.h"

//#include "stdafx.h"
#include <base/timage.h>
#include <logging/logger.h>
namespace ImagingAlgorithms {

class IMAGINGALGORITHMSSHARED_EXPORT ReferenceImageCorrection {
protected:
	kipl::logging::Logger logger;
public:
	ReferenceImageCorrection();
	virtual ~ReferenceImageCorrection();

	void LoadReferenceImages(std::string obname, size_t obcnt,
			std::string dcname, size_t dccnt,
			std::string bbname, size_t bbcnt,
			size_t *doseroi,
			size_t *bbroi);

	void SetReferenceImages(kipl::base::TImage<float,2> *ob,
			kipl::base::TImage<float,2> *dc,
			kipl::base::TImage<float,2> *bb,
			size_t *doseroi,
			size_t *bbroi);

	void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;}

	void Process(kipl::base::TImage<float,2> &img, float dose);
	void Process(kipl::base::TImage<float,3> &img, float *dose);

protected:
	void PrepareReferences();
	void PrepareBlackBody(float w=10);

	void ComputeLogNorm(kipl::base::TImage<float,2> &img, float dose);
	void ComputeNorm(kipl::base::TImage<float,2> &img, float dose);

	bool m_bHaveOpenBeam;
	bool m_bHaveDarkCurrent;
	bool m_bHaveBlackBody;

	bool m_bComputeLogarithm;

	kipl::base::TImage<float,2> m_OpenBeam;
	kipl::base::TImage<float,2> m_DarkCurrent;
	kipl::base::TImage<float,2> m_BlackBody;

	float m_fOpenBeamDose;
	bool m_bHaveDoseROI;
	bool m_bHaveBlackBodyROI;

	size_t m_nDoseROI[4];
	size_t m_nBlackBodyROI[4];

};

}

#endif /* REFERENCEIMAGECORRECTION_H_ */

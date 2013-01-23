/*
 * ReferenceImageCorrection.h
 *
 *  Created on: Jan 17, 2013
 *      Author: anders
 */

#ifndef REFERENCEIMAGECORRECTION_H_
#define REFERENCEIMAGECORRECTION_H_

#include <base/timage.h>

class ReferenceImageCorrection {
public:
	ReferenceImageCorrection();
	virtual ~ReferenceImageCorrection();

	void SetReferenceInformation(std::string obname, size_t obcnt,
			std::string dcname, size_t dccnt,
			std::string bbname, size_t bbcnt,
			size_t *doseroi,
			size_t *bbroi);

	void ComputeLogartihm(bool x) {m_bComputeLogarithm=x;}

	void Process(kipl::base::TImage<float,2> &img, float dose);
	void Process(kipl::base::TImage<float,3> &img, float *dose);

protected:
	void PrepareBlackBody(float w=10);

	bool m_bHaveOpenBeam;
	bool m_bHaveDarkCurrent;
	bool m_bHaveBlackBody;

	bool m_bComputeLogarithm;

	kipl::base::TImage<float,2> m_OpenBeam;
	float m_fOpenBeamDose;
	kipl::base::TImage<float,2> m_DarkCurrent;
	kipl::base::TImage<float,2> m_BlackBody;

	size_t m_nDoseROI[4];
	size_t m_nBlackBodyROI[4];

};

#endif /* REFERENCEIMAGECORRECTION_H_ */

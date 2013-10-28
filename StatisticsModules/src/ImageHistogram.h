/*
 * MaskedHistogram.h
 *
 *  Created on: Jan 7, 2013
 *      Author: kaestner
 */

#ifndef MASKEDHISTOGRAM_H_
#define MASKEDHISTOGRAM_H_

#include <KiplProcessModuleBase.h>

class ImageHistogram: public KiplProcessModuleBase {
public:
	ImageHistogram();
	virtual ~ImageHistogram();

	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

	size_t m_nSize;
	float m_fMaskValue;
	bool m_bUseMask;
	std::string m_sFileName;
};

#endif /* MASKEDHISTOGRAM_H_ */

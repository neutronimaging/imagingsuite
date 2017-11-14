//
// This file is part of the kiptool library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2011-08-17 16:22:51 +0200 (Mi, 17 Aug 2011) $
// $Rev: 1020 $
// $Id: KiplProcessModuleBase.cpp 1020 2011-08-17 14:22:51Z kaestner $
//

#ifndef PCAFILTERMODULE_H_
#define PCAFILTERMODULE_H_

#include <KiplProcessModuleBase.h>
#include <map>
#include <string>
#include <math/covariance.h>
#include <pca/pca.h>

class PCAFilterModule: public KiplProcessModuleBase
{
public:
    PCAFilterModule();
    virtual ~PCAFilterModule();
	
	virtual int Configure(std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
protected:
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);
    int m_nLevel;
    kipl::math::eCovarianceType       m_eCovarianceType;
    kipl::pca::ePCA_DecompositionType m_eDecompositionType;
    bool m_bCenterData;
    bool m_bNormalizeData;

};

#endif /* DATASCALER_H_ */

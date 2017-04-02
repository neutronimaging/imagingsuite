/*
 * GeneralFilter.h
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */

#ifndef GENERALFILTER_H_
#define GENERALFILTER_H_

#include "StdPreprocModules_global.h"
#include <string>

#include <PreprocModuleBase.h>


class STDPREPROCMODULESSHARED_EXPORT GeneralFilter: public PreprocModuleBase {
public:
    enum eGeneralFilter {
        FilterBox,
        FilterGauss
    };
	GeneralFilter();
	virtual ~GeneralFilter();
	
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual std::map<std::string, std::string> GetParameters();
	virtual bool SetROI(size_t *roi);
protected:
    eGeneralFilter filterType;
    float filterSize;
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

};

void STDPREPROCMODULESSHARED_EXPORT string2enum(const std::string str,GeneralFilter::eGeneralFilter &ft);
std::string STDPREPROCMODULESSHARED_EXPORT enum2string(GeneralFilter::eGeneralFilter ft);

std::ostream STDPREPROCMODULESSHARED_EXPORT & operator<<(std::ostream & s, GeneralFilter::eGeneralFilter ft);

#endif /* GENERALFILTER_H_ */

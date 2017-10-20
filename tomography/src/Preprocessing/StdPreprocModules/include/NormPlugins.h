//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//


#ifndef _NORMPLUGINS_H_
#define _NORMPLUGINS_H_

#include "StdPreprocModules_global.h"
#include <logging/logger.h>
#include <base/timage.h>
#include <math/LUTCollection.h>
#include <PreprocModuleBase.h>
#include "PreprocEnums.h"
#include <ReconConfig.h>
#include <averageimage.h>

class  STDPREPROCMODULESSHARED_EXPORT NormBase :
	public PreprocModuleBase
{
public:
	NormBase(std::string name);
	virtual ~NormBase(void);
	
	virtual void LoadReferenceImages(size_t *roi) = 0;
	virtual bool SetROI(size_t *roi);
	virtual std::map<std::string, std::string> GetParameters();
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

protected:
	virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat);
    virtual kipl::base::TImage<float,2> ReferenceLoader(std::string fname,
                                                        int firstIndex,
                                                        int N, size_t *roi,
                                                        float initialDose,
                                                        float doseBias,
                                                        ReconConfig &config, float &dose);
	ReconConfig m_Config;
	std::string path;
	std::string flatname;
	std::string darkname;
    std::string blackbodyname;

	size_t nOBCount;
	size_t nOBFirstIndex;
	size_t nDCCount;
	size_t nDCFirstIndex;
    size_t nBBCount;
    size_t nBBFirstIndex;

    ImagingAlgorithms::AverageImage::eAverageMethod m_ReferenceAvagerage;

	kipl::base::TImage<float,2> mFlatField;
	kipl::base::TImage<float,2> mDark;
    kipl::base::TImage<float,2> mBlack;

	eNormFunctionType mNormFunction;
	float fFlatDose; 
	float fDarkDose;
    float fBlackDose;
    // chiara begin
    float fBackScatt;
    bool bBackScatt;
    // chiara end

	bool bUseNormROI;
	bool bUseLUT;
    bool bUseWeightedMean;

	size_t nNormRegion[4];
	size_t nOriginalNormRegion[4];
};

class  STDPREPROCMODULESSHARED_EXPORT FullLogNorm : public NormBase
{
public:
	FullLogNorm();
	virtual ~FullLogNorm();

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

	virtual void LoadReferenceImages(size_t *roi);

protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

private:
	virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat);
	kipl::math::LogLUT LUT;
};

class  STDPREPROCMODULESSHARED_EXPORT FullNorm : public NormBase
{
public:
    FullNorm();
    virtual ~FullNorm();

    virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

    virtual void LoadReferenceImages(size_t *roi);

protected:
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff);
    virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff);

private:
    virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat);
};

class NegLogNorm : public NormBase
{
public:
	NegLogNorm();
	virtual ~NegLogNorm();

	virtual void LoadReferenceImages(size_t *roi);


	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
protected:
    virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, string> & coeff);
};

class  STDPREPROCMODULESSHARED_EXPORT NegLogProjection : public NormBase
{
public:
	NegLogProjection();
	virtual ~NegLogProjection();
	virtual void LoadReferenceImages(size_t *roi);

	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, float> & coeff);
private:
	float mDose;
};

class  STDPREPROCMODULESSHARED_EXPORT LogProjection : public NormBase
{
public:
	LogProjection();
	virtual ~LogProjection();
	virtual void LoadReferenceImages(size_t *roi);
	virtual std::map<std::string, std::string> GetParameters();
	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);

protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, string> & coeff);
	virtual int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, string> & coeff);
	float fFactor;
};

class  STDPREPROCMODULESSHARED_EXPORT InvProjection : public NormBase
{
public:
	InvProjection();
	virtual ~InvProjection();
	virtual void Configure(ReconConfig *config);
	virtual void LoadReferenceImages(size_t *roi);
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, float> & coeff);
};

class  STDPREPROCMODULESSHARED_EXPORT DoseWeightProjection : public NormBase
{
public:
	DoseWeightProjection();
	virtual ~DoseWeightProjection();
	virtual void LoadReferenceImages(size_t *roi);


	virtual int Configure(ReconConfig config, std::map<std::string, std::string> parameters);
	virtual int Initialize() {return 0;}
protected:
	virtual int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, float> & coeff);
};

void NormPluginBuilder(ReconConfig * config, NormBase ** plugin);

#endif

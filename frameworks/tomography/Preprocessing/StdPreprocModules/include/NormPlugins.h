//<LICENSE>

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
	~NormBase(void);
	
    virtual void LoadReferenceImages(const std::vector<size_t> &roi) = 0;
    bool SetROI(const std::vector<size_t> &roi) override;
	std::map<std::string, std::string> GetParameters() override;
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

protected:
	virtual void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat);
    virtual kipl::base::TImage<float,2> ReferenceLoader(std::string fname,
                                                        int firstIndex,
                                                        int N, const std::vector<size_t> &roi,
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

    std::vector<size_t> nNormRegion;
    std::vector<size_t> nOriginalNormRegion;
};

class  STDPREPROCMODULESSHARED_EXPORT FullLogNorm : public NormBase
{
public:
	FullLogNorm();
	virtual ~FullLogNorm();

	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

    void LoadReferenceImages(const std::vector<size_t> &roi) override;

protected:
	using NormBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
	int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

private:
	void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat) override;
	kipl::math::LogLUT LUT;
};

class  STDPREPROCMODULESSHARED_EXPORT FullNorm : public NormBase
{
public:
    FullNorm();
    virtual ~FullNorm();

    int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

    void LoadReferenceImages(const std::vector<size_t> &roi) override;

protected:
	using NormBase::ProcessCore;
    int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
    int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff) override;

private:
    void SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat) override;
};

class NegLogNorm : public NormBase
{
public:
	NegLogNorm();
	virtual ~NegLogNorm();

    void LoadReferenceImages(const std::vector<size_t> &roi) override;


	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
protected:
	using NormBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, string> & coeff) override;
};

class  STDPREPROCMODULESSHARED_EXPORT NegLogProjection : public NormBase
{
public:
	NegLogProjection();
	virtual ~NegLogProjection();
    void LoadReferenceImages(const std::vector<size_t> &roi) override;

	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
protected:
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
private:
	// float mDose;
};

class  STDPREPROCMODULESSHARED_EXPORT LogProjection : public NormBase
{
public:
	LogProjection();
	virtual ~LogProjection();
    void LoadReferenceImages(const std::vector<size_t> &roi) override;
	std::map<std::string, std::string> GetParameters() override;
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;

protected:
	using NormBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, string> & coeff) override;
	int ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, string> & coeff) override;
	float fFactor;
};

class  STDPREPROCMODULESSHARED_EXPORT InvProjection : public NormBase
{
public:
	InvProjection();
	virtual ~InvProjection();
	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
	void LoadReferenceImages(const std::vector<size_t> &roi) override;
protected:
	using NormBase::ProcessCore;
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
};

class  STDPREPROCMODULESSHARED_EXPORT DoseWeightProjection : public NormBase
{
public:
	DoseWeightProjection();
	virtual ~DoseWeightProjection();
	void LoadReferenceImages(const std::vector<size_t> &roi) override;


	int Configure(ReconConfig config, std::map<std::string, std::string> parameters) override;
	int Initialize() override {return 0;}
protected:
	int ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff) override;
};

void NormPluginBuilder(ReconConfig * config, NormBase ** plugin);

#endif

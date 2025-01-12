//<LICENSE>

#include <atomic>

#include "../include/StdPreprocModules_global.h"


#include <math/image_statistics.h>
#include <math/median.h>
#include <strings/miscstring.h>
#include <strings/filenames.h>

#include <ReconException.h>
#include <ProjectionReader.h>
#include <ReconConfig.h>
#include <analyzefileext.h>

#include <ParameterHandling.h>

#include "../include/NormPlugins.h"

NormBase::NormBase(std::string name) :
    PreprocModuleBase(name),
    m_Config(""),
    nOBCount(0),
    nOBFirstIndex(1),
    nDCCount(0),
    nDCFirstIndex(1),
    nBBCount(0),
    nBBFirstIndex(1),
    m_ReferenceAvagerage(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    fFlatDose(1.0f),
    fBlackDose(1.0f),
    bUseNormROI(true),
    bUseLUT(false),
    bUseWeightedMean(false),
    nNormRegion(4,0UL),
    nOriginalNormRegion(4,0UL)
{
    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner"}),
                                       "MuhRec - a new tomography reconstructor",
                                       "Nuclear Instruments and Methods Section A",
                                       2011,
                                       651,
                                       1,
                                       "156-160",
                                       "10.1016/j.nima.2011.01.129"));

    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner","C. Carminati"}),
                                       "MuhRec software",
                                       "Zenodo",
                                       2019,
                                       1,
                                       1,
                                       "no pages",
                                       "10.5281/zenodo.1117850"));
}

NormBase::~NormBase(void) {}

bool NormBase::SetROI(const std::vector<size_t> &roi)
{
	std::stringstream msg;
	msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger.message(msg.str(),__FUNCTION__);
	LoadReferenceImages(roi);
    nNormRegion = nOriginalNormRegion;

	return true;
}


int NormBase::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	m_Config    = config;
	path        = config.ProjectionInfo.sReferencePath;
	flatname    = config.ProjectionInfo.sOBFileMask;
	darkname    = config.ProjectionInfo.sDCFileMask;

	nOBCount      = config.ProjectionInfo.nOBCount;
	nOBFirstIndex = config.ProjectionInfo.nOBFirstIndex;

	nDCCount      = config.ProjectionInfo.nDCCount;
	nDCFirstIndex = config.ProjectionInfo.nDCFirstIndex;

	bUseNormROI = kipl::strings::string2bool(GetStringParameter(parameters,"usenormregion"));
    try {
        bUseLUT     = kipl::strings::string2bool(GetStringParameter(parameters,"uselut"));
    }
    catch (...) {
        logger(logger.LogWarning,"Parameter uselut missing, using uselut=false");
        bUseLUT=false;
    }

    try {
        string2enum(GetStringParameter(parameters,"referenceaverage"),m_ReferenceAvagerage);
    }
    catch (...) {
        logger(logger.LogWarning,"referenceaverage not found, using image average.");
        m_ReferenceAvagerage = ImagingAlgorithms::AverageImage::ImageAverage;
    }

    nOriginalNormRegion = config.ProjectionInfo.dose_roi;

	return 0;
}

void NormBase::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{
	mDark=dark;
	mFlatField=flat;
}

kipl::base::TImage<float,2> NormBase::ReferenceLoader(std::string fname,
                                                      int firstIndex, int N,
                                                      const std::vector<size_t> &roi,
                                                      float initialDose,
                                                      float doseBias,
                                                      ReconConfig &config,
                                                      float &dose)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, refimg;

    float tmpdose = 0.0f;

    if (fname.empty() && N!=0)
        throw ReconException("The reference image file name mask is empty",__FILE__,__LINE__);

    std::string fmask=fname;

    std::string filename,ext;
    ProjectionReader reader;

    dose = initialDose; // A precaution in case no dose is calculated

    if (N!=0)
    {
        msg.str(""); msg<<"Loading "<<N<<" reference images";
        logger.message(msg.str(),__FUNCTION__);

        float *fDoses=new float[N];

        auto exttype = readers::GetFileExtensionType(fmask);

        if (exttype == readers::ExtensionHDF5 )
        {
            img     = reader.ReadNexus(fmask, firstIndex,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);

            tmpdose = bUseNormROI ? reader.GetProjectionDoseNexus(fmask, firstIndex,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : initialDose;
        }
        else
        {
            kipl::strings::filenames::MakeFileName(fmask,firstIndex,filename,ext,'#','0');
            img     = reader.Read(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);

            tmpdose = bUseNormROI ? reader.GetProjectionDose(filename,
                        config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        nOriginalNormRegion) : initialDose;
        }

        tmpdose   = tmpdose - doseBias;
        dose      = tmpdose;
        fDoses[0] = tmpdose;

        std::vector<size_t> obdims={img.Size(0), img.Size(1),static_cast<size_t>(N)};

        kipl::base::TImage<float,3> img3D(obdims);
        memcpy(img3D.GetDataPtr(),img.GetDataPtr(),img.Size()*sizeof(float));

        for (int i=1; i<N; ++i) {
            kipl::strings::filenames::MakeFileName(fmask,i+firstIndex,filename,ext,'#','0');

            if (exttype != readers::ExtensionHDF5 )
            {
                img=reader.Read(filename,
                        m_Config.ProjectionInfo.eFlip,
                        m_Config.ProjectionInfo.eRotate,
                        m_Config.ProjectionInfo.fBinning,
                        roi);
                memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                tmpdose = bUseNormROI ? reader.GetProjectionDose(filename,
                            config.ProjectionInfo.eFlip,
                            config.ProjectionInfo.eRotate,
                            config.ProjectionInfo.fBinning,
                            nOriginalNormRegion) : initialDose;
            }
            else
            {
                    img=reader.ReadNexus(fmask,i+firstIndex,
                            m_Config.ProjectionInfo.eFlip,
                            m_Config.ProjectionInfo.eRotate,
                            m_Config.ProjectionInfo.fBinning,
                            roi);
                    memcpy(img3D.GetLinePtr(0,i),img.GetDataPtr(),img.Size()*sizeof(float));

                    tmpdose = bUseNormROI ? reader.GetProjectionDoseNexus(fmask,i+firstIndex,
                                config.ProjectionInfo.eFlip,
                                config.ProjectionInfo.eRotate,
                                config.ProjectionInfo.fBinning,
                                nOriginalNormRegion) : initialDose;
            }

            tmpdose   = tmpdose - doseBias;
            dose     += tmpdose;
            fDoses[i] = fDoses[0]/tmpdose;

        }

        fDoses[0]=1.0f;

        dose/=static_cast<float>(N);
        msg.str(""); msg<<"Dose="<<dose;
        logger(logger.LogMessage,msg.str());

        float *tempdata=new float[N];
        refimg.resize(img.dims());

        ImagingAlgorithms::AverageImage avg;

        refimg = avg(img3D,m_ReferenceAvagerage);

        delete [] tempdata;
        delete [] fDoses;

        if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram)
        {
            float *pFlat=refimg.GetDataPtr();
            for (size_t i=1; i<refimg.Size(1); i++)
            {
                memcpy(refimg.GetLinePtr(i), pFlat, sizeof(float)*refimg.Size(0));

            }
        }
    }
    else
        logger(kipl::logging::Logger::LogWarning,"Reference image count is zero");

    msg.str(""); msg<<"Loaded reference image (dose="<<dose<<")";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    return refimg;
}

std::map<std::string, std::string> NormBase::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["usenormregion"]    = kipl::strings::bool2string(bUseNormROI);
    parameters["uselut"]           = kipl::strings::bool2string(bUseLUT);
    parameters["referenceaverage"] = enum2string(m_ReferenceAvagerage);

	return parameters;
}

//-------------------------------------------------
FullLogNorm::FullLogNorm() :
		NormBase("FullLogNorm"),
		LUT(1<<16,0.0f,(1<<16)-1)
{

}

FullLogNorm::~FullLogNorm()
{
}

int FullLogNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
	NormBase::Configure(config,parameters);
	return 0;
}

void FullLogNorm::LoadReferenceImages(const std::vector<size_t> &roi)
{
    std::ostringstream msg;

	kipl::base::TImage<float,2> img, flat, dark;

	if (flatname.empty() && nOBCount!=0)
		throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
	if (darkname.empty() && nDCCount!=0)
		throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

	std::string flatmask=path+flatname;
	std::string darkmask=path+darkname;
	std::string filename,ext;
	ProjectionReader reader;



	fDarkDose=0.0f;
	fFlatDose=1.0f;

    if (nDCCount!=0) {
        dark = ReferenceLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,roi,0.0f,0.0f,m_Config,fDarkDose);
    }


    if (nOBCount!=0) {
        flat = ReferenceLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,roi,1.0f,fDarkDose,m_Config,fFlatDose);
    }

//    if (dark.Size()==0){
//        dark.Resize(flat.Dims());
//        dark = 0.0f;
//    }

    msg.str(""); msg<<"Loaded reference images (flat dose="<<fFlatDose<<", dark dose="<<fDarkDose<<")";
    logger(kipl::logging::Logger::LogMessage,msg.str());
	SetReferenceImages(dark, flat);
}

void FullLogNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{
	NormBase::SetReferenceImages(dark,flat);
	
    float dose=1.0f/(fFlatDose);

	if (dose!=dose)
		throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    if (isinf(dose)==true)
        throw ReconException("The reference dose is Inf",__FILE__,__LINE__);

	const int N=static_cast<int>(flat.Size());
	float *pFlat=flat.GetDataPtr();
	float *pDark=dark.GetDataPtr();

	if (nDCCount!=0) {
		#pragma omp parallel for
		for (int i=0; i<N; i++) {
			float fProjPixel=pFlat[i]-pDark[i];
			if (fProjPixel<=0)
				pFlat[i]=0;
			else
				pFlat[i]=log(fProjPixel*dose);
		}
	}
	else {
		#pragma omp parallel for
		for (int i=0; i<N; i++) {
			float fProjPixel=pFlat[i];
			if (fProjPixel<=0)
				pFlat[i]=0;
			else
				pFlat[i]=log(fProjPixel*dose);
		}
	}

	if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (int i=1; i<static_cast<int>(flat.Size(1)); i++) {
				memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
	}
}

int FullLogNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
	std::stringstream msg;
	float dose=1.0f;
	if (bUseNormROI==true) {
		dose=GetFloatParameter(coeff,"dose")-fDarkDose;
        dose= dose<=0.0f ? 1.0f : dose;
	}

	if (dose!=dose) 
		throw ReconException("The projection dose is a NaN",__FILE__,__LINE__);

	float cLogDose=log(dose);

	int N=static_cast<int>(img.Size());

	float *pImg=img.GetDataPtr();
	float *pFlat=mFlatField.GetDataPtr();
	float *pDark=mDark.GetDataPtr();
	if (static_cast<int>(mFlatField.Size())!=N) {
		msg.str("");
		msg<<"Flat field ("<<mFlatField.Size(0)<<", "<<mFlatField.Size(1)<<"),"
			<<" does not match projection ("<<img.Size(0)<<", "<<img.Size(1)<<")"
			<<"(N flat="<<mFlatField.Size()<<", N proj="<<img.Size()<<")";
		throw ReconException(msg.str(),__FILE__, __LINE__);
	}

    if (bUseLUT == true) {
		if (nDCCount!=0){
			#pragma omp parallel for firstprivate(pImg,pDark)
			for (int i=0; i<N; i++) {
				pImg[i]=pImg[i]-pDark[i];
			}
		}

		LUT.InPlace(pImg,img.Size());
		
		#pragma omp parallel for firstprivate(pImg,pFlat)
		for (int i=0; i<N; i++) {
			pImg[i]=pFlat[i]-pImg[i]+cLogDose;
		}
	}
	else {
		if (nDCCount!=0) {
			#pragma omp parallel for firstprivate(pImg,pFlat,pDark)
			for (int i=0; i<N; i++) {
				float fProjPixel=(pImg[i]-pDark[i]);
				if (fProjPixel<=0)
					pImg[i]=0;
				else
                    pImg[i]=pFlat[i]-log(fProjPixel)+cLogDose;
			}
		}
		else {
			#pragma omp parallel for firstprivate(pImg,pFlat)
			for (int i=0; i<N; i++) {
				float fProjPixel=pImg[i];
				if (fProjPixel<=0)
					pImg[i]=0;
				else
                    pImg[i]=pFlat[i]-log(fProjPixel)+cLogDose;
			}
		}
	}

	return 0;
}

int FullLogNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	int nDose=img.Size(2);
    std::vector<float> doselist;

	std::stringstream msg;
	
	if (bUseNormROI==true) {
		GetFloatParameterVector(coeff,"dose",doselist,nDose);
        msg.str("");
        msg<<"Dose vector length "<<doselist.size();
        logger.message(msg.str());
		for (int i=0; i<nDose; i++) {
			doselist[i] = doselist[i]-fDarkDose;
			doselist[i] = log(doselist[i]<1 ? 1.0f : doselist[i]);
		}
	}
	else {
		nDose=1;
		doselist[0]=0.0f;
	}
	
    int N=static_cast<int>(img.Size(0)*img.Size(1)); // Projection size

    if ((nOBCount!=0) && (static_cast<int>(mFlatField.Size())!=N)) {
		msg.str("");
		msg<<"Flat field ("<<mFlatField.Size(0)<<", "<<mFlatField.Size(1)<<"),"
			<<" does not match projection ("<<img.Size(0)<<", "<<img.Size(1)<<")"
			<<"(N flat="<<mFlatField.Size()<<", N proj="<<img.Size()<<")";
        logger(logger.LogError,msg.str());
		throw ReconException(msg.str(),__FILE__, __LINE__);
	}

    if ((nDCCount!=0) && (static_cast<int>(mDark.Size())!=N)) {
        msg.str("");
        msg<<"Dark field ("<<mDark.Size(0)<<", "<<mDark.Size(1)<<"),"
            <<" does not match projection ("<<img.Size(0)<<", "<<img.Size(1)<<")"
            <<"(N flat="<<mDark.Size()<<", N proj="<<img.Size()<<")";
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__, __LINE__);
    }

    float *pFlat=mFlatField.GetDataPtr();
    float *pDark=mDark.GetDataPtr();

    std::atomic<float> cnt;
    cnt=0.0f;
	if (bUseLUT) {
		#pragma omp parallel for firstprivate(pDark, pFlat)
        for (int j=0; j<static_cast<int>(img.Size(2)); j++) {
			float *pImg=img.GetLinePtr(0,j);
			float dose=nDose !=1 ? doselist[j] : doselist[0];

			if (nDCCount!=0) {
				for (int i=0; i<N; i++) {
					pImg[i]=pImg[i]-pDark[i];
				}
			}
			LUT.InPlace(pImg,N);
			
			if (nOBCount!=0) {
				for (int i=0; i<N; i++) {
					pImg[i]=pFlat[i]-pImg[i]+dose;
				}
			}
			else {
				dose-=doselist[0];
				for (int i=0; i<N; i++) {
					pImg[i]=-pImg[i]+dose;
				}
			}
		}
	}
	else {
		if (nDCCount!=0) {
			if (nOBCount!=0) {
                    #pragma omp parallel for firstprivate(pFlat,pDark)
//                    for (int j=0; j<static_cast<int>(img.Size(2)) && (UpdateStatus(cnt/static_cast<int>(img.Size(2)),m_sModuleName)==false); cnt=cnt+1,++j) {
                     for (int j=0; j<static_cast<int>(img.Size(2));++j) {
						float *pImg=img.GetLinePtr(0,j);
						float dose=nDose !=1 ? doselist[j] : doselist[0];
						
						for (int i=0; i<N; i++) {
							float fProjPixel=(pImg[i]-pDark[i]);
                            if (fProjPixel<=0.0f)
                                pImg[i]=0.0f;
							else
								pImg[i]=pFlat[i]-log(fProjPixel)+dose;
						}
					}
			}
			else {
				#pragma omp parallel for firstprivate(pDark)
//                for (int j=0; j<static_cast<int>(img.Size(2)) && (UpdateStatus(cnt/static_cast<int>(img.Size(2)),m_sModuleName)==false); cnt=cnt+1,++j) {
                for (int j=0; j<static_cast<int>(img.Size(2)); ++j) {
					float *pImg=img.GetLinePtr(0,j);
					float dose=nDose !=1 ? doselist[j] : doselist[0];
					
					for (int i=0; i<N; i++) {
						float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=0.0f)
                            pImg[i]=0.0f;
						else
							pImg[i]=-log(fProjPixel)+dose;
					}
				}
			}
		}
		else {
			if (nOBCount!=0) {
				#pragma omp parallel for firstprivate(pFlat)
//                for (int j=0; j<static_cast<int>(img.Size(2)) && (UpdateStatus(cnt/static_cast<int>(img.Size(2)),m_sModuleName)==false); cnt=cnt+1,++j) {
                for (int j=0; j<static_cast<int>(img.Size(2)); ++j) {
					float *pImg=img.GetLinePtr(0,j);
					float dose=nDose !=1 ? doselist[j] : doselist[0];
					for (int i=0; i<N; i++) {
						float fProjPixel=(pImg[i]);
                        if (fProjPixel<=0.0f)
                            pImg[i]=0.0f;
						else
							pImg[i]=pFlat[i]-log(fProjPixel)+dose;
					}	
				}
			}
			else {
				#pragma omp parallel for
//                for (int j=0; j<static_cast<int>(img.Size(2)) && (UpdateStatus(cnt/static_cast<int>(img.Size(2)),m_sModuleName)==false); cnt=cnt+1, ++j) {
                for (int j=0; j<static_cast<int>(img.Size(2)); ++j) {
					float *pImg=img.GetLinePtr(0,j);
					float dose=nDose !=1 ? doselist[j] : doselist[0];
					for (int i=0; i<N; i++) {
						float fProjPixel=(pImg[i]);
                        if (fProjPixel<=0.0f)
                            pImg[i]=0.0f;
						else
							pImg[i]=-log(fProjPixel)+dose;
					}	
				}
			}
		}
	}

    return 0;
}

//----------------------------------------------------
// Fullnorm module

FullNorm::FullNorm() :
        NormBase("FullNorm")
{

}

FullNorm::~FullNorm()
{
}

int FullNorm::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    NormBase::Configure(config,parameters);
    return 0;
}

void FullNorm::LoadReferenceImages(const std::vector<size_t> &roi)
{
    std::ostringstream msg;

    kipl::base::TImage<float,2> img, flat, dark;

    if (flatname.empty() && nOBCount!=0)
        throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);
    if (darkname.empty() && nDCCount!=0)
        throw ReconException("The dark current image mask is empty",__FILE__,__LINE__);

    std::string flatmask=path+flatname;
    std::string darkmask=path+darkname;
    std::string filename,ext;
    ProjectionReader reader;



    fDarkDose=0.0f;
    fFlatDose=1.0f;

    if (nDCCount!=0) {
        dark = ReferenceLoader(darkmask,m_Config.ProjectionInfo.nDCFirstIndex,m_Config.ProjectionInfo.nDCCount,roi,0.0f,0.0f,m_Config,fDarkDose);
    }

    flat = ReferenceLoader(flatmask,m_Config.ProjectionInfo.nOBFirstIndex,m_Config.ProjectionInfo.nOBCount,roi,1.0f,fDarkDose,m_Config,fFlatDose);

    if (dark.Size()==0){
        dark.resize(flat.dims());
        dark = 0.0f;
    }


    msg.str(""); msg<<"Loaded reference images (flat dose="<<fFlatDose<<", dark dose="<<fDarkDose<<")";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    SetReferenceImages(dark, flat);
}

void FullNorm::SetReferenceImages(kipl::base::TImage<float,2> dark, kipl::base::TImage<float,2> flat)
{
    NormBase::SetReferenceImages(dark,flat);

    float dose=1.0f/(fFlatDose);
    if (dose!=dose)
        throw ReconException("The reference dose is a NaN",__FILE__,__LINE__);

    const int N=static_cast<int>(flat.Size());
    float *pFlat=flat.GetDataPtr();
    float *pDark=dark.GetDataPtr();

    if (nDCCount!=0) {
        #pragma omp parallel for
        for (int i=0; i<N; i++) {
            float fProjPixel=pFlat[i]-pDark[i];
            if (fProjPixel<=0.0f)
                pFlat[i]=1.0f;
            else
                pFlat[i]=1.0f/(fProjPixel*dose);
        }
    }
    else {
        #pragma omp parallel for
        for (int i=0; i<N; i++) {
            float fProjPixel=pFlat[i];
            if (fProjPixel<=0.0f)
                pFlat[i]=1.0f;
            else
                pFlat[i]=1.0f/(fProjPixel*dose);
        }
    }

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            for (int i=1; i<static_cast<int>(flat.Size(1)); i++) {
                memcpy(flat.GetLinePtr(i),flat.GetLinePtr(0),sizeof(float)*flat.Size(0));			}
    }
}

int FullNorm::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    std::stringstream msg;
    float dose=1.0f;
    if (bUseNormROI==true) {
        dose=GetFloatParameter(coeff,"dose")-fDarkDose;
        dose= dose<=0 ? 1.0f : 1.0f/dose;
    }

    if (dose!=dose)
        throw ReconException("The projection dose is a NaN",__FILE__,__LINE__);

    float cDose=dose;

    int N=static_cast<int>(img.Size());

    float *pImg=img.GetDataPtr();
    float *pFlat=mFlatField.GetDataPtr();
    float *pDark=mDark.GetDataPtr();
    if (static_cast<int>(mFlatField.Size())!=N) {
        msg.str("");
        msg<<"Flat field ("<<mFlatField.Size(0)<<", "<<mFlatField.Size(1)<<"),"
            <<" does not match projection ("<<img.Size(0)<<", "<<img.Size(1)<<")"
            <<"(N flat="<<mFlatField.Size()<<", N proj="<<img.Size()<<")";
        throw ReconException(msg.str(),__FILE__, __LINE__);
    }


    if (nDCCount>0) {
        #pragma omp parallel for firstprivate(pImg,pFlat,pDark)
        for (int i=0; i<N; i++) {
            float fProjPixel=(pImg[i]-pDark[i]);
            if (fProjPixel<=0)
                pImg[i]=0;
            else
                pImg[i]=pFlat[i]*fProjPixel*cDose;
        }
    }
    else {
        #pragma omp parallel for firstprivate(pImg,pFlat)
        for (int i=0; i<N; i++) {
            float fProjPixel=pImg[i];
            if (fProjPixel<=0)
                pImg[i]=0;
            else
                pImg[i]=pFlat[i]*fProjPixel*cDose;
        }
    }

    return 0;
}

int FullNorm::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    int nDose=img.Size(2);
    float *doselist=new float[nDose];

    std::stringstream msg;

    if (bUseNormROI==true) {
        GetFloatParameterVector(coeff,"dose",doselist,nDose);
        for (int i=0; i<nDose; i++) {
            doselist[i] = doselist[i]-fDarkDose;
            doselist[i] = doselist[i]<1 ? 1.0f : 1.0f/doselist[i];
        }
    }
    else {
        nDose=1;
        doselist[0]=1.0f;
    }

    int N=static_cast<int>(img.Size(0)*img.Size(1));

    if ((nDCCount!=0) && (static_cast<int>(mFlatField.Size())!=N))
    {
        msg.str("");
        msg<<"Flat field ("<<mFlatField.Size(0)<<", "<<mFlatField.Size(1)<<"),"
            <<" does not match projection ("<<img.Size(0)<<", "<<img.Size(1)<<")"
            <<"(N flat="<<mFlatField.Size()<<", N proj="<<img.Size()<<")";
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__, __LINE__);
    }

    float *pFlat=mFlatField.GetDataPtr();
    float *pDark=mDark.GetDataPtr();

    if (nDCCount>0) {
        if (nOBCount>0) {
                
                #pragma omp parallel for firstprivate(pFlat,pDark)
                for (int j=0; j<static_cast<int>(img.Size(2)); ++j) 
                {
                    float *pImg=img.GetLinePtr(0,j);
                    float dose=nDose !=1 ? doselist[j] : doselist[0];

                    for (int i=0; i<N; i++) 
                    {
                        float fProjPixel=(pImg[i]-pDark[i]);
                        if (fProjPixel<=0)
                            pImg[i]=0;
                        else
                            pImg[i]=pFlat[i]*fProjPixel*dose;
                    }
                }
        }
        else {
            #pragma omp parallel for firstprivate(pDark)
            for (int j=0; j<static_cast<int>(img.Size(2)); ++j) 
            {
                float *pImg=img.GetLinePtr(0,j);
                float dose=nDose !=1 ? doselist[j] : doselist[0];

                for (int i=0; i<N; i++) 
                {
                    float fProjPixel=(pImg[i]-pDark[i]);
                    if (fProjPixel<=0)
                        pImg[i]=0;
                    else
                        pImg[i]=fProjPixel*dose;
                }
            }
        }
    }
    else {
        if (nOBCount>0) 
        {
            #pragma omp parallel for firstprivate(pFlat)
            for (int j=0; j<static_cast<int>(img.Size(2)); ++j) 
            {
                float *pImg=img.GetLinePtr(0,j);
                float dose=nDose !=1 ? doselist[j] : doselist[0];
                for (int i=0; i<N; i++) {
                    float fProjPixel=(pImg[i]);
                    if (fProjPixel<=0)
                        pImg[i]=0;
                    else
                        pImg[i]=pFlat[i]*fProjPixel*dose;
                }
            }
        }
        else 
        {
            #pragma omp parallel for
            for (int j=0; j<static_cast<int>(img.Size(2)); ++j) 
            {
                float *pImg=img.GetLinePtr(0,j);
                float dose=nDose !=1 ? doselist[j] : doselist[0];
                for (int i=0; i<N; i++) 
                {
                    float fProjPixel=(pImg[i]);
                    if (fProjPixel<=0)
                        pImg[i]=0;
                    else
                        pImg[i]=fProjPixel*dose;
                }
            }
        }
    }

    delete [] doselist;
    return 0;
}
//------------------------------------------------------------------------------
NegLogNorm::NegLogNorm() : NormBase("NegLogNorm")
{
	throw ReconException("NegLogNorm is not implemented");
}

NegLogNorm::~NegLogNorm() 
{}

int NegLogNorm::Configure(  ReconConfig /*config*/, 
                            std::map<std::string, std::string> /*parameters*/)
{
    return 0;
}

void NegLogNorm::LoadReferenceImages(const std::vector<size_t> & /*roi*/)
{}

int NegLogNorm::ProcessCore(kipl::base::TImage<float,2> & /*img*/, 
                            std::map<string, string>    & /*coeff*/)
{
    return 0;
}

//------------------------------------------------------------------------------
NegLogProjection::NegLogProjection() : NormBase("NegLogProjection")
{
	
}

NegLogProjection::~NegLogProjection() {}

int NegLogProjection::Configure(ReconConfig /*config*/, 
                                std::map<std::string, std::string> /*parameters*/)
{
//	path        = config->Projections.sPath;
//	flatname    = config->Projections.sFileMask;
//	darkname.clear();
//
//	nCount      = 1;
//	nFirstIndex = config->Reconstructor.Norm.nFirstIndex;
//
//	bUseNormROI = config->Reconstructor.Norm.bUseNormRegion;
//
//	memcpy(nOriginalNormRegion,config->Reconstructor.Norm.nNormRegion,4*sizeof(size_t));
	return 0;
}

void NegLogProjection::LoadReferenceImages(const std::vector<size_t> & /*roi*/)
{
	kipl::base::TImage<float,2> flat, dark;

	if (flatname.empty())
		throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);

	if (path.empty())
		throw ReconException("The reference path is empty",__FILE__,__LINE__);

//	ProjectionReader reader;
//
//	flat=reader.Read(path, flatname, nFirstIndex, roi);

	SetReferenceImages(dark, flat);
}

int NegLogProjection::ProcessCore(  kipl::base::TImage<float,2> & img, 
                                    std::map<std::string, std::string> & /*coeff*/)
{

	float cLogDose=0.0f;
	if (this->bUseNormROI)
		cLogDose=logf(fFlatDose/static_cast<float>(kipl::math::RegionMean(img,nNormRegion)));

	const int N=static_cast<int>(img.Size());
	float *pImg=img.GetDataPtr();
	#pragma omp parallel for firstprivate(pImg)
	for (int i=0; i<N; i++) {
		float fProjPixel=pImg[i];
		if (fProjPixel<=0)
			pImg[i]=0;
		else
			pImg[i]=-logf(fProjPixel)+cLogDose;
	}

	return 0;
}

//----------------------------------------------------------------------
LogProjection::LogProjection() : NormBase("LogProjection"), fFactor(-1.0f)
{
}

LogProjection::~LogProjection() {}

int LogProjection::Configure(   ReconConfig /*config*/, 
                                std::map<std::string, std::string> parameters)
{
	fFactor = GetFloatParameter(parameters,"factor");

	return 0;
}

std::map<std::string, std::string> LogProjection::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["factor"]="-1";

	return parameters;
}

void LogProjection::LoadReferenceImages(const std::vector<size_t> & /*roi */) 
{}

int LogProjection::ProcessCore( kipl::base::TImage<float,2> & img, 
                                std::map<std::string, string> & /*coeff*/)
{
	float *pImg=img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		if (0.0f<pImg[i])
			pImg[i]=fFactor*logf(pImg[i]);
		else
			pImg[i]=0.0f;

	}

	return 0;
}

int LogProjection::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, string> & /*coeff*/)
{
	float *pImg=img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++) {
		if (0.0f<pImg[i])
			pImg[i]=fFactor*logf(pImg[i]);
		else
			pImg[i]=0.0f;

	}

	return 0;
}

InvProjection::InvProjection() : NormBase("InvProjection")
{
	throw ReconException("InvProjection is not implemented");
}
InvProjection::~InvProjection() 
{}

int InvProjection::Configure(ReconConfig /*config*/, std::map<std::string, std::string> /*parameters*/) 
{
    return 0;
}

void InvProjection::LoadReferenceImages(const std::vector<size_t> & /*roi*/) 
{}

int InvProjection::ProcessCore(kipl::base::TImage<float,2> & /*img*/, std::map<std::string, std::string> & /*coeff*/)
{
	return 0;
}

//----------------------------------------------------------------------

DoseWeightProjection::DoseWeightProjection() : NormBase("DoseWeightProjection")
{
}

DoseWeightProjection::~DoseWeightProjection() 
{
}

int DoseWeightProjection::Configure(ReconConfig /*config*/, std::map<std::string, std::string> /*parameters*/)
{
//	path        = config->Projections.sPath;
//	flatname    = config->Projections.sFileMask;
//	darkname.clear();
//
//	nCount      = 1;
//	nFirstIndex = config->Reconstructor.Norm.nFirstIndex;
//
//	bUseNormROI = config->Reconstructor.Norm.bUseNormRegion;
//
//	memcpy(nOriginalNormRegion,config->Reconstructor.Norm.nNormRegion,4*sizeof(size_t));
	return 0;
}

void DoseWeightProjection::LoadReferenceImages(const std::vector<size_t> & /*roi*/) 
{
	kipl::base::TImage<float,2> flat, dark;

	if (flatname.empty())
		throw ReconException("The flat field image mask is empty",__FILE__,__LINE__);

	if (path.empty())
		throw ReconException("The reference path is empty",__FILE__,__LINE__);
//
//	ProjectionReader reader;
//
//	flat=reader.Read(path, flatname, nFirstIndex, roi);

	SetReferenceImages(dark, flat);
}

int DoseWeightProjection::ProcessCore(  kipl::base::TImage<float,2> & img, 
                                        std::map<std::string, std::string> & /*coeff*/)
{
	float fDose=1.0f;
	if (this->bUseNormROI)
		fDose=fFlatDose/static_cast<float>(kipl::math::RegionMean(img,nNormRegion));

	const int N=static_cast<int>(img.Size());
	float *pImg=img.GetDataPtr();
	#pragma omp parallel for firstprivate(pImg)
	for (int i=0; i<N; i++) 
		pImg[i]=fDose*pImg[i];

	return 0;
}


void NormPluginBuilder(ReconConfig * /*config*/, NormBase ** /*plugin*/)
{
//	int selector = static_cast<int>(config->Reconstructor.Norm.bUseOpenBeam)
//		     + (static_cast<int>(config->Reconstructor.Norm.bUseDarkCurrent)<<1);
//
//	switch (selector) {
//		case 0: // No references
//			switch (config->Reconstructor.Norm.eNormFunction) {
//				case Norm_DoseWeight:
//					*plugin = reinterpret_cast<NormBase *>(new DoseWeightProjection);
//					break;
//				case Norm_Log:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//				case Norm_NegLog:
//					*plugin = reinterpret_cast<NormBase *>(new NegLogProjection);
//					break;
//			}
//			break;
//		case 1: // Dark current only
//			switch (config->Reconstructor.Norm.eNormFunction) {
//				case Norm_DoseWeight:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//				case Norm_Log:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//				case Norm_NegLog:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//			}
//			break;
//		case 2: // Open beam only
//			switch (config->Reconstructor.Norm.eNormFunction) {
//				case Norm_DoseWeight:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//				case Norm_Log:
//					throw ReconException("Cannot create the requested logarithm module.",__FILE__,__LINE__);
//					break;
//				case Norm_NegLog:
//					*plugin = reinterpret_cast<NormBase *>(new NegLogNorm);
//					break;
//			}
//			break;
//		case 3: // Full reference information
//			*plugin = reinterpret_cast<NormBase *>(new FullLogNorm);
//			break;
//		default:
//			throw ReconException("Unkown reference image combination",__FILE__, __LINE__);
	
//	}


}
//----------------------------------------------------------------------



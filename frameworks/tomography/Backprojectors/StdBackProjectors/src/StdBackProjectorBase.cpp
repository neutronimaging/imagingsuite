//<LICENSE>

#include "../include/StdBackProjectorBase.h"
#include "../include/ReconException.h"
#include "../include/ReconHelpers.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include <ParameterHandling.h>

#include <strings/miscstring.h>
#include <base/tpermuteimage.h>
#include <math/mathconstants.h>

#include <QDebug>

#define USE_PROJ_PADDING

StdBackProjectorBase::StdBackProjectorBase(std::string name, eMatrixAlignment align,kipl::interactors::InteractionBase *interactor) :
BackProjectorModuleBase("muhrec",name, align, interactor),
nProjCounter(0), 
SizeU(0),
SizeV(0),
SizeProj(0),
MatrixCenterX(0),
ProjCenter(0.0f),
nProjectionBufferSize(16),
nSliceBlock(32),
fRotation(0.0f),
filter(nullptr)
{
	logger(kipl::logging::Logger::LogMessage,"c'tor StdBackProjectorBase");
    nSubVolume[0]=nSubVolume[1]=1;
}

StdBackProjectorBase::~StdBackProjectorBase(void)
{
}

void StdBackProjectorBase::ClearAll()
{
	logger(kipl::logging::Logger::LogVerbose,"Enter clear all");
    //volume
    BackProjectorModuleBase::ClearAll();
	ProjectionList.clear();

    //projection parameters
	nProjCounter=0;
	SizeU=0;
	SizeV=0;
	SizeProj=0;
	MatrixCenterX=0;

	ProjCenter=0.0;
	memset(fWeights,0,sizeof(float)*1024);
	memset(fSin,0,sizeof(float)*1024);
	memset(fCos,0,sizeof(float)*1024);
	memset(fStartU,0,sizeof(float)*1024);
	memset(fLocalStartU,0,sizeof(float)*1024);
	logger(kipl::logging::Logger::LogVerbose,"Leave clear all");
}


size_t StdBackProjectorBase::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{
    std::ostringstream msg;
	if (volume.Size()==0)
		throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

	proj.Clone();

    filter.process(proj);

	ProjCenter=mConfig.ProjectionInfo.fCenter;

    float dirWeight = 2.0f*(mConfig.ProjectionInfo.eDirection-0.5f);
    msg.str("");

	fWeights[nProjCounter]  = weight;
    fSin[nProjCounter]      = sin(dirWeight*angle*fPi/180.0f);
    fCos[nProjCounter]      = cos(dirWeight*angle*fPi/180.0f);
	fStartU[nProjCounter]   = MatrixCenterX*(fSin[nProjCounter]-fCos[nProjCounter])+ProjCenter;
    float *pProj=nullptr;
	
	kipl::base::TImage<float,2> img;
	proj*=weight;
	size_t N=0;
	if (MatrixAlignment==MatrixZXY) {
		kipl::base::Transpose<float,8> transpose;
		img=transpose(proj);
		pProj=img.GetDataPtr();
		N=img.Size(0);
	}
	else {
		pProj=proj.GetDataPtr();
		N=proj.Size(0);
	}

#ifdef USE_PROJ_PADDING
    for (int i=0; i<static_cast<int>(projections.Size(1)); i++) {
		memcpy(projections.GetLinePtr(i,nProjCounter),pProj+i*N,N*sizeof(float));
	}
#else
	memcpy(projections.GetLinePtr(0,nProjCounter),pProj,proj.Size()*sizeof(float));
#endif
    nProjCounter++;
    if (bLastProjection || (nProjectionBufferSize<=(nProjCounter))) {
        if (nProjectionBufferSize<=nProjCounter)
            nProjCounter--;

        msg.str("");
        msg<<"Counter="<<nProjCounter<<", buffer size="<<nProjectionBufferSize<<" last "<<(bLastProjection ? "True" : "False");
        logger(logger.LogDebug,msg.str());
		this->BackProject();
		nProjCounter=0;
	}


	return nProjCounter;
}

size_t StdBackProjectorBase::Process(kipl::base::TImage<float,3> projections, std::map<std::string, std::string> parameters)
{
	if (volume.Size()==0)
		throw ReconException("The target matrix is not allocated.",__FILE__,__LINE__);

    kipl::base::TImage<float,2> img(projections.dims());

	size_t nProj=projections.Size(2);
	// Extract the projection parameters
	float *weights=new float[nProj+16];
	GetFloatParameterVector(parameters,"weights",weights,nProj);


	float *angles=new float[nProj+16];
	GetFloatParameterVector(parameters,"angles",angles,nProj);

	// Process the projections
	float *pImg=img.GetDataPtr();
    float *pProj=nullptr;
	size_t i=0;
	for (i=0; (i<nProj) && (!UpdateStatus(static_cast<float>(i)/nProj, "Back-projecting")); i++) {
		pProj=projections.GetLinePtr(0,i);
		memcpy(pImg,pProj,sizeof(float)*img.Size());
		Process(img,angles[i],weights[i],i==(nProj-1));
	}

	delete [] weights;
	delete [] angles;
	return 0;
}

void StdBackProjectorBase::SetROI(const std::vector<size_t> &roi)
{
	ClearAll();
	ProjCenter    = mConfig.ProjectionInfo.fCenter;
	SizeU         = roi[2]-roi[0];
    if (mConfig.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram)
        SizeV = roi[3];
    else
        SizeV = roi[3]-roi[1];

    mConfig.ProjectionInfo.roi=roi;

	SizeProj      = SizeU*SizeV;
	size_t rest=0;
#ifdef USE_PROJ_PADDING
	rest = SizeV & 3 ;
	rest = rest !=0 ? 4 - rest : 0;
#endif
    std::vector<size_t> projDims = { SizeU, SizeV + rest, nProjectionBufferSize };

	if (MatrixAlignment==MatrixZXY) {
        MatrixDims = { SizeV, SizeU, SizeU };

		std::swap(projDims[0],   projDims[1]);
	}
	else {
        MatrixDims = { SizeU, SizeU, SizeV };
	}

    volume.resize(MatrixDims);
	volume=0.0f;

	stringstream msg;
	
	msg<<"Setting up reconstructor with ROI=["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]"<<std::endl;
	msg<<"Matrix dimensions "<<volume<<std::endl;
    projections.resize(projDims);
	projections=0.0f;
	msg<<"Projection buffer dimensions "<<projections<<std::endl;
	logger(kipl::logging::Logger::LogVerbose,msg.str());

	BuildCircleMask();
    MatrixCenterX = volume.Size(1)/2;
}

void StdBackProjectorBase::GetMatrixDims(size_t *dims)
{
	if (MatrixAlignment==MatrixZXY) {
		dims[0]=volume.Size(1);
		dims[1]=volume.Size(2);
		dims[2]=volume.Size(0);
	}
	else {
		dims[0]=volume.Size(0);
		dims[1]=volume.Size(1);
		dims[2]=volume.Size(2);
	} 
}

void StdBackProjectorBase::ChangeMaskValue(float x)
{
	const size_t N=std::max(std::max(volume.Size(0),volume.Size(1)),volume.Size(2));
	
	float *data=new float[N];
	for (size_t i=0; i<N; i++) {
		data[i]=x;
	}
	
	if (MatrixAlignment==StdBackProjectorBase::MatrixXYZ) {
		logger(kipl::logging::Logger::LogWarning,"ChangeMaskValue is not implemented for MatrixXYZ");
		throw ReconException("ChangeMaskValue is not implemented for MatrixXYZ",__FILE__,__LINE__);
	}
	else {
		for (size_t y=0; y<mask.size(); y++) {
			for (size_t x=0; x<mask[y].first; x++) {
				float *pLine=volume.GetLinePtr(x,y);
				memcpy(pLine,data,volume.Size(0)*sizeof(float));
			}
			
			for (size_t x=mask[y].second+1; x<volume.Size(1); x++) {
				float *pLine=volume.GetLinePtr(x,y);
				memcpy(pLine,data,volume.Size(0)*sizeof(float));
			}

		}	
	}
	
	delete [] data;
}


void StdBackProjectorBase::GetHistogram(float *axis, size_t *hist,size_t nBins)
{
	float matrixMin=Min();
	float matrixMax=Max();
	ostringstream msg;

	msg<<"Preparing histogram; #bins: "<<nBins<<", Min: "<<matrixMin<<", Max: "<<matrixMax;
	logger(kipl::logging::Logger::LogMessage,msg.str());

	memset(axis,0,sizeof(float)*nBins);
	memset(hist,0,sizeof(size_t)*nBins);
	float scale=(matrixMax-matrixMin)/(nBins+1);
	float invScale=1.0f/scale;
	size_t index=0;
	for (size_t y=0; y<mask.size(); y++) {
		for (size_t x=mask[y].first; x<mask[y].second; x++) {
			float *pLine=volume.GetLinePtr(x,y);
			for (size_t z=0; z<volume.Size(0); z++) { 
				index=static_cast<size_t>(invScale*(pLine[z]-matrixMin));
                if (pLine[z]<matrixMin)
					index=0;
				else if (nBins<=index)
					index=nBins-1;
				hist[index]++;
			}
		}
	}

	axis[0]=matrixMin+scale/2.0f;
	for (size_t i=1; i<nBins; i++)
		axis[i]=axis[i-1]+scale;
}

float StdBackProjectorBase::Min()
{
	float minval=std::numeric_limits<float>::max();
	
    if (MatrixAlignment==StdBackProjectorBase::MatrixXYZ) {
        logger(kipl::logging::Logger::LogWarning,"Min is not implemented for MatrixXYZ");
        throw ReconException("Min is not implemented for MatrixXYZ",__FILE__,__LINE__);
    }
    else {
		for (size_t y=0; y<mask.size(); y++) {
			for (size_t x=mask[y].first; x<mask[y].second; x++) {
				float *pLine=volume.GetLinePtr(x,y);
				minval=min(minval,*min(pLine,pLine+volume.Size(0)));
			}
        }
    }

	return minval;
}

float StdBackProjectorBase::Max()
{
	float maxval=-std::numeric_limits<float>::max();
	
    if (MatrixAlignment==StdBackProjectorBase::MatrixXYZ) {
        logger(kipl::logging::Logger::LogWarning,"Max is not implemented for MatrixXYZ");
        throw ReconException("Max is not implemented for MatrixXYZ",__FILE__,__LINE__);
    }
    else {
		for (size_t y=0; y<mask.size(); y++) {
			for (size_t x=mask[y].first; x<mask[y].second; x++) {
				float *pLine=volume.GetLinePtr(x,y);
				maxval=max(maxval,*min(pLine,pLine+volume.Size(0)));
			}
        }
    }
	
	return maxval;
}

int StdBackProjectorBase::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    qDebug()<<"b";
	mConfig=config;
qDebug()<<"b";
    nProjectionBufferSize = GetIntParameter(parameters,"ProjectionBufferSize");
    nSliceBlock           = GetIntParameter(parameters,"SliceBlock");
	GetUIntParameterVector(parameters,"SubVolume",nSubVolume,2);
    filter.setParameters(parameters);
    qDebug()<<"b";
	return 0;
}

std::map<std::string, std::string> StdBackProjectorBase::GetParameters()
{
	std::map<std::string, std::string> parameters;
    parameters = filter.parameters();
	parameters["ProjectionBufferSize"]=kipl::strings::value2string(nProjectionBufferSize);
	parameters["SliceBlock"]= kipl::strings::value2string(nSliceBlock);
	
	parameters["SubVolume"]=kipl::strings::value2string(nSubVolume[0])+" "+kipl::strings::value2string(nSubVolume[1]);
	return parameters;
}

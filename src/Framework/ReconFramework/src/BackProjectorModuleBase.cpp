//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//
#include "stdafx.h"

#include <math/mathconstants.h>

#include "../include/BackProjectorModuleBase.h"
#include "../include/ReconException.h"

BackProjectorModuleBase::BackProjectorModuleBase(std::string application, std::string name, eMatrixAlignment align, kipl::interactors::InteractionBase *interactor) :
	logger(name),
	MatrixAlignment(align),
	m_sModuleName(name),
	m_sApplication(application),
	m_Interactor(interactor)
{
	logger(kipl::logging::Logger::LogMessage,"C'tor BackProjBase");
    if (m_Interactor!=nullptr) {
		logger(kipl::logging::Logger::LogVerbose,"Got an interactor");
	}
	else {
		logger(kipl::logging::Logger::LogVerbose,"An interactor was not provided");

	}
}

BackProjectorModuleBase::~BackProjectorModuleBase(void)
{
}

size_t BackProjectorModuleBase::Process(kipl::base::TImage<float,2> proj, float angle, float weight, bool bLastProjection)
{
	std::ostringstream msg;

	msg<<"Backprojector module "<<m_sModuleName<<" does not support 2D processing.";
	throw ReconException(msg.str(),__FILE__,__LINE__);

	return 0;
}

size_t BackProjectorModuleBase::Process(kipl::base::TImage<float,3> proj, std::map<std::string, std::string> parameters)
{
	std::ostringstream msg;

	msg<<"Backprojector module "<<m_sModuleName<<" does not support 3D processing.";
	throw ReconException(msg.str(),__FILE__,__LINE__);

	return 0;
}


kipl::base::TImage<float,2> BackProjectorModuleBase::GetSlice(size_t idx)
{

    std::ostringstream msg;
	size_t origin[2]={0,0};
	size_t dims[2]={0,0};
	if (mConfig.MatrixInfo.bUseROI) {
		dims[0]=mConfig.MatrixInfo.roi[2]-mConfig.MatrixInfo.roi[0]+1;
		dims[1]=mConfig.MatrixInfo.roi[3]-mConfig.MatrixInfo.roi[1]+1;

		origin[0]=mConfig.MatrixInfo.roi[0];
		origin[1]=mConfig.MatrixInfo.roi[1];
	}
	else {
		switch (MatrixAlignment) {
			case MatrixXYZ : 
				dims[0]=volume.Size(0);
				dims[1]=volume.Size(1);
				break;
			case MatrixZXY : 
				dims[0]=volume.Size(1);
				dims[1]=volume.Size(2);
				break;
		}
	}

	kipl::base::TImage<float,2> slice(dims);

//    std::cout << "slice dimensions: " << std::endl; // sono giuste
//    std::cout << dims[0] << " " << dims[1] << std::endl;

    size_t sxy=volume.Size(0)*volume.Size(1);
	size_t sx=volume.Size(0);

//    std::cout << "Volume size in BackProjectorModuleBase::GetSlice(size_t idx)" << std::endl;
//    std::cout<< volume.Size(0) << " " << volume.Size(1) << " " << volume.Size(2) << std::endl;

    float *pVolume=volume.GetDataPtr()+idx;
	float *pSlice=slice.GetDataPtr();
	switch (MatrixAlignment) {
		case MatrixXYZ : 
            if (volume.Size(2)<=idx){
                msg<<"Slice index greater than matrix size (XYZ matrix). Slice index is "<<idx<<" and Matrix Sixe is: " << volume.Size(2);
//				throw ReconException("Slice index greater than matrix size (XYZ matrix)",__FILE__,__LINE__);
            throw ReconException(msg.str(),__FILE__,__LINE__);
            }

			for (size_t i=0; i<slice.Size(1); i++) {
				memcpy(slice.GetLinePtr(i),volume.GetLinePtr(i+origin[1],idx)+origin[0],slice.Size(0)*sizeof(float));
			}

			break;
		case MatrixZXY : 
            pVolume+=origin[0]*sx+origin[1]*sxy;
            if (volume.Size(0)<=idx) {
                msg<<"Slice index greater than matrix size (ZXY matrix). Slice index is "<<idx<<" and Matrix Sixe is: " << volume.Size(0);
                throw ReconException(msg.str(),__FILE__,__LINE__);}
			for (size_t y=0; y<dims[1]; y++) {
				pSlice=slice.GetLinePtr(y);
				for (size_t x=0; x<dims[0]; x++) {
                    pSlice[x]=pVolume[x*sx];
				}
				pVolume+=sxy;
			}
			break;
	}

	return slice;
}

size_t BackProjectorModuleBase::GetNSlices()
{
	switch (MatrixAlignment) {
		case MatrixXYZ : 
			return volume.Size(2);
		case MatrixZXY : 
			return volume.Size(0);
	}

	return volume.Size(2);
}

bool BackProjectorModuleBase::UpdateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) {
		return m_Interactor->SetProgress(val,msg);
	}

	return false;
}

void BackProjectorModuleBase::ClearAll()
{
    mask.clear();
    memset(MatrixDims,0,3*sizeof(size_t));
}

void BackProjectorModuleBase::BuildCircleMask()
{
    size_t nSizeX=0;
    size_t nSizeY=0;
    if (MatrixAlignment==BackProjectorModuleBase::MatrixXYZ) {
        nSizeX=MatrixDims[0];
        nSizeY=MatrixDims[1];
    }
    else {
        nSizeX=MatrixDims[1];
        nSizeY=MatrixDims[2];
    }

    const float matrixCenterX=static_cast<float>(nSizeX>>1);
    const float matrixCenterY=static_cast<float>(nSizeY>>1);
    mask.resize(nSizeY);

    float R=matrixCenterX-1;
    if (mConfig.ProjectionInfo.bCorrectTilt) {
        float slices=0;
        if (mConfig.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
            slices=mConfig.ProjectionInfo.roi[3];
        }
        else {
            slices=mConfig.ProjectionInfo.nDims[1];
        }
        R-=floor(tan(fabs(mConfig.ProjectionInfo.fTiltAngle)*fPi/180.0f)*slices);
    }

    const float R2=R*R;
    for (size_t i=0; i<nSizeY; i++) {
        float y=static_cast<float>(i)-matrixCenterY;
        float y2=y*y;

        if (y2<=R2) {
            float x=sqrt(R2-y2);
            mask[i].first=static_cast<size_t>(ceil(matrixCenterX-x));
            mask[i].second=min(nSizeX-1u,static_cast<size_t>(floor(matrixCenterX+x)));
        }
        else {
            mask[i].first  = 0u;
            mask[i].second = 0u;
        }
        if (mConfig.MatrixInfo.bUseROI) {
            if ((mConfig.MatrixInfo.roi[1]<=i) && (i<=mConfig.MatrixInfo.roi[3])) {
                mask[i].first=max(mask[i].first,mConfig.MatrixInfo.roi[0]);
                mask[i].second=min(mask[i].second,mConfig.MatrixInfo.roi[2]);
            }
            else {
                mask[i].first=0u;
                mask[i].second=0u;
            }
        }

    }

}

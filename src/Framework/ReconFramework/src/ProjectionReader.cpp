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

#include <map>
#include <string>
#include <iostream>
#include <base/timage.h>
#include <io/io_matlab.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <math/median.h>
#include <base/kiplenums.h>
#include <base/trotate.h>
#include <base/imagesamplers.h>
#include "../include/ReconException.h"
#include "../include/ProjectionReader.h"
#include "../include/ReconHelpers.h"

ProjectionReader::ProjectionReader(kipl::interactors::InteractionBase *interactor) :
	logger("ProjectionReader"),
		m_Interactor(interactor)
{

}

ProjectionReader::~ProjectionReader(void)
{

}

void ProjectionReader::GetImageSize(std::string path,
								   std::string filemask,
								   size_t number,
								   float binning,
								   size_t * dims)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');

	return GetImageSize(filename,binning, dims);
}

void ProjectionReader::GetImageSize(std::string filename, float binning, size_t *dims)
{
	std::map<std::string, size_t> extensions;
	extensions[".mat"]=0;
	extensions[".fits"]=1;
	extensions[".fit"]=1;
	extensions[".fts"]=1;
	extensions[".tif"]=2;
	extensions[".tiff"]=2;
	extensions[".TIF"]=2;
	extensions[".png"]=3;

	size_t extpos=filename.find_last_of(".");
	std::stringstream msg;

	try {
		if (extpos!=filename.npos) {
			std::string ext=filename.substr(extpos);
			switch (extensions[ext]) {
			case 0  : kipl::io::GetMATDims(filename.c_str(),dims);  break;
			case 1  : kipl::io::GetFITSDims(filename.c_str(),dims); break;
			case 2  : kipl::io::GetTIFFDims(filename.c_str(),dims);  break;
			//case 3  : return GetImageSizePNG(filename.c_str(),dims);  break;

			default : throw ReconException("Unknown file type",__FILE__, __LINE__); break;
			}
		}
		else {
			throw ReconException("Unknown file type",__FILE__, __LINE__);
		}
	}
	catch (std::exception &e) {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &e) {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
	dims[0]/=binning;
	dims[1]/=binning;
}

void ProjectionReader::UpdateCrop(kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		size_t *dims,
		size_t *nCrop)
{
	if (nCrop!=NULL) {
		switch (flip) {
		case kipl::base::ImageFlipNone : break;
		case kipl::base::ImageFlipHorizontal :
			nCrop[0]=dims[0]-1-nCrop[0];
			nCrop[2]=dims[0]-1-nCrop[2];
			break;
		case kipl::base::ImageFlipVertical :
			nCrop[1]=dims[1]-1-nCrop[1];
			nCrop[3]=dims[1]-1-nCrop[3];
			break;
		case kipl::base::ImageFlipHorizontalVertical :
			nCrop[0]=dims[0]-1-nCrop[0];
			nCrop[1]=dims[1]-1-nCrop[1];
			nCrop[2]=dims[0]-1-nCrop[2];
			nCrop[3]=dims[1]-1-nCrop[3];
			break;
		}

		size_t nCropOrig[4];
		memcpy(nCropOrig,nCrop,4*sizeof(size_t));
		switch (rotate) {
		case kipl::base::ImageRotateNone : break;
		case kipl::base::ImageRotate90   :
			nCrop[0]=nCropOrig[3];
			nCrop[1]=nCropOrig[0];
			nCrop[2]=nCropOrig[1];
			nCrop[3]=nCropOrig[2];

			swap(dims[0],dims[1]);
			break;
		case kipl::base::ImageRotate180  :
			nCrop[0]=dims[0]-1-nCrop[0];
			nCrop[1]=dims[1]-1-nCrop[1];
			nCrop[2]=dims[0]-1-nCrop[2];
			nCrop[3]=dims[1]-1-nCrop[3];

			break;
		case kipl::base::ImageRotate270  :
			nCrop[0]=nCropOrig[1];
			nCrop[1]=nCropOrig[2];
			nCrop[2]=nCropOrig[3];
			nCrop[3]=nCropOrig[0];

			swap(dims[0],dims[1]);
			break;
		}
		if (nCrop[2]<nCrop[0]) swap(nCrop[0],nCrop[2]);
		if (nCrop[3]<nCrop[1]) swap(nCrop[1],nCrop[3]);
	}


}

kipl::base::TImage<float,2> ProjectionReader::RotateProjection(kipl::base::TImage<float,2> img,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate)
{

	kipl::base::TImage<float,2> res;

	kipl::base::TRotate<float> r;

	switch (flip) {
	case kipl::base::ImageFlipNone                : res=img; break;
	case kipl::base::ImageFlipHorizontal          : res=r.MirrorHorizontal(img); break;
	case kipl::base::ImageFlipVertical            : res=r.MirrorVertical(img); break;
	case kipl::base::ImageFlipHorizontalVertical  : res=r.MirrorVertical(r.MirrorHorizontal(img));break;
	}

	switch (rotate) {
	case kipl::base::ImageRotateNone : break;
	case kipl::base::ImageRotate90   : res=r.Rotate90(res);break;
	case kipl::base::ImageRotate180  : res=r.Rotate180(res);break;
	case kipl::base::ImageRotate270  : res=r.Rotate270(res);break;
	}

	return res;
}

kipl::base::TImage<float,2> ProjectionReader::Read(std::string filename,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
		size_t const * const nCrop)
{
//	std::cout<<"Reading: "<<filename<<", "<<flip<<", "<<rotate<<" "<<binning<<std::endl;
	size_t dims[8];
	try {
		GetImageSize(filename, binning,dims);
	}
	catch (ReconException &e) {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &e) {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
	catch (...) {
		throw ReconException("Unhandled exception",__FILE__,__LINE__);
	}
	size_t local_crop[4];
	size_t *pCrop=NULL;
	if (nCrop!=NULL) {

		local_crop[0]=nCrop[0];
		local_crop[1]=nCrop[1];
		local_crop[2]=nCrop[2];
		local_crop[3]=nCrop[3];

		UpdateCrop(flip,rotate,dims,local_crop);
		pCrop=local_crop;

		for (size_t i=0; i<4; i++)
			pCrop[i]*=binning;
	}




	std::map<std::string, size_t> extensions;
	extensions[".mat"]=0;
	extensions[".fits"]=1;
	extensions[".fit"]=1;
	extensions[".fts"]=1;
	extensions[".tif"]=2;
	extensions[".tiff"]=2;
	extensions[".TIF"]=2;
	extensions[".png"]=3;
	size_t extpos=filename.find_last_of(".");
	std::ostringstream msg;
	kipl::base::TImage<float,2> img;
	if (extpos!=filename.npos) {
		std::string ext=filename.substr(extpos);
		try {
			switch (extensions[ext]) {
			case 0  : img=ReadMAT(filename,pCrop);  break;
			case 1  : img=ReadFITS(filename,pCrop); break;
			case 2  : img=ReadTIFF(filename,pCrop);  break;
			case 3  : img=ReadPNG(filename,pCrop);  break;
			//case 4	: return ReadHDF(filename);  break;
			default : throw ReconException("Unknown file type",__FILE__, __LINE__); break;
			}
		}
		catch (ReconException &e) {
			msg<<"Failed to read "<<filename<<" recon exception:\n"<<e.what();
			throw ReconException(msg.str(),__FILE__,__LINE__);
		}
		catch (kipl::base::KiplException &e) {
			msg<<"Failed to read "<<filename<<" kipl exception:\n"<<e.what();
			throw ReconException(msg.str(),__FILE__,__LINE__);
		}
		catch (std::exception &e) {
			msg<<"Failed to read "<<filename<<" STL exception:\n"<<e.what();
			throw ReconException(msg.str(),__FILE__,__LINE__);
		}
	}
	else {
		throw ReconException("Unknown file type",__FILE__, __LINE__); 
	}
    size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};
	kipl::base::TImage<float,2> binned;
	msg.str("");
	msg<<"Failed to resample or rotate the projection with a ";
	try {
		if (1<binning)
			kipl::base::ReBin(img,binned,bins);
		else
			binned=img;

		img=RotateProjection(binned,flip,rotate);
	}
	catch (kipl::base::KiplException &e) {
		msg<<"KiplException: \n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		msg<<"STL exception: \n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (...) {
		msg<<"unknown exception.";
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	return img;
}

kipl::base::TImage<float,2> ProjectionReader::Read(std::string path, 
												   std::string filemask, 
												   size_t number,
												   kipl::base::eImageFlip flip,
												   kipl::base::eImageRotate rotate,
												   float binning,
												   size_t const * const nCrop)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');

	return Read(filename,flip,rotate, binning,nCrop);
}

kipl::base::TImage<float,2> ProjectionReader::ReadMAT(std::string filename, size_t const * const nCrop)
{
	kipl::base::TImage<float,2> img;
	kipl::io::ReadMAT(img,filename.c_str(),nCrop);
	return img;
}

kipl::base::TImage<float,2> ProjectionReader::ReadFITS(std::string filename, size_t const * const nCrop)
{
	kipl::base::TImage<float,2> img;
	try {
		kipl::io::ReadFITS(img,filename.c_str(),nCrop);
	}
	catch (std::exception &e) {
		throw ReconException(e.what(), __FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &e) {
		throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
	}
	catch (...) {
		throw ReconException("Unknown exception", __FILE__,__LINE__);
	}
	
	return img;
}

kipl::base::TImage<float,2> ProjectionReader::ReadTIFF(std::string filename, size_t const * const nCrop)
{
	kipl::base::TImage<float,2> img;
	kipl::io::ReadTIFF(img,filename.c_str(),nCrop);
	
	return img;
}

kipl::base::TImage<float,2> ProjectionReader::ReadPNG(std::string filename, size_t const * const nCrop)
{
	throw ReconException("ReadPNG is not implemented",__FILE__, __LINE__); 
	return kipl::base::TImage<float,2>();
}

float ProjectionReader::GetProjectionDose(std::string filename,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
		size_t const * const nDoseROI)
{
	kipl::base::TImage<float,2> img;

	if (!(nDoseROI[0]*nDoseROI[1]*nDoseROI[2]*nDoseROI[3]))
		return 1.0f;

	img=Read(filename,flip,rotate,binning,nDoseROI);

	float *pImg=img.GetDataPtr();

	float *means=new float[img.Size(1)];
	memset(means,0,img.Size(1)*sizeof(float));

	for (size_t y=0; y<img.Size(1); y++) {
		pImg=img.GetLinePtr(y);
		
		for (size_t x=0; x<img.Size(0); x++) {
			means[y]+=pImg[x];
		}
		means[y]=means[y]/static_cast<float>(img.Size(0));
	}

	float dose; 
	kipl::math::median(means,img.Size(1),&dose);
	delete [] means;
	return dose;
}

float ProjectionReader::GetProjectionDose(std::string path,
		std::string filemask,
		size_t number,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
		size_t const * const nDoseROI)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');

	return GetProjectionDose(filename,flip,rotate,binning,nDoseROI);
}

kipl::base::TImage<float,3> ProjectionReader::Read( ReconConfig config, size_t const * const nCrop,
													std::map<std::string,std::string> &parameters)
{
// todo handle rotations
	kipl::base::TImage<float,2> proj;

	std::map<float, ProjectionInfo> ProjectionList;
	BuildFileList( &config, &ProjectionList);

	size_t dims[3]={nCrop[2]-nCrop[0],nCrop[3]-nCrop[1],ProjectionList.size()};
    dims[1]=config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram ? nCrop[3] : dims[1];
	kipl::base::TImage<float,3> img(dims);
//	size_t roi[4]; memcpy(roi,config.ProjectionInfo.roi,4*sizeof(size_t));
    size_t roi[4]; memcpy(roi,nCrop,4*sizeof(size_t));

	std::ostringstream dose;
	std::ostringstream angle;
	std::ostringstream weight;
	std::map<float, ProjectionInfo>::iterator it,it2;

	float fResolutionWeight=1.0f/(0<config.ProjectionInfo.fResolution[0] ? config.ProjectionInfo.fResolution[0]*0.1f : 1.0f);
	size_t i=0;
	switch (config.ProjectionInfo.imagetype) {
	case ReconConfig::cProjections::ImageType_Projections :
		logger(kipl::logging::Logger::LogMessage,"Using projections");
		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";
			dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					config.ProjectionInfo.dose_roi)<<" ";
        //	proj = Read(it->second.name,config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,config.ProjectionInfo.roi);
            proj = Read(it->second.name,config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,nCrop);

			memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
			i++;
		}
		break;
	case ReconConfig::cProjections::ImageType_Sinograms :
		logger(kipl::logging::Logger::LogMessage,"Using sinograms");
		throw ReconException("Sinograms are not yet supported by ProjectionReader", __FILE__, __LINE__); break;
		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";
			dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					config.ProjectionInfo.dose_roi)<<" ";
			proj = Read(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					roi);
			for (size_t j=0; j<img.Size(1); j++)
				memcpy(img.GetLinePtr(j,i),proj.GetDataPtr(),sizeof(float)*proj.Size(0));
			i++;
		}
		break;
	case ReconConfig::cProjections::ImageType_Proj_RepeatProjection :
		logger(kipl::logging::Logger::LogMessage,"Using repeat projection");
		it2=it=ProjectionList.begin();

		proj = Read(it->second.name,config.ProjectionInfo.eFlip,
				config.ProjectionInfo.eRotate,
				config.ProjectionInfo.fBinning,
				config.ProjectionInfo.roi);

		for (i=0; i<img.Size(2); i++,it2++) {
			memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
			angle  << (it2->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it2->second.weight)*fResolutionWeight << " ";
			dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					config.ProjectionInfo.dose_roi)<<" ";
		}

		break;
	case ReconConfig::cProjections::ImageType_Proj_RepeatSinogram :
		logger(kipl::logging::Logger::LogMessage,"Using repeat sinogram");
		roi[3]=roi[1]+1;

		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";
			dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					config.ProjectionInfo.dose_roi)<<" ";
			proj = Read(it->second.name,config.ProjectionInfo.eFlip,
					config.ProjectionInfo.eRotate,
					config.ProjectionInfo.fBinning,
					roi);
			for (size_t j=0; j<img.Size(1); j++)
				memcpy(img.GetLinePtr(j,i),proj.GetDataPtr(),sizeof(float)*proj.Size(0));
			i++;
		}
		break;
	default :
		throw ReconException("Unknown image type in ProjectionReader", __FILE__, __LINE__); break;
	}

	parameters["weights"]=weight.str();
	parameters["dose"]=dose.str();
	parameters["angles"]=angle.str();
	
//	ofstream dosefile("dose_list");
//	dosefile<<angle.str()<<"\n";
//	dosefile<<dose.str()<<"\n";
//	dosefile<<weight.str()<<"\n";

	return img;
}

bool ProjectionReader::UpdateStatus(float val, std::string msg)
{
	if (m_Interactor!=NULL)
		return m_Interactor->SetProgress(val, msg);;

	return false;
}

bool ProjectionReader::Aborted()
{
	if (m_Interactor!=NULL)
		return m_Interactor->Aborted();

	return false;
}

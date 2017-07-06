//<LICENSE>
#include "stdafx.h"

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <base/timage.h>
#include <base/tsubimage.h>
#include <io/io_matlab.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <io/analyzefileext.h>

#ifdef HAVE_NEXUS
    #include <io/io_nexus.h>
#endif

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
    size_t found = filemask.find("hdf");
    if (found!=std::string::npos) {
        return GetImageSizeNexus(filemask, binning, dims);
    }
    else {
        return GetImageSize(filename,binning, dims);
    }
}

void ProjectionReader::GetImageSizeNexus(string filename, float binning, size_t *dims){
    #ifdef HAVE_NEXUS
        kipl::io::GetNexusDims(filename.c_str(), dims);
        dims[0]/=binning;
        dims[1]/=binning;
    #else
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif


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
//    extensions[".hdf"]=4; // to be implemented
//    extensions[".hd5"]=4;

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
    if (nCrop!=nullptr) {
        bool doRotate=true;
        bool doFlip=true;

        int nCropOrig[4];
        int nTmpCrop[4];
        int nDims[2];

        nDims[0]=static_cast<int>(dims[0]);
        nDims[1]=static_cast<int>(dims[1]);

        nTmpCrop[0]=nCropOrig[0]=nCrop[0];
        nTmpCrop[1]=nCropOrig[1]=nCrop[1];
        nTmpCrop[2]=nCropOrig[2]=nCrop[2];
        nTmpCrop[3]=nCropOrig[3]=nCrop[3];

        switch (flip) {
        case kipl::base::ImageFlipNone : doFlip=false; break;
		case kipl::base::ImageFlipHorizontal :
            nTmpCrop[0]=nDims[0]-nCropOrig[2];
            nTmpCrop[2]=nDims[0]-nCropOrig[0];
			break;
		case kipl::base::ImageFlipVertical :
            nTmpCrop[1]=nDims[1]-nCropOrig[1];
            nTmpCrop[3]=nDims[1]-nCropOrig[3];
			break;
		case kipl::base::ImageFlipHorizontalVertical :
            nTmpCrop[0]=nDims[0]-nCropOrig[0];
            nTmpCrop[1]=nDims[1]-nCropOrig[1];
            nTmpCrop[2]=nDims[0]-nCropOrig[2];
            nTmpCrop[3]=nDims[1]-nCropOrig[3];
			break;
		}

		switch (rotate) {
        case kipl::base::ImageRotateNone :
            doRotate = false;
            break;
		case kipl::base::ImageRotate90   :
            nTmpCrop[0]=nCropOrig[1];
            nTmpCrop[1]=nDims[1]-nCropOrig[2];
            nTmpCrop[2]=nCropOrig[3];
            nTmpCrop[3]=nDims[1]-nCropOrig[0];

            break;
		case kipl::base::ImageRotate180  :
            nTmpCrop[0]=dims[0]-nCropOrig[2];
            nTmpCrop[1]=dims[1]-nCropOrig[3];
            nTmpCrop[2]=dims[0]-nCrop[0];
            nTmpCrop[3]=dims[1]-nCrop[1];

			break;
		case kipl::base::ImageRotate270  :
            nTmpCrop[0]=nDims[0]-nCropOrig[3];
            nTmpCrop[1]=nCropOrig[2];
            nTmpCrop[2]=nDims[0]-nCropOrig[1];
            nTmpCrop[3]=nCropOrig[0];

			swap(dims[0],dims[1]);
			break;
		}

        if (doRotate && doFlip)
            throw ReconException("Combined rotate and flip is currently not supported",__FILE__,__LINE__);


        nCrop[0]=nTmpCrop[0];
        nCrop[1]=nTmpCrop[1];
        nCrop[2]=nTmpCrop[2];
        nCrop[3]=nTmpCrop[3];


	}

    if (nCrop[2]<nCrop[0]) swap(nCrop[0],nCrop[2]);
    if (nCrop[3]<nCrop[1]) swap(nCrop[1],nCrop[3]);
}

kipl::base::TImage<float,2> ProjectionReader::
Read(std::string filename,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
		size_t const * const nCrop)
{
    std::ostringstream msg;

    msg.str(""); msg<<"Reading : "<<filename<<", "<<flip<<", "<<rotate<<" "<<binning;
    logger(logger.LogVerbose,msg.str());

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

    std::fill(local_crop,local_crop+4,0);

    size_t *pCrop=nullptr;

    if (nCrop!=nullptr) {

		local_crop[0]=nCrop[0];
		local_crop[1]=nCrop[1];
		local_crop[2]=nCrop[2];
		local_crop[3]=nCrop[3];

        // PrintCrop("local_crop pre-update",local_crop);
		UpdateCrop(flip,rotate,dims,local_crop);

		pCrop=local_crop;

		for (size_t i=0; i<4; i++)
			pCrop[i]*=binning;
	}

	kipl::base::TImage<float,2> img;

    try {
        kipl::io::eExtensionTypes ext=kipl::io::GetFileExtensionType(filename);
        switch (ext) {
        case kipl::io::ExtensionMAT  : img=ReadMAT(filename,pCrop);   break;
        case kipl::io::ExtensionFITS : img=ReadFITS(filename,pCrop);  break;
        case kipl::io::ExtensionTIFF : img=ReadTIFF(filename,pCrop);  break;
        case kipl::io::ExtensionPNG  : img=ReadPNG(filename,pCrop);   break;
        case kipl::io::ExtensionHDF  : img=ReadHDF(filename);         break; // does not enter in here..
        default : throw ReconException("Unknown file type",__FILE__, __LINE__); break;
        }
    }
    catch (ReconException &e) {
        msg.str("");
        msg<<"Failed to read "<<filename<<" recon exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"Failed to read "<<filename<<" kipl exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Failed to read "<<filename<<" STL exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
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
        kipl::base::TRotate<float> rotator;
        img=rotator.Rotate(binned,flip,rotate);
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


kipl::base::TImage<float,3> ProjectionReader::ReadNexusTomo(string filename){

    kipl::base::TImage<int16_t,3> tmp;

    #ifdef HAVE_NEXUS
        kipl::io::ReadNexus(tmp, filename.c_str());
    #else
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif

    kipl::base::TImage<float,3> img(tmp.Dims());

    float* pImg = img.GetDataPtr();
    int16_t *ptmp = tmp.GetDataPtr();

    for (size_t i=0; i<tmp.Size(0)*tmp.Size(1)*tmp.Size(2); ++i) {
        pImg[i] = static_cast<float>(ptmp[i]);
    }

    return img;


}

int ProjectionReader::GetNexusInfo(string filename, size_t *NofImg, double *ScanAngles){


    #ifdef HAVE_NEXUS
         kipl::io::GetNexusInfo(filename.c_str(), NofImg, ScanAngles);
    #else
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif

    return 1;
}

kipl::base::TImage<float,2> ProjectionReader::GetNexusSlice(kipl::base::TImage<float,3> &NexusTomo,
                                          size_t number,
                                          kipl::base::eImageFlip flip,
                                          kipl::base::eImageRotate rotate,
                                          float binning,
                                          size_t const * const nCrop){
    // should this be in the Nexus io?
    std::ostringstream msg;

    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> cropped;
    kipl::base::TImage<float,2> binned;
    size_t num_slices = NexusTomo.Size(2);
    if (number< num_slices){
        size_t img_size2D[2] = {NexusTomo.Size(0), NexusTomo.Size(1)};
        img.Resize(img_size2D);
        memcpy(img.GetDataPtr(), NexusTomo.GetLinePtr(0, number), sizeof(float)*img_size2D[0]*img_size2D[1]);
    }


    // 1. crop
        if (nCrop!=nullptr) {
//    std::cout<< nCrop[0] << " " << nCrop[1] << " " << nCrop[2] << " " << nCrop[3] << std::endl;

            cropped = kipl::base::TSubImage<float,2>::Get(img, nCrop);}
        else {
            cropped = img;
        }

    size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};

    msg.str("");
    msg<<"Failed to resample or rotate the projection with a ";
    try {
        if (1<binning)
            kipl::base::ReBin(cropped,binned,bins);
        else
            binned=cropped;
        kipl::base::TRotate<float> rotator;
        img=rotator.Rotate(binned,flip,rotate);
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

kipl::base::TImage<float,2> ProjectionReader::ReadNexus(std::string filename,
                                                        size_t number,
                                                        kipl::base::eImageFlip flip,
                                                        kipl::base::eImageRotate rotate,
                                                        float binning,
                                                        size_t const * const nCrop)
{
   std::ostringstream msg;
   kipl::base::TImage<float,2> img;



    #ifdef HAVE_NEXUS


           const char *fname = filename.c_str();

            size_t dims[2];
            try {
                GetImageSizeNexus(filename, binning,dims);
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

            memset(local_crop,0,sizeof(size_t)*4);
            size_t *pCrop=nullptr;
            if (nCrop!=nullptr) {

                local_crop[0]=nCrop[0];
                local_crop[1]=nCrop[1];
                local_crop[2]=nCrop[2];
                local_crop[3]=nCrop[3];

                // PrintCrop("local_crop pre-update",local_crop);
                UpdateCrop(flip,rotate,dims,local_crop);

                pCrop=local_crop;

                for (size_t i=0; i<4; i++)
                    pCrop[i]*=binning;
            }


            kipl::io::ReadNexus(img, fname, number, pCrop);
            kipl::base::TImage<float,2> binned(img.Dims());



            size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};

            msg.str("");
            msg<<"Failed to resample or rotate the projection with a ";
            try {
                if (1<binning)
                    kipl::base::ReBin(img,binned,bins);
                else
                    binned=img;
                kipl::base::TRotate<float> rotator;
                img=rotator.Rotate(binned,flip,rotate);
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

    #else
        std::cout << "HAVE_NEXUS not defined" << std::endl;
//        throw kipl::base::KiplException("Nexus library is not supported",__FILE__,__LINE__);
//        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
        return kipl::base::TImage<float,2>();
    #endif




}

kipl::base::TImage<float, 3> ProjectionReader::ReadNexusStack(std::string filename,
                                      size_t start, size_t end,
                                      kipl::base::eImageFlip flip,
                                      kipl::base::eImageRotate rotate,
                                      float binning,
                                      size_t const * const nCrop)
{
    std::ostringstream msg;
    kipl::base::TImage<float,3> img;


     #ifdef HAVE_NEXUS

            const char *fname = filename.c_str();

             size_t dims[2];
             try {
                 GetImageSizeNexus(filename, binning,dims);
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

             memset(local_crop,0,sizeof(size_t)*4);
             size_t *pCrop=nullptr;
             if (nCrop!=nullptr) {

                 local_crop[0]=nCrop[0];
                 local_crop[1]=nCrop[1];
                 local_crop[2]=nCrop[2];
                 local_crop[3]=nCrop[3];

                 // PrintCrop("local_crop pre-update",local_crop);
                 UpdateCrop(flip,rotate,dims,local_crop);

                 pCrop=local_crop;

                 for (size_t i=0; i<4; i++)
                     pCrop[i]*=binning;
             }

             size_t dim_img[3] = {pCrop[2]-pCrop[0], pCrop[3]-pCrop[1], end-start}; // img size in original coordinate
             img.Resize(dim_img);
             kipl::io::ReadNexusStack(img, fname, start, end, pCrop);

             kipl::base::TImage<float,3> returnimg;
             size_t dims_3D[3] = {nCrop[2]-nCrop[0], nCrop[3]-nCrop[1], end-start}; // img size in rotated and binned coordinate
             returnimg.Resize(dims_3D);


             for (size_t i=0; i<img.Size(2); ++i) {

                 kipl::base::TImage<float,2> slice = kipl::base::ExtractSlice(img, i);
                 kipl::base::TImage<float,2> binned(slice.Dims());

                 size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};

                 msg.str("");
                 msg<<"Failed to resample or rotate the projection with a ";
                 try {
                     if (1<binning) // create a new image with a bin
                         kipl::base::ReBin(slice,binned,bins);
                     else
                         binned=slice;
                     kipl::base::TRotate<float> rotator;
                     slice=rotator.Rotate(binned,flip,rotate);
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

                 kipl::base::InsertSlice(returnimg,slice,i); // to check this!!!!
             }

             return returnimg;

//             return img;

     #else
         std::cout << "HAVE_NEXUS not defined" << std::endl;
           throw kipl::base::KiplException("Nexus library is not supported",__FILE__,__LINE__);
             throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
         return kipl::base::TImage<float,3>();
     #endif
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

    try {
        kipl::io::ReadTIFF(img,filename.c_str(),nCrop);
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

kipl::base::TImage<float,2> ProjectionReader::ReadPNG(std::string filename, size_t const * const nCrop)
{
	throw ReconException("ReadPNG is not implemented",__FILE__, __LINE__); 
	return kipl::base::TImage<float,2>();
}

kipl::base::TImage<float,2> ProjectionReader::ReadHDF(std::string filename, size_t const * const nCrop)
{
    kipl::base::TImage<float,2> img;
    try {
        // todo kipl::io::ReadFITS(img,filename.c_str(),nCrop);
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

float ProjectionReader::GetProjectionDoseNexus(string filename, size_t number,
                                               kipl::base::eImageFlip flip,
                                               kipl::base::eImageRotate rotate,
                                               float binning,
                                               size_t const * const nDoseROI)
{

    kipl::base::TImage<float,2> img;

    if (!(nDoseROI[0]*nDoseROI[1]*nDoseROI[2]*nDoseROI[3]))
        return 1.0f;

    img=ReadNexus(filename,number,flip,rotate,binning,nDoseROI);

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

float * ProjectionReader::GetProjectionDoseListNexus(string filename, size_t start, size_t end,
                                                     kipl::base::eImageFlip flip,
                                                     kipl::base::eImageRotate rotate,
                                                     float binning,
                                                     size_t const * const nDoseROI)
{

    kipl::base::TImage<float,3> img;

    if (!(nDoseROI[0]*nDoseROI[1]*nDoseROI[2]*nDoseROI[3]))
        return NULL; // possibly stupid

    img=ReadNexusStack(filename,start,end,flip,rotate,binning,nDoseROI);

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

    float *doselist = new float[img.Size(2)];
    for (size_t i=0; i<img.Size(2); ++i) {
            kipl::math::median(means,img.Size(1),&doselist[i]);
    }
    delete [] means;
    return doselist;

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
    float dose;

    size_t found = filemask.find("hdf");
    if (found==std::string::npos )
    {   dose = GetProjectionDose(filename,flip,rotate,binning,nDoseROI);}
    else {
        dose = GetProjectionDoseNexus(filemask,number,flip,rotate,binning,nDoseROI);
    }

    return dose;
}

kipl::base::TImage<float,3> ProjectionReader::Read( ReconConfig config, size_t const * const nCrop,
													std::map<std::string,std::string> &parameters)
{
// todo handle rotations
    std::ostringstream msg;
	kipl::base::TImage<float,2> proj;

	std::map<float, ProjectionInfo> ProjectionList;
	BuildFileList( &config, &ProjectionList);


	size_t dims[3]={nCrop[2]-nCrop[0],nCrop[3]-nCrop[1],ProjectionList.size()};

    dims[1]=config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram ? nCrop[3] : dims[1];
	kipl::base::TImage<float,3> img(dims);
//	size_t roi[4]; memcpy(roi,config.ProjectionInfo.roi,4*sizeof(size_t));
    size_t roi[4]; memcpy(roi,nCrop,4*sizeof(size_t));

    msg.str(""); msg<<"ProjectionList="<<ProjectionList.size()<<", dims=["<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<"]";
    logger(logger.LogMessage,msg.str());

	std::ostringstream dose;
	std::ostringstream angle;
	std::ostringstream weight;
	std::map<float, ProjectionInfo>::iterator it,it2;

    kipl::io::eExtensionTypes fileext=kipl::io::GetFileExtensionType(ProjectionList.begin()->second.name);

	float fResolutionWeight=1.0f/(0<config.ProjectionInfo.fResolution[0] ? config.ProjectionInfo.fResolution[0]*0.1f : 1.0f);
	size_t i=0;
	switch (config.ProjectionInfo.imagetype) {
    case ReconConfig::cProjections::ImageType_Projections : {
		logger(kipl::logging::Logger::LogMessage,"Using projections");

        if (fileext!=kipl::io::ExtensionHDF) {
            for (it=ProjectionList.begin();
                 (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections");
                 ++it)
            {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";

            proj = Read(it->second.name,config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,nCrop);

            dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.dose_roi)<<" ";

			memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
            ++i;
            }
        }
        else{
            img = ReadNexusStack(ProjectionList.begin()->second.name, 0, dims[2], config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,nCrop);
            float *doselist = new float[dims[2]];
            doselist = GetProjectionDoseListNexus(ProjectionList.begin()->second.name,
                                                         0, dims[2],
                                                        config.ProjectionInfo.eFlip,
                                                        config.ProjectionInfo.eRotate,
                                                        config.ProjectionInfo.fBinning,
                                                        config.ProjectionInfo.dose_roi);


            for (size_t i=0; i<dims[2]; ++i){
                dose << doselist[i] << " ";
            }
            for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
                angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
                weight << (it->second.weight)*fResolutionWeight << " ";
            }

        }
		break;
    }
    case ReconConfig::cProjections::ImageType_Sinograms : {
		logger(kipl::logging::Logger::LogMessage,"Using sinograms");
		throw ReconException("Sinograms are not yet supported by ProjectionReader", __FILE__, __LINE__); break;
		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";

            if (fileext != kipl::io::ExtensionHDF ) {
                dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";

                proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);}
            else {
                dose   << GetProjectionDoseNexus(it->second.name,i,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";

                proj = ReadNexus(it->second.name, i, config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,roi);

            }
			for (size_t j=0; j<img.Size(1); j++)
				memcpy(img.GetLinePtr(j,i),proj.GetDataPtr(),sizeof(float)*proj.Size(0));
			i++;
		}
		break;
    }
    case ReconConfig::cProjections::ImageType_Proj_RepeatProjection : {
		logger(kipl::logging::Logger::LogMessage,"Using repeat projection");
		it2=it=ProjectionList.begin();



		for (i=0; i<img.Size(2); i++,it2++) {
			memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
			angle  << (it2->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it2->second.weight)*fResolutionWeight << " ";

		}

        if (fileext != kipl::io::ExtensionHDF) {
            dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.dose_roi)<<" ";

            proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.roi);
        }
        else {

            dose   << GetProjectionDoseNexus(it->second.name,i,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.dose_roi)<<" ";

            proj = ReadNexus(it->second.name, i,config.ProjectionInfo.eFlip,
                             config.ProjectionInfo.eRotate,
                             config.ProjectionInfo.fBinning,
                             config.ProjectionInfo.roi);
        }



		break;
    }
    case ReconConfig::cProjections::ImageType_Proj_RepeatSinogram :{
		logger(kipl::logging::Logger::LogMessage,"Using repeat sinogram");
		roi[3]=roi[1]+1;

		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";

            if (fileext != kipl::io::ExtensionHDF) {
                proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);
                dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";
            }
            else {
                proj = ReadNexus(it->second.name,i,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);
                dose   << GetProjectionDoseNexus(it->second.name,i,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";
            }

			for (size_t j=0; j<img.Size(1); j++)
				memcpy(img.GetLinePtr(j,i),proj.GetDataPtr(),sizeof(float)*proj.Size(0));
			i++;
		}
		break;
    }
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
    if (m_Interactor!=nullptr)
		return m_Interactor->SetProgress(val, msg);;

	return false;
}

bool ProjectionReader::Aborted()
{
    if (m_Interactor!=nullptr)
		return m_Interactor->Aborted();

	return false;
}

void ProjectionReader::PrintCrop(std::string name, size_t *crop)
{
    std::cout<<name<<" "<<crop[0]<<", "<<crop[1]<<", "<<crop[2]<<", "<<crop[3]<<std::endl;
}

void ProjectionReader::PrintCrop(std::string name, int *crop)
{
    std::cout<<name<<" "<<crop[0]<<", "<<crop[1]<<", "<<crop[2]<<", "<<crop[3]<<std::endl;
}

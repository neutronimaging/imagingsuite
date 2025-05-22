//<LICENSE>

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <base/timage.h>
#include <base/tsubimage.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <analyzefileext.h>


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

std::vector<size_t> ProjectionReader::GetImageSize(std::string path,
                                                   std::string filemask,
                                                   size_t number,
                                                   float binning)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');

    auto maskext = readers::GetFileExtensionType(filemask);

    if (maskext == readers::ExtensionHDF5)
    {
        try
        {
            return GetImageSizeNexus(filemask, binning);
        }
        catch (kipl::base::KiplException &e)
        {
            throw ReconException(e.what(),__FILE__,__LINE__);
        }
        catch (std::exception &e)
        {
            throw ReconException(e.what(),__FILE__,__LINE__);
        }
    }
    else
    {
        return GetImageSize(filename,binning);
    }
}

std::vector<size_t> ProjectionReader::GetImageSizeNexus(string filename, float binning)
{
    #ifdef HAVE_NEXUS
        std::vector<size_t> dims;
        dims = kipl::io::GetNexusDims(filename);
        dims[0]/=binning;
        dims[1]/=binning;
    #else
        std::ignore = filename;
        std::ignore = binning;
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif

    return {};
}

std::vector<size_t> ProjectionReader::GetImageSize(std::string filename, float binning)
{
	std::stringstream msg;

    std::vector<size_t> dims;
    try
    {
        auto ext = readers::GetFileExtensionType(filename);

        switch (ext)
        {
        case readers::ExtensionFITS  : dims=kipl::io::GetFITSDims(filename);  break;
        case readers::ExtensionTIFF  : dims=kipl::io::GetTIFFDims(filename);  break;
    #ifdef HAVE_NEXUS
        case readers::ExtensionHDF5  : dims=kipl::io::GetNexusDims(filename); break;
    #else
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif
        //case 3  : return GetImageSizePNG(filename.c_str(),dims);  break;

        default : throw ReconException("Unknown file type",__FILE__, __LINE__); break;
        }
	}
    catch (kipl::base::KiplException &e)
    {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
        throw ReconException(e.what(),__FILE__,__LINE__);
    }
	dims[0]/=binning;
	dims[1]/=binning;

    return dims;
}

void ProjectionReader::UpdateCrop(kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
        std::vector<size_t>  &dims,
        std::vector<size_t> &nCrop)
{
    if (!nCrop.empty())
    {
        bool doRotate=true;
        bool doFlip=true;

        std::vector<int> nCropOrig(4,0);
        std::vector<int> nTmpCrop(4,0);
        std::vector<int> nDims(dims.size());
        std::transform(dims.begin(),dims.end(),nDims.begin(),[](size_t x){return static_cast<int>(x);});

        nTmpCrop[0]=nCropOrig[0]=nCrop[0];
        nTmpCrop[1]=nCropOrig[1]=nCrop[1];
        nTmpCrop[2]=nCropOrig[2]=nCrop[2];
        nTmpCrop[3]=nCropOrig[3]=nCrop[3];

        switch (flip) {
        case kipl::base::ImageFlipDefault : doFlip=false; break;
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
        case kipl::base::ImageRotateDefault :
            doRotate = false;
            break;
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

        nCrop = std::vector<size_t>(nTmpCrop.begin(),nTmpCrop.end());
	}

    if (nCrop[2]<nCrop[0]) swap(nCrop[0],nCrop[2]);
    if (nCrop[3]<nCrop[1]) swap(nCrop[1],nCrop[3]);
}

kipl::base::TImage<float,2> ProjectionReader::
Read(std::string filename,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
        const std::vector<size_t> & nCrop)
{
    std::ostringstream msg;

    msg.str(""); msg<<"Reading : "<<filename<<", "<<flip<<", "<<rotate<<" "<<binning;
//    logger(logger.LogVerbose,msg.str());

    std::vector<size_t> dims;
    try
    {
        dims=GetImageSize(filename, binning);
	}
    catch (ReconException &e)
    {
		throw ReconException(e.what(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
        throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
        throw kipl::base::KiplException(e.what(),__FILE__,__LINE__);
    }
    catch (...)
    {
		throw ReconException("Unhandled exception",__FILE__,__LINE__);
	}
    std::vector<size_t> local_crop(4,0UL);

    std::vector<size_t> pCrop;

    if (!nCrop.empty())
    {
        local_crop = nCrop;

		UpdateCrop(flip,rotate,dims,local_crop);

		pCrop=local_crop;

		for (size_t i=0; i<4; i++)
			pCrop[i]*=binning;
	}

	kipl::base::TImage<float,2> img;

    try
    {
        readers::eExtensionTypes ext=readers::GetFileExtensionType(filename);
        switch (ext)
        {
        case readers::ExtensionFITS : img=ReadFITS(filename,pCrop);  break;
        case readers::ExtensionTIFF : img=ReadTIFF(filename,pCrop);  break;
        case readers::ExtensionPNG  : img=ReadPNG(filename,pCrop);   break;
        case readers::ExtensionHDF  : img=ReadHDF(filename);         break; // does not enter in here..
        default : throw ReconException("Unknown file type",__FILE__, __LINE__); break;
        }
    }
    catch (ReconException &e)
    {
        msg.str("");
        msg<<"Failed to read "<<filename<<" recon exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Failed to read "<<filename<<" kipl exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg.str("");
        msg<<"Failed to read "<<filename<<" STL exception:\n"<<e.what();
        logger(logger.LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    vector<size_t> bins={static_cast<size_t>(binning), static_cast<size_t>(binning)};
	kipl::base::TImage<float,2> binned;
	msg.str("");
	msg<<"Failed to resample or rotate the projection with a ";
    try
    {
		if (1<binning)
			kipl::base::ReBin(img,binned,bins);
		else
			binned=img;
        kipl::base::TRotate<float> rotator;
        img=rotator.Rotate(binned,flip,rotate);
	}
    catch (kipl::base::KiplException &e)
    {
		msg<<"KiplException: \n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg<<"STL exception: \n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
		msg<<"unknown exception.";
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	return img;
}


kipl::base::TImage<float,3> ProjectionReader::ReadNexusTomo(string filename)
{

    kipl::base::TImage<int16_t,3> tmp;

    #ifdef HAVE_NEXUS
        kipl::io::ReadNexus(tmp, filename.c_str());
    #else
        std::ignore = filename;
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif

    kipl::base::TImage<float,3> img(tmp.dims());

    float* pImg   = img.GetDataPtr();
    int16_t *ptmp = tmp.GetDataPtr();

    for (size_t i=0; i<tmp.Size(); ++i)
    {
        pImg[i] = static_cast<float>(ptmp[i]);
    }

    return img;


}

int ProjectionReader::GetNexusInfo(string filename, size_t *NofImg, double *ScanAngles)
{
    #ifdef HAVE_NEXUS
    std::ostringstream msg;

    try
    {
         kipl::io::GetNexusInfo(filename.c_str(), NofImg, ScanAngles);
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"KiplException: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg<<"STL exception: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
        msg<<"unknown exception.";
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }


    #else
        std::ignore = filename;
        std::ignore = NofImg;
        std::ignore = ScanAngles;
        throw ReconException("Nexus library is not supported",__FILE__,__LINE__);
    #endif

    return 1;
}

kipl::base::TImage<float,2> ProjectionReader::GetNexusSlice(kipl::base::TImage<float,3> &NexusTomo,
                                          size_t number,
                                          kipl::base::eImageFlip flip,
                                          kipl::base::eImageRotate rotate,
                                          float binning,
                                          const std::vector<size_t> &nCrop)
{
    // should this be in the Nexus io?
    std::ostringstream msg;

    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> cropped;
    kipl::base::TImage<float,2> binned;
    size_t num_slices = NexusTomo.Size(2);
    if (number< num_slices)
    {
        std::vector<size_t> img_size2D = {NexusTomo.Size(0), NexusTomo.Size(1)};
        img.resize(img_size2D);
        std::copy_n(NexusTomo.GetLinePtr(0,number),img.Size(),img.GetDataPtr());
    }


    // 1. crop
    if (! nCrop.empty())
    {
        cropped = kipl::base::TSubImage<float,2>::Get(img, nCrop);
    }
    else
    {
        cropped = img;
    }

    std::vector<size_t> bins={static_cast<size_t>(binning), static_cast<size_t>(binning)};

    msg.str("");
    msg<<"Failed to resample or rotate the projection with a ";
    try
    {
        if (1<binning)
            kipl::base::ReBin(cropped,binned,bins);
        else
            binned=cropped;
        kipl::base::TRotate<float> rotator;
        img=rotator.Rotate(binned,flip,rotate);
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"KiplException: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg<<"STL exception: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
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
                                                        const std::vector<size_t> &nCrop)
{
   std::ostringstream msg;
   kipl::base::TImage<float,2> img;
#ifdef HAVE_NEXUS
            std::vector<size_t> dims;
            try
            {
                dims = GetImageSizeNexus(filename, binning);
            }
            catch (ReconException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (kipl::base::KiplException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (std::exception &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (...) {
                throw ReconException("Unhandled exception",__FILE__,__LINE__);
            }


            std::vector<size_t> local_crop(4,0UL);

            std::vector<size_t>  pCrop;
            if ( !nCrop.empty() )
            {
                local_crop = nCrop;

                // PrintCrop("local_crop pre-update",local_crop);
                UpdateCrop(flip,rotate,dims,local_crop);

                pCrop=local_crop;

                for (size_t i=0; i<4; i++)
                    pCrop[i]*=binning;
            }


            try
            {
                kipl::io::ReadNexus(img, filename, number, pCrop);
            }
            catch (ReconException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (kipl::base::KiplException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (std::exception &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (...)
            {
                throw ReconException("Unhandled exception",__FILE__,__LINE__);
            }

            kipl::base::TImage<float,2> binned(img.dims());

            size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};

            msg.str("");
            msg<<"Failed to resample or rotate the projection with a ";
            try
            {
                if (1<binning)
                    kipl::base::ReBin(img,binned,bins);
                else
                    binned=img;
                kipl::base::TRotate<float> rotator;
                img=rotator.Rotate(binned,flip,rotate);
            }
            catch (kipl::base::KiplException &e)
            {
                msg<<"KiplException: \n"<<e.what();
                logger(kipl::logging::Logger::LogError,msg.str());
                throw ReconException(msg.str(),__FILE__,__LINE__);
            }
            catch (std::exception &e)
            {
                msg<<"STL exception: \n"<<e.what();
                logger(kipl::logging::Logger::LogError,msg.str());
                throw ReconException(msg.str(),__FILE__,__LINE__);
            }
            catch (...)
            {
                msg<<"unknown exception.";
                logger(kipl::logging::Logger::LogError,msg.str());
                throw ReconException(msg.str(),__FILE__,__LINE__);
            }

            return img;

    #else
        std::ignore = filename;
        std::ignore = number;
        std::ignore = flip;
        std::ignore = rotate;
        std::ignore = binning;
        std::ignore = nCrop;
        logger.warning("HAVE_NEXUS not defined");
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
                                      const std::vector<size_t> &nCrop)
{
    std::ostringstream msg;
    kipl::base::TImage<float,3> img;


     #ifdef HAVE_NEXUS
             std::vector<size_t> dims;
             try
             {
                 dims=GetImageSizeNexus(filename, binning);
             }
             catch (ReconException &e)
             {
                 throw ReconException(e.what(),__FILE__,__LINE__);
             }
             catch (kipl::base::KiplException &e)
             {
                 throw ReconException(e.what(),__FILE__,__LINE__);
             }
             catch (std::exception &e)
             {
                 throw ReconException(e.what(),__FILE__,__LINE__);
             }
             catch (...) {
                 throw ReconException("Unhandled exception",__FILE__,__LINE__);
             }


             std::vector<size_t> local_crop(4,0UL);

             std::vector<size_t> pCrop;
             if ( !nCrop.empty())
             {
                local_crop = nCrop;

                UpdateCrop(flip,rotate,dims,local_crop);

                pCrop=local_crop;

                for (size_t i=0; i<4; i++)
                    pCrop[i]*=binning;
             }

             std::vector<size_t> dim_img = { pCrop[2]-pCrop[0],
                                             pCrop[3]-pCrop[1],
                                             end-start}; // img size in original coordinate
             img.resize(dim_img);
             kipl::io::ReadNexusStack(img, filename, start, end, pCrop);

             kipl::base::TImage<float,3> returnimg;
             std::vector<size_t> dims_3D = { nCrop[2]-nCrop[0],
                                             nCrop[3]-nCrop[1],
                                             end-start}; // img size in rotated and binned coordinate
             returnimg.resize(dims_3D);


             for (size_t i=0; i<img.Size(2); ++i)
             {
                 kipl::base::TImage<float,2> slice = kipl::base::ExtractSlice(img, i);
                 kipl::base::TImage<float,2> binned(slice.dims());

                 size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};

                 msg.str("");
                 msg<<"Failed to resample or rotate the projection with a ";
                 try
                 {
                     if (1<binning) // create a new image with a bin
                         kipl::base::ReBin(slice,binned,bins);
                     else
                         binned=slice;
                     kipl::base::TRotate<float> rotator;
                     slice=rotator.Rotate(binned,flip,rotate);
                 }
                 catch (kipl::base::KiplException &e)
                 {
                     msg<<"KiplException: \n"<<e.what();
                     logger(kipl::logging::Logger::LogError,msg.str());
                     throw ReconException(msg.str(),__FILE__,__LINE__);
                 }
                 catch (std::exception &e)
                 {
                     msg<<"STL exception: \n"<<e.what();
                     logger(kipl::logging::Logger::LogError,msg.str());
                     throw ReconException(msg.str(),__FILE__,__LINE__);
                 }
                 catch (...)
                 {
                     msg<<"unknown exception.";
                     logger(kipl::logging::Logger::LogError,msg.str());
                     throw ReconException(msg.str(),__FILE__,__LINE__);
                 }

                 kipl::base::InsertSlice(returnimg,slice,i); // to check this!!!!
             }

             return returnimg;

//             return img;

     #else
        std::ignore = filename;
        std::ignore = start;
        std::ignore = end;
        std::ignore = flip;
        std::ignore = rotate;
        std::ignore = binning;
        std::ignore = nCrop;

         logger.warning("HAVE_NEXUS not defined");
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
                                                   const std::vector<size_t> &nCrop)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');

	return Read(filename,flip,rotate, binning,nCrop);
}

kipl::base::TImage<float,2> ProjectionReader::ReadFITS(std::string filename, const std::vector<size_t> & nCrop)
{
	kipl::base::TImage<float,2> img;
    try
    {
		kipl::io::ReadFITS(img,filename.c_str(),nCrop);
	}
    catch (kipl::base::KiplException &e)
    {
		throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
        throw ReconException(e.what(), __FILE__,__LINE__);
    }
    catch (...)
    {
		throw ReconException("Unknown exception", __FILE__,__LINE__);
	}
	
	return img;
}

kipl::base::TImage<float,2> ProjectionReader::ReadTIFF(const std::string &filename, const std::vector<size_t> &nCrop)
{
	kipl::base::TImage<float,2> img;

    try
    {
        kipl::io::ReadTIFF(img,filename,nCrop,0);
    }
    catch (kipl::base::KiplException &e)
    {
        throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        throw ReconException(e.what(), __FILE__,__LINE__);
    }
    catch (...)
    {
        throw ReconException("Unknown exception", __FILE__,__LINE__);
    }
	
	return img;
}

kipl::base::TImage<float,2> ProjectionReader::ReadPNG(  const std::string & /*filename*/, 
                                                        const std::vector<size_t> & /*nCrop*/)
{
	throw ReconException("ReadPNG is not implemented",__FILE__, __LINE__); 
	return kipl::base::TImage<float,2>();
}

kipl::base::TImage<float,2> ProjectionReader::ReadHDF(  const string & /*filename*/, 
                                                        const std::vector<size_t> & /*nCrop*/)
{
    kipl::base::TImage<float,2> img;
    try
    {
        // todo kipl::io::ReadFITS(img,filename.c_str(),nCrop);
    }
    catch (kipl::base::KiplException &e)
    {
        throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        throw ReconException(e.what(), __FILE__,__LINE__);
    }
    catch (...)
    {
        throw ReconException("Unknown exception", __FILE__,__LINE__);
    }

    return img;
}

float ProjectionReader::GetProjectionDose(std::string filename,
		kipl::base::eImageFlip flip,
		kipl::base::eImageRotate rotate,
		float binning,
        const std::vector<size_t> & nDoseROI)
{
	kipl::base::TImage<float,2> img;

    auto [min0, max0] = std::minmax(nDoseROI[0], nDoseROI[2]);
    auto [min1, max1] = std::minmax(nDoseROI[1], nDoseROI[3]);

    std::vector<size_t> doseROI = {min0,min1,max0,max1}; 
    // logger.message("Dose ROI: ["+std::to_string(doseROI[0])+","
    //             +std::to_string(doseROI[1])+","
    //             +std::to_string(doseROI[2])+","
    //             +std::to_string(doseROI[3])+"]");
    if (doseROI[0]==doseROI[2] || doseROI[1]==doseROI[3])
    {
        logger.warning("Dose ROI is empty, returning 1.0");
		return 1.0f;
    }

	img=Read(filename,flip,rotate,binning,doseROI);

	float *pImg=img.GetDataPtr();

	float *means=new float[img.Size(1)];
	memset(means,0,img.Size(1)*sizeof(float));

    for (size_t y=0; y<img.Size(1); y++)
    {
		pImg=img.GetLinePtr(y);
		
        for (size_t x=0; x<img.Size(0); x++)
        {
			means[y]+=pImg[x];
		}
		means[y]=means[y]/static_cast<float>(img.Size(0));
	}

	float dose; 
	kipl::math::median(means,img.Size(1),&dose);
	delete [] means;
	return dose;
}

float ProjectionReader::GetProjectionDoseNexus(const std::string & filename, size_t number,
                                               kipl::base::eImageFlip flip,
                                               kipl::base::eImageRotate rotate,
                                               float binning,
                                               const std::vector<size_t> & nDoseROI)
{

    kipl::base::TImage<float,2> img;

    auto [min0, max0] = std::minmax(nDoseROI[0], nDoseROI[2]);
    auto [min1, max1] = std::minmax(nDoseROI[1], nDoseROI[3]);

    std::vector<size_t> doseROI = {min0,max1,max0,max1}; 
    
    if (doseROI[0]==doseROI[2] || doseROI[1]==doseROI[3])
    {
        logger.warning("Dose ROI is empty, returning 1.0");
        return 1.0f;
    }

    img=ReadNexus(filename,number,flip,rotate,binning,doseROI);

    float *pImg=img.GetDataPtr();

    float *means=new float[img.Size(1)];
    memset(means,0,img.Size(1)*sizeof(float));

    for (size_t y=0; y<img.Size(1); y++)
    {
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

std::vector<float> ProjectionReader::GetProjectionDoseListNexus(const std::string &filename, size_t start, size_t end,
                                                     kipl::base::eImageFlip flip,
                                                     kipl::base::eImageRotate rotate,
                                                     float binning,
                                                     const std::vector<size_t> &nDoseROI)
{

    kipl::base::TImage<float,3> img;

    auto [min0, max0] = std::minmax(nDoseROI[0], nDoseROI[2]);
    auto [min1, max1] = std::minmax(nDoseROI[1], nDoseROI[3]);

    std::vector<size_t> doseROI = {min0,max1,max0,max1}; 
    
    if (doseROI[0]==doseROI[2] || doseROI[1]==doseROI[3])
    {
        logger.warning("Dose ROI is empty, returning 1.0");
        return {};
    }
        

    img=ReadNexusStack(filename,start,end,flip,rotate,binning,doseROI);

    float *pImg=img.GetDataPtr();

    std::vector<float> doselist(img.Size(2),0.0f);

    for (size_t z=0; z<img.Size(2); ++z)
    {
        std::vector<float> means(img.Size(1),0.0f);

        for (size_t y=0; y<img.Size(1); y++)
        {
            pImg=img.GetLinePtr(y,z);

            for (size_t x=0; x<img.Size(0); x++)
            {
                means[y]+=pImg[x];
            }

            means[y]=means[y]/static_cast<float>(img.Size(0));
        }

        kipl::math::median(means,&doselist[z]);
    }

    return doselist;
}

float ProjectionReader::GetProjectionDose(const std::string &path,
        const std::string &filemask,
        size_t number,
        kipl::base::eImageFlip flip,
        kipl::base::eImageRotate rotate,
        float binning,
        const std::vector<size_t> & nDoseROI)
{
	std::string filename;
	std::string ext;
	kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');
    float dose;

    auto maskext = readers::GetFileExtensionType(filemask);

    if (maskext != readers::ExtensionHDF5)
    {
        dose = GetProjectionDose(filename,flip,rotate,binning,nDoseROI);
    }
    else
    {
        dose = GetProjectionDoseNexus(filemask,number,flip,rotate,binning,nDoseROI);
    }

    return dose;
}

kipl::base::TImage<float,3> ProjectionReader::Read( ReconConfig config, const std::vector<size_t> & nCrop,
													std::map<std::string,std::string> &parameters)
{
// todo handle rotations
    std::ostringstream msg;
	kipl::base::TImage<float,2> proj;

	std::map<float, ProjectionInfo> ProjectionList;
	BuildFileList( config, ProjectionList);
    msg.str(""); msg<<config.WriteXML();
    logger.message(msg.str());


    std::vector<size_t> dims = { nCrop[2]-nCrop[0],
                                 nCrop[3]-nCrop[1],
                                 ProjectionList.size()};

    dims[1]=config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram ? nCrop[3] : dims[1];

	kipl::base::TImage<float,3> img(dims);

    auto roi = nCrop;

    msg.str(""); msg<<"ProjectionList="<<ProjectionList.size()<<", dims=["<<dims[0]<<", "<<dims[1]<<", "<<dims[2]<<"]";
    logger(logger.LogMessage,msg.str());

	std::ostringstream dose;
	std::ostringstream angle;
	std::ostringstream weight;
	std::map<float, ProjectionInfo>::iterator it,it2;

    auto fileext=readers::GetFileExtensionType(ProjectionList.begin()->second.name);

	float fResolutionWeight=1.0f/(0<config.ProjectionInfo.fResolution[0] ? config.ProjectionInfo.fResolution[0]*0.1f : 1.0f);
	size_t i=0;
    switch (config.ProjectionInfo.imagetype)
    {
    case ReconConfig::cProjections::ImageType_Projections :
    {
		logger(kipl::logging::Logger::LogMessage,"Using projections");

        if (fileext!=readers::ExtensionHDF5)
        {
            for (it=ProjectionList.begin();
                 (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections");
                 ++it)
            {
                angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
                weight << (it->second.weight)*fResolutionWeight << " ";

                proj = Read(it->second.name,
                            config.ProjectionInfo.eFlip,
                            config.ProjectionInfo.eRotate,
                            config.ProjectionInfo.fBinning,
                            nCrop);

                dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";

                memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
                ++i;
            }
        }
        else
        {
            try
            {
                img = ReadNexusStack(ProjectionList.begin()->second.name, 0, dims[2], config.ProjectionInfo.eFlip,config.ProjectionInfo.eRotate,config.ProjectionInfo.fBinning,nCrop);
            }
            catch (ReconException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (kipl::base::KiplException &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (std::exception &e)
            {
                throw ReconException(e.what(),__FILE__,__LINE__);
            }
            catch (...)
            {
                throw ReconException("Unhandled exception",__FILE__,__LINE__);
            }

            auto doselist = GetProjectionDoseListNexus(ProjectionList.begin()->second.name,
                                                         0, dims[2],
                                                        config.ProjectionInfo.eFlip,
                                                        config.ProjectionInfo.eRotate,
                                                        config.ProjectionInfo.fBinning,
                                                        config.ProjectionInfo.dose_roi);


            for (size_t j=0; j<dims[2]; ++j)
            {
                dose << doselist[j] << " ";
            }

            for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++)
            {
                angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
                weight << (it->second.weight)*fResolutionWeight << " ";
            }

        }
		break;
    }
    case ReconConfig::cProjections::ImageType_Sinograms :
    {
		logger(kipl::logging::Logger::LogMessage,"Using sinograms");
		throw ReconException("Sinograms are not yet supported by ProjectionReader", __FILE__, __LINE__); break;
        for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++)
        {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";

            if (fileext != readers::ExtensionHDF5 )
            {
                dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";

                proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);
            }
            else
            {
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
    case ReconConfig::cProjections::ImageType_Proj_RepeatProjection :
    {
		logger(kipl::logging::Logger::LogMessage,"Using repeat projection");
		it2=it=ProjectionList.begin();

        for (i=0; i<img.Size(2); i++,it2++)
        {
			memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
			angle  << (it2->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it2->second.weight)*fResolutionWeight << " ";

		}

        if (fileext != readers::ExtensionHDF5)
        {
            dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.dose_roi)<<" ";

            proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                    config.ProjectionInfo.eRotate,
                    config.ProjectionInfo.fBinning,
                    config.ProjectionInfo.roi);
        }
        else
        {

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
    case ReconConfig::cProjections::ImageType_Proj_RepeatSinogram :
    {
		logger(kipl::logging::Logger::LogMessage,"Using repeat sinogram");
		roi[3]=roi[1]+1;

		for (it=ProjectionList.begin(); (it!=ProjectionList.end()) && !UpdateStatus(static_cast<float>(i)/ProjectionList.size(),"Reading projections"); it++) {
			angle  << (it->second.angle)+config.MatrixInfo.fRotation  << " ";
			weight << (it->second.weight)*fResolutionWeight << " ";

            if (fileext != readers::ExtensionHDF5)
            {
                proj = Read(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        roi);
                dose   << GetProjectionDose(it->second.name,config.ProjectionInfo.eFlip,
                        config.ProjectionInfo.eRotate,
                        config.ProjectionInfo.fBinning,
                        config.ProjectionInfo.dose_roi)<<" ";
            }
            else
            {
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
        throw ReconException("Unknown image type in ProjectionReader", __FILE__, __LINE__);
	}

    parameters["weights"] = weight.str();
    parameters["dose"]    = dose.str();
    parameters["angles"]  = angle.str();

	return img;
}

bool ProjectionReader::UpdateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) return m_Interactor->SetProgress(val, msg);

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

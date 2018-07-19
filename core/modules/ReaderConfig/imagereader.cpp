//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2015-09-05 16:05:45 +0200 (Sat, 05 Sep 2015) $
// $Rev: 1235 $
// $Id: ImageReader.cpp 1235 2015-09-05 14:05:45Z kaestner $
//
#include "readerconfig_global.h"

#include <map>
#include <string>
#include <iostream>
#include <algorithm>

#include <base/timage.h>
#include <io/io_matlab.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <io/io_vivaseq.h>
#include <io/io_png.h>
#ifdef HAVE_NEXUS
    #include <io/io_nexus.h>
#endif
#include <math/median.h>
#include <base/kiplenums.h>
#include <base/trotate.h>
#include <base/imagesamplers.h>

#include "readerconfig.h"
#include "buildfilelist.h"
#include "imagereader.h"
#include "analyzefileext.h"
#include "readerexception.h"

ImageReader::ImageReader() :
    logger("ImageReader")
{

}

ImageReader::~ImageReader(void)
{

}

int ImageReader::GetImageSize(std::string path,
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

int ImageReader::GetImageSize(std::string filename, float binning, size_t *dims)
{
    readers::eExtensionTypes ext=readers::GetFileExtensionType(filename);

    std::stringstream msg;
    int nDims=0;
    try {

        switch (ext) {
            case readers::ExtensionMAT   : nDims = kipl::io::GetMATDims(filename.c_str(),dims);  break;
            case readers::ExtensionFITS  : nDims = kipl::io::GetFITSDims(filename.c_str(),dims); break;
            case readers::ExtensionTIFF  : nDims = kipl::io::GetTIFFDims(filename.c_str(),dims);  break;
            case readers::ExtensionPNG   : nDims = kipl::io::GetPNGDims(filename.c_str(),dims);  break;
            case readers::ExtensionSEQ   : nDims = kipl::io::GetViVaSEQDims(filename,dims); break;
#ifdef HAVE_NEXUS
            case readers::ExtensionHDF5  : nDims = kipl::io::GetNexusDims(filename.c_str(),dims); break;
#endif

            default : throw ReaderException("Unknown file type",__FILE__, __LINE__); break;
        }

    }
    catch (std::exception &e) {
        throw ReaderException(e.what(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        throw ReaderException(e.what(),__FILE__,__LINE__);
    }
    dims[0]/=binning;
    dims[1]/=binning;

    return nDims;
}

void ImageReader::UpdateCrop(kipl::base::eImageFlip flip,
        kipl::base::eImageRotate rotate,
        size_t *dims,
        size_t *nCrop)
{
    if (nCrop!=nullptr) {
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

kipl::base::TImage<float,2> ImageReader::RotateProjection(kipl::base::TImage<float,2> img,
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

kipl::base::TImage<float,2> ImageReader::Read(std::string filename,
        kipl::base::eImageFlip flip,
        kipl::base::eImageRotate rotate,
        float binning,
        size_t const * const nCrop, size_t idx)
{
    size_t dims[8];
    int nDims=0;
    try {
        nDims=GetImageSize(filename, binning,dims);
    }
    catch (ReaderException &e) {
        throw ReaderException(e.what(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
        throw ReaderException(e.what(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        throw ReaderException(e.what(),__FILE__,__LINE__);
    }
    catch (...) {
        throw ReaderException("Unhandled exception",__FILE__,__LINE__);
    }
    size_t local_crop[4];
    size_t *pCrop=nullptr;
    if (nCrop!=nullptr) {

        local_crop[0]=nCrop[0];
        local_crop[1]=nCrop[1];
        local_crop[2]=nCrop[2];
        local_crop[3]=nCrop[3];

        UpdateCrop(flip,rotate,dims,local_crop);
        pCrop=local_crop;

        for (size_t i=0; i<4; i++)
            pCrop[i]*=binning;
    }
    std::ostringstream msg;
    kipl::base::TImage<float,2> img;
    try {
        readers::eExtensionTypes ext = readers::GetFileExtensionType(filename);
        switch (ext) {
        case readers::ExtensionMAT  : img=ReadMAT(filename,pCrop);  break;
        case readers::ExtensionFITS : img=ReadFITS(filename,pCrop,idx); break;
        case readers::ExtensionTIFF : img=ReadTIFF(filename,pCrop,idx);  break;
        case readers::ExtensionPNG  : img=ReadPNG(filename,pCrop,idx);  break;
        case readers::ExtensionHDF5	: img=ReadHDF(filename,pCrop,idx);  break;
        case readers::ExtensionSEQ	: img=ReadSEQ(filename,pCrop,idx);  break;
        default : throw ReaderException("Unknown file type",__FILE__, __LINE__); break;
        }
    }
    catch (ReaderException &e) {
        msg<<"Failed to read "<<filename<<" recon exception:\n"<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Failed to read "<<filename<<" kipl exception:\n"<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
        msg<<"Failed to read "<<filename<<" STL exception:\n"<<e.what();
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }

    size_t bins[2]={static_cast<size_t>(binning), static_cast<size_t>(binning)};
    kipl::base::TImage<float,2> binned;
    msg.str("");
    msg<<"Failed to resample or rotate the projection with a ";
    try {
//        if (1<binning)
//            kipl::base::ReBin(img,binned,bins);
//        else
            binned=img;

        img=RotateProjection(binned,flip,rotate);
    }
    catch (kipl::base::KiplException &e) {
        msg<<"KiplException: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e) {
        msg<<"STL exception: \n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }
    catch (...) {
        msg<<"unknown exception.";
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReaderException(msg.str(),__FILE__,__LINE__);
    }

    return img;
}

kipl::base::TImage<float,2> ImageReader::Read(std::string path,
                                                   std::string filemask,
                                                   size_t number,
                                                   kipl::base::eImageFlip flip,
                                                   kipl::base::eImageRotate rotate,
                                                   float binning,
                                                   size_t const * const nCrop)
{
    kipl::base::TImage<float,2> img;

    if (filemask.find('#')==std::string::npos)
    { // Reading from single file

    }
    else {
        std::string filename;
        std::string ext;
        kipl::strings::filenames::MakeFileName(path+filemask,number,filename,ext,'#','0');
        img=Read(filename,flip,rotate, binning,nCrop);
    }

    return img;
}

kipl::base::TImage<float,3> ImageReader::Read(ImageLoader &loader,
                                  kipl::base::eImageFlip flip,
                                  kipl::base::eImageRotate rotate,
                                  float binning,
                                  size_t const * const nCrop)
{
    return Read(loader.m_sFilemask,
                loader.m_nFirst,
                loader.m_nLast,
                loader.m_nStep,
                flip,
                rotate,
                binning,
                nCrop);
}

kipl::base::TImage<float,3> ImageReader::Read(string fname,
                                              size_t first,
                                              size_t last,
                                              size_t step,
                                              kipl::base::eImageFlip flip,
                                              kipl::base::eImageRotate rotate,
                                              float binning,
                                              size_t const * const nCrop)
{

    kipl::base::TImage<float,3> img;
    size_t dims[8];
    size_t imgdims[8];

    if (fname.find('#')==std::string::npos)
    { // Reading from single file


        img.Resize(imgdims);
    }
    else
    {
        std::string filename;
        std::string ext;
        kipl::base::TImage<float,2> tmpimg;

        for (size_t i=first; i<=last; i+=step)
        {
            kipl::strings::filenames::MakeFileName(fname,i,filename,ext,'#','0');

            tmpimg=Read(filename,flip,rotate,binning,nCrop);

            if (i==first) {
                dims[0]=tmpimg.Size(0);
                dims[1]=tmpimg.Size(1);
                dims[2]=last-first+1;

                img.Resize(dims);
            }
            float *pImg=tmpimg.GetDataPtr();
            std::copy(pImg,pImg+tmpimg.Size(),img.GetLinePtr(0,(i-first)/step));
        }
    }
    return img;
}


kipl::base::TImage<float,2> ImageReader::ReadMAT(std::string filename, size_t const * const nCrop)
{
    kipl::base::TImage<float,2> img;
    kipl::io::ReadMAT(img,filename.c_str(),nCrop);
    return img;
}

kipl::base::TImage<float,2> ImageReader::ReadFITS(std::string filename, size_t const * const nCrop,size_t idx)
{
    kipl::base::TImage<float,2> img;
    try {
        kipl::io::ReadFITS(img,filename.c_str(),nCrop,idx);
    }
    catch (std::exception &e) {
        throw ReaderException(e.what(), __FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
    }
    catch (...) {
        throw ReaderException("Unknown exception", __FILE__,__LINE__);
    }

    return img;
}

kipl::base::TImage<float,2> ImageReader::ReadTIFF(std::string filename, size_t const * const nCrop, size_t idx)
{
    kipl::base::TImage<float,2> img;
    try {
        kipl::io::ReadTIFF(img,filename.c_str(), nCrop, idx);
    }
    catch (std::exception &e) {
        throw ReaderException(e.what(), __FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e) {
        throw kipl::base::KiplException(e.what(), __FILE__,__LINE__);
    }
    catch (...) {
        throw ReaderException("Unknown exception", __FILE__,__LINE__);
    }

    return img;
}

kipl::base::TImage<float,2> ImageReader::ReadHDF(std::string filename, size_t const * const nCrop, size_t idx)
{
    kipl::base::TImage<float,2> img;

    return img;
}

kipl::base::TImage<float,2> ImageReader::ReadSEQ(std::string filename, size_t const * const nCrop, size_t idx)
{
    kipl::base::TImage<float,2> img;
    kipl::io::ReadViVaSEQ(filename, img, idx, nCrop);

    return img;
}

kipl::base::TImage<float,2> ImageReader::ReadPNG(std::string filename, size_t const * const nCrop, size_t idx)
{
    throw ReaderException("ReadPNG is not implemented",__FILE__, __LINE__);
    return kipl::base::TImage<float,2>();
}

float ImageReader::GetProjectionDose(std::string filename,
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

float ImageReader::GetProjectionDose(std::string path,
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



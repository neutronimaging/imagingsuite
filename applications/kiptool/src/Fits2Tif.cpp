//<LICENSE>
#include "stdafx.h"

#include <map>
#include "Fits2Tif.h"
#include <base/timage.h>
#include <base/trotate.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_generic.h>
#include <io/analyzefileext.h>
#include <strings/filenames.h>
#include <MorphSpotClean.h>


Fits2Tif::Fits2Tif() : logger("Fits2Tif")
{
	// TODO Auto-generated constructor stub

}

Fits2Tif::~Fits2Tif() {
	// TODO Auto-generated destructor stub
}

int Fits2Tif::process(ImagingToolConfig::FileConversionConfig &config)
{
	std::ostringstream msg;
	std::list<std::string> filelist;
    BuildFileList(config,filelist,config.bReverseIdx);


	if (filelist.empty()) {
		logger(kipl::logging::Logger::LogWarning,"Empty file list");
		return -1;
	}

    std::list<kipl::base::TImage<float,2> > imglist;

	std::string dstname,dstmask,ext;
	kipl::strings::filenames::CheckPathSlashes(config.sDestPath,true);
	dstmask=config.sDestPath+config.sDestMask;

    logger(kipl::logging::Logger::LogMessage,"Converting files to tiff");

	std::list<std::string>::iterator it;
	int i=config.nFirstDest;
    for (it=filelist.begin(); it!=filelist.end(); it++) {
        imglist.clear();

        GetImage(imglist,*it,config);
        std::list<kipl::base::TImage<float,2> >::iterator imgit;

        for (imgit=imglist.begin(); imgit!=imglist.end(); imgit++,i++) {
            kipl::strings::filenames::MakeFileName(dstmask,i,dstname,ext,'#','0');
            msg.str("");
            msg<<"Converting "<<*it<<" -> "<<dstname;
            logger(kipl::logging::Logger::LogMessage,msg.str());
            kipl::io::WriteTIFF(*imgit,dstname.c_str(),0.0f,65535.0f);
        }
	}

	return 0;
}

int Fits2Tif::GetImage(std::list<kipl::base::TImage<float,2> > &imglist, std::string fname, ImagingToolConfig::FileConversionConfig &config)
{
    imglist.clear();
    std::list<kipl::base::TImage<unsigned short,2> > simglist;
    std::ostringstream msg;
    std::string fm=config.sSourceMask;
    kipl::io::eExtensionTypes exttype=kipl::io::GetFileExtensionType(fm);
    kipl::base::TImage<unsigned short> simg;
    kipl::base::TRotate<float> rotate;
    size_t *thecrop=config.bCrop ? config.nCrop : NULL;
    std::vector<size_t> crop = {thecrop[0], thecrop[1], thecrop[2], thecrop[3]};

//    size_t *crop=config.bCrop ? config.nCrop : NULL;

    try {
        switch (exttype) {
        case kipl::io::ExtensionDMP :
        case kipl::io::ExtensionDAT :

        case kipl::io::ExtensionRAW : kipl::io::ReadGeneric(simglist,fname.c_str(),
                                                                config.nImgSizeX,config.nImgSizeY,
                                                                config.nReadOffset,config.nStride,
                                                                config.nImagesPerFile,
                                                                config.datatype,config.endian,crop);
                break;
        case kipl::io::ExtensionFITS: kipl::io::ReadFITS(simg,fname.c_str(),crop); simglist.push_back(simg); break;
        case kipl::io::ExtensionTIFF: kipl::io::ReadTIFF(simg,fname.c_str(),*thecrop); simglist.push_back(simg); break;
        case kipl::io::ExtensionTXT :
        case kipl::io::ExtensionXML :
        case kipl::io::ExtensionPNG :
//        case kipl::io::ExtensionMAT :
        case kipl::io::ExtensionHDF :
        default : throw kipl::base::KiplException("The chosen file type is not implemented",__FILE__,__LINE__);
        }
    }
    catch (kipl::base::KiplException &e) {
        msg.str("");
        msg<<"At line "<<__LINE__<<" Failed to open :"<<fname<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        return -1;
    }
    catch (std::exception &e) {
        msg.str("");
        msg<<"Failed to open :"<<fname<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());

        return -1;
    }

    msg.str("");
    msg<<"GetImage got "<<simglist.size()<<" images from "<<fname;
    logger(kipl::logging::Logger::LogMessage,msg.str());
    kipl::base::TImage<float,2> src, dst, dst2;

    std::list<kipl::base::TImage<unsigned short,2> >::iterator it;

    for (it=simglist.begin(); it!=simglist.end(); it++) {
        simg=*it;
        if (config.bReplaceZeros==true) {
            short *pSImg=reinterpret_cast<short *>(simg.GetDataPtr());
            float *pImg=src.GetDataPtr();
            for (size_t i=0; i<simg.Size(); i++) {
                pImg[i]=static_cast<float>(pSImg[i]);
            }
        }
        else {
            unsigned short *pSImg=simg.GetDataPtr();
            float *pImg=src.GetDataPtr();
            for (size_t i=0; i<simg.Size(); i++) {
                pImg[i]=static_cast<float>(pSImg[i]);
            }
        }

        //dst.FreeImage();
        switch (config.flip) {
        case kipl::base::ImageFlipNone       :
            dst=src; break;
        case kipl::base::ImageFlipHorizontal :
            dst=rotate.MirrorHorizontal(src); break;
        case kipl::base::ImageFlipVertical   :
            dst=rotate.MirrorVertical(src); break;
        case kipl::base::ImageFlipHorizontalVertical :
            dst2=rotate.MirrorVertical(src);
            dst=rotate.MirrorHorizontal(dst2); break;
        }

        switch (config.rotate) {
        case kipl::base::ImageRotateNone :
            dst2=dst;
            break;
        case kipl::base::ImageRotate90   :
            dst2=rotate.Rotate90(dst); break;
        case kipl::base::ImageRotate180  :
            dst2=rotate.Rotate180(dst); break;
        case kipl::base::ImageRotate270  :
            dst2=rotate.Rotate270(dst); break;
        }

        if (config.bUseSpotClean) {
            ImagingAlgorithms::MorphSpotClean cleaner;
            cleaner.setConnectivity(kipl::base::conn4);
            cleaner.setCleanMethod(ImagingAlgorithms::MorphDetectPeaks,ImagingAlgorithms::MorphCleanReplace);
            cleaner.process(dst2,config.fSpotThreshold,config.fSpotSigma);
        }
        imglist.push_back(dst2);
    }

    return 0;
}

std::string Fits2Tif::BuildFileList(ImagingToolConfig::FileConversionConfig &config, std::list<std::string> &filelist, bool bReversedIndex)
{
	std::ostringstream msg;
	std::string srcname,srcmask,ext;
    kipl::strings::filenames::CheckPathSlashes(config.sSourceMask,false);
    srcmask=config.sSourceMask;

	filelist.clear();

    if (config.bSortGoldenScan) {
        logger(kipl::logging::Logger::LogMessage,"Building file list for a golden scan");
        std::map<float,std::string> tmplist;
      //  float fScanArc = config.nGoldenScanArc == 0 ? 180.0f : 360.0f;
          float fScanArc = 180.0f;
          msg.str("");
          msg<<"Using arc "<<fScanArc<<" first="<<config.nFirstSrc<<", last="<<config.nLastSrc;
          logger(kipl::logging::Logger::LogMessage,msg.str());
        const float fGoldenSection=0.5f*(1.0f+sqrt(5.0f));

        int cnt=0;
        for (int i=config.nFirstSrc; i<=config.nLastSrc; i++) {
           // if (config.skip_list.find(i)==config.skip_list.end()) {
                kipl::strings::filenames::MakeFileName(srcmask,i,srcname,ext,'#','0',bReversedIndex);

                float angle=fmod(cnt*fScanArc*fGoldenSection,180.0f);
                tmplist.insert(std::make_pair(angle,srcname));

                std::cout<<"cnt="<<cnt<<", i="<<i<<", size(tmplist)="<<tmplist.size()<<", angle="<<angle<<std::endl;
                cnt++;
        }

        std::map<float,std::string>::iterator it;
        for (it=tmplist.begin(); it!=tmplist.end(); it++)
            filelist.push_back(it->second);

        msg.str("");
        msg<<"tmp size="<<tmplist.size()<<", flist size="<<filelist.size();
        logger(kipl::logging::Logger::LogMessage,msg.str());

    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Building file list");
        for (int i=config.nFirstSrc; i<=config.nLastSrc; i++) {
            if (config.skip_list.find(i)==config.skip_list.end()) {
                kipl::strings::filenames::MakeFileName(srcmask,i,srcname,ext,'#','0');
                filelist.push_back(srcname);
            }
            else {
                msg.str("");
                msg<<"Skipping index "<<i;
                logger(kipl::logging::Logger::LogMessage,msg.str());
            }
        }
    }

    return ext;
}

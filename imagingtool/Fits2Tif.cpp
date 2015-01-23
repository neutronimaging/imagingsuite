//
// This file is part of the MuhRec reconstruction application by Anders Kaestner
// (c) 2010 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2010-09-05 15:55:11 +0200 (So, 05 Sep 2010) $
// $Rev: 694 $
//
#include "stdafx.h"

#include <map>
#include "Fits2Tif.h"
#include <base/timage.h>
#include <base/trotate.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
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
	BuildFileList(config,filelist);

	if (filelist.empty()) {
		logger(kipl::logging::Logger::LogWarning,"Empty file list");
		return -1;
	}

    size_t *crop=config.bCrop ? config.nCrop : NULL;

	kipl::base::TImage<float,2> src,dst;
    kipl::base::TImage<short,2> simg;

	std::string dstname,dstmask,ext;
	kipl::strings::filenames::CheckPathSlashes(config.sDestPath,true);
	dstmask=config.sDestPath+config.sDestMask;

    logger(kipl::logging::Logger::LogMessage,"Converting files to tiff");

	kipl::base::TRotate<float> rotate;
	std::list<std::string>::iterator it;
	int i=config.nFirstDest;
	for (it=filelist.begin(); it!=filelist.end(); it++, i++) {
        try {
            kipl::io::ReadFITS(simg,it->c_str(),crop);
        }
        catch (kipl::base::KiplException &e) {
            msg.str("");
            msg<<"At line "<<__LINE__<<" Failed to open :"<<*it<<"\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            return -1;
        }
        catch (std::exception &e) {
            msg.str("");
            msg<<"Failed to open :"<<*it<<"\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());

            return -1;
        }

        if (it==filelist.begin())
            src.Resize(simg.Dims());

        if (config.bReplaceZeros==true) {
            unsigned short *pSImg=reinterpret_cast<unsigned short *>(simg.GetDataPtr());
            float *pImg=src.GetDataPtr();
            for (size_t i=0; i<simg.Size(); i++) {
                pImg[i]=static_cast<float>(pSImg[i]);
            }
        }


        kipl::base::TImage<float,2> dst2;
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
            cleaner.setConnectivity(kipl::morphology::conn4);
            cleaner.setCleanMethod(ImagingAlgorithms::MorphCleanPeaks);
            cleaner.Process(dst2,config.fSpotThreshold);
        }

		kipl::strings::filenames::MakeFileName(dstmask,i,dstname,ext,'#','0');
		msg.str("");
		msg<<"Converting "<<*it<<" -> "<<dstname;
		logger(kipl::logging::Logger::LogMessage,msg.str());
        kipl::io::WriteTIFF(dst2,dstname.c_str(),0.0f, 65535.0f );
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
        for (size_t i=config.nFirstSrc; i<=config.nLastSrc; i++) {
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
        for (size_t i=config.nFirstSrc; i<=config.nLastSrc; i++) {
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

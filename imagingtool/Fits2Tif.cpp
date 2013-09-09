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
#include "Fits2Tif.h"
#include <base/timage.h>
#include <base/trotate.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>

Fits2Tif::Fits2Tif() : logger("Fits2Tif")
{
	// TODO Auto-generated constructor stub

}

Fits2Tif::~Fits2Tif() {
	// TODO Auto-generated destructor stub
}

int Fits2Tif::process(ImagingToolConfig::Fits2TifConfig &config)
{
	std::ostringstream msg;
	std::list<std::string> filelist;
	BuildFileList(config,filelist);

	if (filelist.empty()) {
		logger(kipl::logging::Logger::LogWarning,"Empty file list");
		return -1;
	}

	size_t *crop=NULL;
	kipl::base::TImage<float,2> src,dst;
	std::string dstname,dstmask,ext;
	kipl::strings::filenames::CheckPathSlashes(config.sDestPath,true);
	dstmask=config.sDestPath+config.sDestMask;

	logger(kipl::logging::Logger::LogMessage,"Converting fits to tiff");

	kipl::base::TRotate<float> rotate;
	std::list<std::string>::iterator it;
	int i=config.nFirstDest;
	for (it=filelist.begin(); it!=filelist.end(); it++, i++) {
		try {
			kipl::io::ReadFITS(src,it->c_str(),crop);
		}
		catch (kipl::base::KiplException &e) {
			msg.str("");
			msg<<"Failed to open :"<<*it<<"\n"<<e.what();
			logger(kipl::logging::Logger::LogError,msg.str());
			return -1;
		}
		catch (std::exception &e) {
			msg.str("");
			msg<<"Failed to open :"<<*it<<"\n"<<e.what();
			logger(kipl::logging::Logger::LogError,msg.str());

			return -1;
		}

		if (config.bReplaceZeros==true) {
			float *pImg=src.GetDataPtr();
			for (size_t i=0; i<src.Size(); i++) {
				if (pImg[i]<1.0f)
					pImg[i]=32767.0f;
			}
		}
		switch (config.flip) {
		case kipl::base::ImageFlipNone       :
			dst=src; break;
		case kipl::base::ImageFlipHorizontal :
			dst=rotate.MirrorHorizontal(src); break;
		case kipl::base::ImageFlipVertical   :
			dst=rotate.MirrorVertical(src); break;
		case kipl::base::ImageFlipHorizontalVertical :
			dst=rotate.MirrorHorizontal(rotate.MirrorVertical(src)); break;
		}

		switch (config.rotate) {
		case kipl::base::ImageRotateNone : break;
		case kipl::base::ImageRotate90   :
			dst=rotate.Rotate90(dst); break;
		case kipl::base::ImageRotate180  :
			dst=rotate.Rotate180(dst); break;
		case kipl::base::ImageRotate270  :
			dst=rotate.Rotate270(dst); break;
		}

		kipl::strings::filenames::MakeFileName(dstmask,i,dstname,ext,'#','0');
		msg.str("");
		msg<<"Converting "<<*it<<" -> "<<dstname;
		logger(kipl::logging::Logger::LogMessage,msg.str());
		kipl::io::WriteTIFF(dst,dstname.c_str(),0.0f, 65535.0f );
	}

	return 0;
}

void Fits2Tif::BuildFileList(ImagingToolConfig::Fits2TifConfig &config, std::list<std::string> &filelist)
{
	std::ostringstream msg;
	std::string srcname,srcmask,ext;
	kipl::strings::filenames::CheckPathSlashes(config.sSourcePath,true);
	srcmask=config.sSourcePath+config.sSourceMask;

	filelist.clear();

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

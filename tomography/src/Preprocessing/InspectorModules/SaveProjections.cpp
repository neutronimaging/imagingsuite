//<LICENSE>

#include "stdafx.h"
#include "SaveProjections.h"
#include <io/io_stack.h>

SaveProjections::SaveProjections() :
	PreprocModuleBase("SaveProjections"),
    m_sPath("./"),
    m_sFileMask("projections_####.tif"),
    m_eImageType(ReconConfig::cProjections::ImageType_Projections),
    m_eFileType(kipl::io::TIFFfloat)
{
}

SaveProjections::~SaveProjections() {
}

std::map<std::basic_string<char>, std::basic_string<char> > SaveProjections::GetParameters()
{
	std::map<std::basic_string<char>, std::basic_string<char> > parameters;

    parameters["path"]=m_sPath;
    parameters["filemask"]=m_sFileMask;

	kipl::strings::filenames::CheckPathSlashes(parameters["filemask"],false);

	parameters["imagetype"]=enum2string(m_eImageType); 
    parameters["filetype"]=enum2string(m_eFileType);

	return parameters;
}

int SaveProjections::Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters)
{
	m_config=config;

    m_sPath = parameters["path"];
    kipl::strings::filenames::CheckPathSlashes(m_sPath,true);
	m_sFileMask  = parameters["filemask"];
	string2enum(parameters["imagetype"],m_eImageType);
    string2enum(parameters["filetype"],m_eFileType);

	return 0;
}

int SaveProjections::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
    std::string filemask=m_sPath+m_sFileMask;

//    (kipl::base::TImage<ImgType,3> img,const std::string fname,
//            ImgType lo, ImgType hi,
//            const size_t start, const size_t stop, const size_t count_start=0,
//            kipl::io::eFileType filetype=kipl::io::MatlabSlices,
//            const kipl::base::eImagePlanes imageplane=kipl::base::ImagePlaneYZ,
//            size_t *roi=NULL)

	switch (m_eImageType) {
		case ReconConfig::cProjections::ImageType_Projections :

            kipl::io::WriteImageStack(img,filemask,
				0.0f, 0.0f,
                0, img.Size(2), m_config.ProjectionInfo.nFirstIndex,
                m_eFileType,kipl::base::ImagePlaneXY);
			break;
		case ReconConfig::cProjections::ImageType_Sinograms : 
			{
            kipl::io::WriteImageStack(img,filemask,
                0.0f, 0.0f,
                0, img.Size(1), m_config.ProjectionInfo.roi[1],
                m_eFileType,kipl::base::ImagePlaneXZ);

//				kipl::base::TImage<float,2> sino;
//				std::string fname,ext;
//				for (size_t i=0; i<img.Size(1); i++) {
//					ExtractSinogram(img,sino,i);
//                    kipl::strings::filenames::MakeFileName(filemask,i+m_config.ProjectionInfo.nFirstIndex,fname,ext,'#','0');
//					kipl::io::WriteTIFF32(sino,fname.c_str());
//				}
			}
			break;
		default: break;
	}

	return 0;
}

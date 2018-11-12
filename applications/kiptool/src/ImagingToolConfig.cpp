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
#include <sstream>
#include <iomanip>

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/KiplException.h>
#include <memory.h>

#include "ImagingToolConfig.h"

void ImagingToolConfig::LoadConfigFile(std::string filename)
{
	xmlTextReaderPtr reader;
    const xmlChar *name;
    std::string sName;
    int ret;

	reader = xmlReaderForFile(filename.c_str(), NULL, 0);
    if (reader != NULL) {
    	ret = xmlTextReaderRead(reader);
        name = xmlTextReaderConstName(reader);

        if (name==NULL) {
			throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
        }
        sName=reinterpret_cast<const char *>(name);
        
        if (sName!="tomomerge") {
        	throw kipl::base::KiplException("Unexpected contents in parameter file this is not a tomomerge config file",__FILE__,__LINE__);
        }
        
        logger(kipl::logging::Logger::LogVerbose,"Got project");
    	
        ret = xmlTextReaderRead(reader);
        
        while (ret == 1) {
        	if (xmlTextReaderNodeType(reader)==1) {
	            name = xmlTextReaderConstName(reader);
	            
	            if (name==NULL) {
	                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
	            }
	            sName=reinterpret_cast<const char *>(name);

				if (sName=="merge")
					merge.ParseXML(reader);

				if (sName=="reslice")
					reslice.ParseXML(reader);

				if (sName=="fits2tif")
					fileconv.ParseXML(reader);
        	}
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
        	std::stringstream str;
			str<<"TomoMergeConfig failed to parse "<<filename;
        	throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
        }
    } else {
    	std::stringstream str;
		str<<"TomoMergeConfig could not open "<<filename;
    	throw kipl::base::KiplException(str.str(),__FILE__,__LINE__);
    }
    logger(kipl::logging::Logger::LogVerbose,"Parsing parameter file done");
}



std::string ImagingToolConfig::WriteXML()
{
	std::ostringstream xml;

	xml<<"<tomomerge>\n";
	xml<<merge.WriteXML(4);
	xml<<reslice.WriteXML(4);
	xml<<fileconv.WriteXML(4);
	xml<<"</tomomerge>\n";

	return xml.str();
}

ImagingToolConfig::MergeConfig::MergeConfig() :
	sPathA("./"),
	sPathB("./"),
	sPathDest("./"),
	sFileMaskA("DataA_####.tif"),
	sFileMaskB("DataB_####.tif"),
	sFileMaskDest("Mixed_####.tif"),
	nFirstA(1),
	nLastA(100),
	nStartOverlapA(80),
	nOverlapLength(10),
	nFirstB(1),
	nLastB(100),
	nFirstDest(1),
	bCropSlices(false),
	bShowLocalMix(true)
{
	memset(nCropOffset,0,sizeof(ptrdiff_t)*2);
	memset(nCrop,0,sizeof(ptrdiff_t)*4);
}

std::string ImagingToolConfig::MergeConfig::WriteXML(size_t indent)
{
	std::ostringstream xml;

	xml<<std::setw(indent-1)<<" "<<"<merge>\n";
		xml<<std::setw(indent+3)<<" "<<"<path_a>"<<sPathA<<"</path_a>\n";
		xml<<std::setw(indent+3)<<" "<<"<path_b>"<<sPathB<<"</path_b>\n";
		xml<<std::setw(indent+3)<<" "<<"<path_dest>"<<sPathDest<<"</path_dest>\n";
		xml<<std::setw(indent+3)<<" "<<"<filemask_a>"<<sFileMaskA<<"</filemask_a>\n";
		xml<<std::setw(indent+3)<<" "<<"<filemask_b>"<<sFileMaskB<<"</filemask_b>\n";
		xml<<std::setw(indent+3)<<" "<<"<filemask_dest>"<<sFileMaskDest<<"</filemask_dest>\n";
		
		xml<<std::setw(indent+3)<<" "<<"<first_a>"<<nFirstA<<"</first_a>\n";
		xml<<std::setw(indent+3)<<" "<<"<last_a>"<<nLastA<<"</last_a>\n";
		xml<<std::setw(indent+3)<<" "<<"<startoverlap_a>"<<nStartOverlapA<<"</startoverlap_a>\n";
		xml<<std::setw(indent+3)<<" "<<"<overlaplength>"<<nOverlapLength<<"</overlaplength>\n";

		xml<<std::setw(indent+3)<<" "<<"<first_b>"<<nFirstB<<"</first_b>\n";
		xml<<std::setw(indent+3)<<" "<<"<last_b>"<<nLastB<<"</last_b>\n";
		xml<<std::setw(indent+3)<<" "<<"<first_dest>"<<nFirstDest<<"</first_dest>\n";

		xml<<std::setw(indent+3)<<" "<<"<showlocalmix>"<<kipl::strings::bool2string(bShowLocalMix)<<"</showlocalmix>\n";

		xml<<std::setw(indent+3)<<" "<<"<cropslices>"<<kipl::strings::bool2string(bCropSlices)<<"</cropslices>\n";
		xml<<std::setw(indent+3)<<" "<<"<crop>"<<nCrop[0]<<" "<<nCrop[1]<<" "<<nCrop[2]<<" "<<nCrop[3]<<"</crop>\n";
		xml<<std::setw(indent+3)<<" "<<"<cropboffset>"<<nCropOffset[0]<<" "<<nCropOffset[1]<<"</cropboffset>\n";

	xml<<std::setw(indent-1)<<" "<<"</merge>\n";
	return xml.str();
}

void ImagingToolConfig::MergeConfig::ParseXML(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);
	        
	        value = xmlTextReaderConstValue(reader);
	        if (name==NULL) {
				throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
	        if (value!=NULL)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

	        if (sName=="path_a") {
				sPathA=sValue;
	        }

			if (sName=="path_b") {
				sPathB=sValue;
	        }

			if (sName=="path_dest") {
				sPathDest=sValue;
	        }

			if (sName=="filemask_a") {
				sFileMaskA=sValue;
	        }
			
			if (sName=="filemask_b") {
				sFileMaskB=sValue;
	        }

			if (sName=="filemask_dest") {
				sFileMaskDest=sValue;
	        }

			if (sName=="first_a") {
				nFirstA=atoi(sValue.c_str());
	        }

			if (sName=="first_b") {
				nFirstB=atoi(sValue.c_str());
	        }

			if (sName=="last_a") {
				nLastA=atoi(sValue.c_str());
	        }

			if (sName=="last_b") {
				nLastB=atoi(sValue.c_str());
	        }

			if (sName=="startoverlap_a") {
				nStartOverlapA=atoi(sValue.c_str());
			}

			if (sName=="overlaplength") {
				nOverlapLength=atoi(sValue.c_str());
			}

			if (sName=="first_dest") {
				nFirstDest=atoi(sValue.c_str());
			}

			if (sName=="showlocalmix") {
				bShowLocalMix=kipl::strings::string2bool(sValue);
			}

			if (sName=="cropslices") {
				bCropSlices=kipl::strings::string2bool(sValue);
			}
			if (sName=="crop") {
				kipl::strings::String2Array(sValue,nCrop,4);
			}

			if (sName=="cropboffset") {
				kipl::strings::String2Array(sValue,nCropOffset,2);
			}

		}
	    ret = xmlTextReaderRead(reader);

        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
	}

}

std::string ImagingToolConfig::ResliceConfig::WriteXML(size_t indent)
{
	std::ostringstream xml;

	size_t blockindent=3;
	xml<<std::setw(indent-1)<<" "<<"<reslice>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<srcpath>"<<sSourcePath<<"</srcpath>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<srcmask>"<<sSourceMask<<"</srcmask>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<dstpath>"<<sDestinationPath<<"</dstpath>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<first>"<<nFirst<<"</first>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<last>"<<nLast<<"</last>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<reslicexz>"<<kipl::strings::bool2string(bResliceXZ)<<"</reslicexz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<firstxz>"<<nFirstXZ<<"</firstxz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<lastxz>"<<nLastXZ<<"</lastxz>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<resliceyz>"<<kipl::strings::bool2string(bResliceYZ)<<"</resliceyz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<firstyz>"<<nFirstYZ<<"</firstyz>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<lastyz>"<<nLastYZ<<"</lastyz>\n";
	xml<<std::setw(indent-1)<<" "<<"</reslice>\n";

	return xml.str();
}
void ImagingToolConfig::ResliceConfig::ParseXML(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);
	        
	        value = xmlTextReaderConstValue(reader);
	        if (name==NULL) {
				throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
	        if (value!=NULL)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

	        if (sName=="srcpath") {
				sSourcePath=sValue;
	        }
			
			if (sName=="srcmask") {
				sSourceMask=sValue;
	        }

			if (sName=="dstpath") {
				sDestinationPath=sValue;
	        }

			if (sName=="first") {
				nFirst=atoi(sValue.c_str());
	        }

			if (sName=="last") {
				nLast=atoi(sValue.c_str());
	        }

			if (sName=="reslicexz") {
				bResliceXZ=kipl::strings::string2bool(sValue);
	        }

            if (sName=="firstxz") {
                nFirstXZ=atoi(sValue.c_str());
            }

            if (sName=="lastxz") {
                nLastXZ=atoi(sValue.c_str());
            }

			if (sName=="resliceyz") {
				bResliceYZ=kipl::strings::string2bool(sValue);
	        }

            if (sName=="firstyz") {
                nFirstYZ=atoi(sValue.c_str());
            }

            if (sName=="lastyz") {
                nLastYZ=atoi(sValue.c_str());
            }

		}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
    }
}

ImagingToolConfig::FileConversionConfig::FileConversionConfig() :
    sSourcePath("/"),
    sSourceMask("file_####.fts"),
    nFirstSrc(1),
    nLastSrc(100),
    nReadOffset(0),
    nStride(100),
    nImagesPerFile(1),
    nImgSizeX(100),
    nImgSizeY(100),
    datatype(kipl::base::UInt16),
    bCrop(false),
    flip(kipl::base::ImageFlipNone),
    rotate(kipl::base::ImageRotateNone),
    bReplaceZeros(false),
    sDestPath(""),
    sDestMask("dest_####.tif"),
    nFirstDest(1),
    bSortGoldenScan(false),
    nGoldenScanArc(0)
{
	nCrop[0]=0;
	nCrop[1]=0;
	nCrop[2]=100;
	nCrop[3]=100;
}

void ImagingToolConfig::FileConversionConfig::ParseXML(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);

	        value = xmlTextReaderConstValue(reader);
	        if (name==NULL) {
				throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
	        if (value!=NULL)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

	        if (sName=="srcpath") {
				sSourcePath=sValue;
	        }

			if (sName=="srcmask")
					sSourceMask=sValue;

			if (sName=="srcfirst") {
				nFirstSrc=atoi(sValue.c_str());
			}

		    if (sName=="srclast") {
		    	nLastSrc=atoi(sValue.c_str());
		    }

            if (sName=="readoffset") {
                nReadOffset=atoi(sValue.c_str());
            }

            if (sName=="stride") {
                nStride=atoi(sValue.c_str());
            }

            if (sName=="imagesperfile") {
                nImagesPerFile=atoi(sValue.c_str());
            }
            if (sName=="sizex") {
                nImgSizeX=atoi(sValue.c_str());
            }
            if (sName=="sizey") {
                nImgSizeY=atoi(sValue.c_str());
            }
            if (sName=="datatype") {
                string2enum(sValue,datatype);
            }

			if (sName=="dstpath")
				sDestPath=sValue;

			if (sName=="dstmask")
				sDestMask=sValue;

			if (sName=="dstfirst")
				nFirstDest=atoi(sValue.c_str());

			if (sName=="skip") {
				kipl::strings::String2Set(sValue,skip_list);
			}

			if (sName=="crop") {
				bCrop=kipl::strings::string2bool(sValue);
			}
			if (sName=="roi") {
                kipl::strings::String2Array(sValue,nCrop,4);
			}
			if (sName=="flip") {
				string2enum(sValue,flip);
			}
			if (sName=="rotate") {
				string2enum(sValue,rotate);
			}
			if (sName=="replacezeros") {
				bReplaceZeros=kipl::strings::string2bool(sValue);
			}
            if (sName=="usespotclean") {
                bUseSpotClean=kipl::strings::string2bool(sValue);
            }
            if (sName=="spotthreshold") {
                fSpotThreshold = atof(sValue.c_str());
            }
            if (sName=="sortgoldenscan") {
                bSortGoldenScan = kipl::strings::string2bool(sValue);
            }
            if (sName=="goldenscanarc") {
                nGoldenScanArc = atoi(sValue.c_str());
            }
		}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
    }
}

std::string ImagingToolConfig::FileConversionConfig::WriteXML(size_t indent)
{
	std::ostringstream xml;

	size_t blockindent=3;
	xml<<std::setw(indent-1)<<" "<<"<fits2tif>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<srcpath>"<<sSourcePath<<"</srcpath>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<srcmask>"<<sSourceMask<<"</srcmask>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<srcfirst>"<<nFirstSrc<<"</srcfirst>\n";
	    xml<<std::setw(indent+blockindent)<<" "<<"<srclast>"<<nLastSrc<<"</srclast>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<dstpath>"<<sDestPath<<"</dstpath>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<dstmask>"<<sDestMask<<"</dstmask>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<dstfirst>"<<nFirstDest<<"</dstfirst>\n";
		if (!skip_list.empty()) {
			xml<<std::setw(indent+blockindent)<<" "<<"<skip>";
			std::set<size_t>::iterator it;
			for (it=skip_list.begin(); it!=skip_list.end(); it++)
				xml<<*it<<" ";
			xml<<"</skip>\n";
		}
        xml<<std::setw(indent+blockindent)<<" "<<"<readoffset>"<<nReadOffset<<"</readoffset>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<imagesperfile>"<<nImagesPerFile<<"</imagesperfile>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<sizex>"<<nImgSizeX<<"</sizex>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<sizey>"<<nImgSizeY<<"</sizey>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<datatype>"<<enum2string(datatype)<<"</datatype>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<crop>"<<(bCrop ? "true":"false")<<"</crop>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<roi>"<<nCrop[0]<<" "
				<<nCrop[1]<<" "
				<<nCrop[2]<<" "
				<<nCrop[3]<<"</roi>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<flip>"<<flip<<"</flip>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<rotate>"<<rotate<<"</rotate>\n";
		xml<<std::setw(indent+blockindent)<<" "<<"<replacezeros>"<<(bReplaceZeros ? "true": "false")<<"</replacezeros>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<usespotclean>"<<(bUseSpotClean ? "true": "false")<<"</usespotclean>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<spotthreshold>"<<fSpotThreshold<<"</spotthreshold>\n";
        xml<<std::setw(indent+blockindent)<<" "<<"<sortgoldenscan>"<< (bSortGoldenScan ? "true" : "false")<<"</sortgoldenscan>";
        xml<<std::setw(indent+blockindent)<<" "<<"<goldenscanarc>"<<nGoldenScanArc<<"</goldenscanarc>";
    xml<<std::setw(indent-1)<<" "<<"</fits2tif>\n";

	return xml.str();
}

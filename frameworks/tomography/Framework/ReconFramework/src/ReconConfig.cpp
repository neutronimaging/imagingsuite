//<LICENSE>

#include "../include/ReconFramework_global.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>

#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <strings/filenames.h>

#include <ModuleException.h>
#include <averageimage.h>

#include "../include/ReconConfig.h"
#include "../include/ReconException.h"


ReconConfig::ReconConfig(const std::string &appPath) :
    ConfigBase("ReconConfig",appPath),
    backprojector(appPath)
{

}

ReconConfig::~ReconConfig(void)
{
}

ReconConfig::ReconConfig(const ReconConfig &config) :
        ConfigBase(config),
	System(config.System),
	ProjectionInfo(config.ProjectionInfo),
	MatrixInfo(config.MatrixInfo),
	backprojector(config.backprojector)
{

}


const ReconConfig & ReconConfig::operator=(const ReconConfig &config)
{
    ConfigBase::operator=(config);
    UserInformation  = config.UserInformation;
    System           = config.System;
    ProjectionInfo   = config.ProjectionInfo;
    MatrixInfo       = config.MatrixInfo;
    modules          = config.modules;
    backprojector    = config.backprojector;

	return *this;
}

bool ReconConfig::SanityCheck()
{
    std::ostringstream msg;

    if (ProjectionInfo.roi[2]<=ProjectionInfo.roi[0])
    {
        msg<<"Incorrect config: ROI x1<x0 (x0="<<ProjectionInfo.roi[0]<<", x1="<<ProjectionInfo.roi[2]<<")";
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    if (ProjectionInfo.roi[3]<=ProjectionInfo.roi[1])
    {
        msg<<"Incorrect config: ROI y1<y0 (y0="<<ProjectionInfo.roi[1]<<", y1="<<ProjectionInfo.roi[3]<<")";
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    return true;
}

bool ReconConfig::SanityAnglesCheck()
{
    std::ostringstream msg;

    if ( ((ProjectionInfo.scantype==ProjectionInfo.GoldenSectionScan) || (ProjectionInfo.scantype==ProjectionInfo.InvGoldenSectionScan))
            && (ProjectionInfo.fScanArc[1]!=180.0f && ProjectionInfo.fScanArc[1]!=360.0f))
    {
        msg<<"Incorrect angles configuration " ;
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    return true;
}

std::string ReconConfig::WriteXML()
{
	std::ostringstream str;

	int indent=4;
	str<<"<reconstructor>\n";
		str<<UserInformation.WriteXML(indent);
		str<<System.WriteXML(indent);
		str<<ProjectionInfo.WriteXML(indent);
		str<<MatrixInfo.WriteXML(indent);

		str<<std::setw(indent)<<" "<<"<processchain>\n";
		if (!modules.empty()) {
			str<<std::setw(indent+4)<<" "<<"<preprocessing>\n";
			std::list<ModuleConfig>::iterator it;

            for (auto & module : modules) {
                module.setAppPath(m_sApplicationPath);
                str<<module.WriteXML(indent+8);
			}
			str<<std::setw(indent+4)<<" "<<"</preprocessing>\n";
		}
		str<<std::setw(indent+4)<<" "<<"<backprojector>\n";
		str<<backprojector.WriteXML(indent+8);
		str<<std::setw(indent+4)<<" "<<"</backprojector>\n";

		str<<std::setw(indent)<<" "<<"</processchain>\n";


		str<<"</reconstructor>"<<std::endl;

		return str.str();
}

void ReconConfig::ParseConfig(xmlTextReaderPtr reader, std::string sName)
{
	if (sName=="system")
		ParseSystem(reader);

	if (sName=="projections")
		ParseProjections(reader);

	if (sName=="matrix")
		ParseMatrix(reader);



}
				
void ReconConfig::ParseArgv(std::vector<std::string> &args)
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"ReconConfig argvparse");
    std::string group;
    std::string var;
    std::string value;

    std::vector<std::string>::iterator it;
    for (it=args.begin()+3 ; it!=args.end(); it++) {
        try {
            EvalArg(*it,group,var,value);
            msg.str("");
            msg<<"group: "<<group<<", var: "<<var<<", value: "<<value;
            logger.message(msg.str());
        }
        catch (ModuleException &e) {
            msg<<"Failed to parse argument "<<e.what();
            logger(kipl::logging::Logger::LogWarning,msg.str());
        }
        if (group=="userinformation") {
            if (var=="operator")      UserInformation.sOperator      = value;
            if (var=="instrument")    UserInformation.sInstrument    = value;
            if (var=="projectnumber") UserInformation.sProjectNumber = value;
            if (var=="sample")        UserInformation.sSample        = value;
            if (var=="comment")       UserInformation.sComment       = value;
        }

        if (group=="system")
        {
            if (var=="memory")       System.nMemory = std::stoul(value);
            if (var=="loglevel")     string2enum(value,System.eLogLevel);
            if (var=="validate")     System.bValidateData = kipl::strings::string2bool(value);
            if (var=="maxthreads")   System.nMaxThreads = std::stoi(value);
            if (var=="threadmethod") string2enum(value,System.eThreadMethod);
        }

        if (group=="projections") {
            if (var=="dims")           kipl::strings::String2Array(value,ProjectionInfo.nDims,2);
            if (var=="resolution")     kipl::strings::String2Array(value,ProjectionInfo.fResolution,2);
            if (var=="binning")        ProjectionInfo.fBinning           = std::stof(value);
            if (var=="margin")         ProjectionInfo.nMargin            = std::stoul(value);
            if (var=="firstindex")     ProjectionInfo.nFirstIndex        = std::stoul(value);
            if (var=="lastindex")      ProjectionInfo.nLastIndex         = std::stoul(value);
            if (var=="projectionstep") ProjectionInfo.nProjectionStep    = std::stoul(value);
            if (var=="repeatline")     ProjectionInfo.bRepeatLine=kipl::strings::string2bool(value);
            if (var=="scantype")       string2enum(value,ProjectionInfo.scantype);
            if (var=="imagetype")      string2enum(value,ProjectionInfo.imagetype);
            if (var=="center")         ProjectionInfo.fCenter            = std::stof(value);
            if (var=="tiltangle")      ProjectionInfo.fTiltAngle         = std::stof(value);
            if (var=="tiltpivot")      ProjectionInfo.fTiltPivotPosition = std::stof(value);
            if (var=="correcttilt")    ProjectionInfo.bCorrectTilt=kipl::strings::string2bool(value);
            if (var=="filemask") 
            {     
                ProjectionInfo.sFileMask = value;
                kipl::strings::filenames::CheckPathSlashes(ProjectionInfo.sFileMask,false); 
                
            }
            if (var=="path") 
            {
                ProjectionInfo.sPath=value;
                kipl::strings::filenames::CheckPathSlashes(ProjectionInfo.sPath,true);
            }
            if (var=="referencepath")
            {
                ProjectionInfo.sReferencePath=value;
                kipl::strings::filenames::CheckPathSlashes(ProjectionInfo.sReferencePath,true);
            }
            if (var=="obfilemask")   ProjectionInfo.sOBFileMask   = value;
            if (var=="obfirstindex") ProjectionInfo.nOBFirstIndex = std::stoul(value);
            if (var=="obcount")      ProjectionInfo.nOBCount      = std::stoul(value);
            if (var=="dcfilemask")   ProjectionInfo.sDCFileMask   = value;
            if (var=="dcfirstindex") ProjectionInfo.nDCFirstIndex = std::stoul(value);
            if (var=="dccount")      ProjectionInfo.nDCCount      = std::stoul(value);
            if (var=="roi")          kipl::strings::String2Array(value,ProjectionInfo.roi,4);
            if (var=="projroi")      kipl::strings::String2Array(value,ProjectionInfo.projection_roi,4);
            if (var=="doseroi")      kipl::strings::String2Array(value,ProjectionInfo.dose_roi,4);
            if (var=="scanarc")      kipl::strings::String2Array(value,ProjectionInfo.fScanArc,2);
            if (var=="scanarc0")     ProjectionInfo.fScanArc[0]   = std::stof(value);
            if (var=="scanarc1")     ProjectionInfo.fScanArc[1]   = std::stof(value);
            if (var=="rotate")       string2enum(value,ProjectionInfo.eRotate);
            if (var=="flip")         string2enum(value,ProjectionInfo.eFlip);
            if (var=="direction")    string2enum(value, ProjectionInfo.eDirection);
            if (var=="sod")          ProjectionInfo.fSOD          = std::stof(value);
            if (var=="sdd")          ProjectionInfo.fSDD          = std::stof(value);
            if (var=="pPoint")       kipl::strings::String2Array(value,ProjectionInfo.fpPoint,2);
            if (var=="skiplistmode") string2enum(value,ProjectionInfo.skipListMode);
        }

        if (group=="matrix")
        {
            if (var=="dims")         kipl::strings::String2Array(value,MatrixInfo.nDims,3);
            if (var=="rotation")     MatrixInfo.fRotation           = std::stof(value);
            if (var=="serialize")    MatrixInfo.bAutomaticSerialize = kipl::strings::string2bool(value);
            if (var=="path")
            {
                    MatrixInfo.sDestinationPath=value;
                    kipl::strings::filenames::CheckPathSlashes(MatrixInfo.sDestinationPath,true);
            }
            if (var=="matrixname")   MatrixInfo.sFileMask   = value;
            if (var=="filetype")     string2enum(value,MatrixInfo.FileType);
            if (var=="firstindex")   MatrixInfo.nFirstIndex = std::stoul(value);
            if (var=="grayinterval") kipl::strings::String2Array(value,MatrixInfo.fGrayInterval,2);
            if (var=="useroi")       MatrixInfo.bUseROI=kipl::strings::string2bool(value);
            if (var=="roi")          kipl::strings::String2Array(value,MatrixInfo.roi,4);
            if (var=="voi")          kipl::strings::String2Array(value,MatrixInfo.voi,6);
        }
    }
}

void ReconConfig::ParseSystem(xmlTextReaderPtr reader)
{
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);

    while (ret == 1)
    {
        if (xmlTextReaderNodeType(reader)==1)
        {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);
	        
	        value = xmlTextReaderConstValue(reader);
            if (name==nullptr)
            {
	            throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }

            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

            if (sName=="memory")
            {
                System.nMemory=static_cast<size_t>(std::stoi(sValue));
	        }

	        if (sName=="loglevel") 
                string2enum(sValue,System.eLogLevel);

            if (sName=="validate")
                System.bValidateData=kipl::strings::string2bool(sValue);

            if (sName=="maxthreads")
                System.nMaxThreads=std::stoi(sValue);

            if (sName=="threadmethod")
                string2enum(sValue,System.eThreadMethod);
		}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
    }
}

void ReconConfig::ParseProjections(xmlTextReaderPtr reader)
{
	std::ostringstream msg;
	const xmlChar *name, *value;
    int ret = xmlTextReaderRead(reader);
    std::string sName, sValue;
    int depth=xmlTextReaderDepth(reader);
	ProjectionInfo.fScanArc[0]=0.0f;
    ProjectionInfo.fScanArc[1]=180.0f;

    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);

	        value = xmlTextReaderConstValue(reader);
            if (name==nullptr) {
	            throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
                sValue="";
	        sName=reinterpret_cast<const char *>(name);

            if (sName=="dims")            kipl::strings::String2Array(sValue,ProjectionInfo.nDims,2);
            if (sName=="beamgeometry")    string2enum(sValue,ProjectionInfo.beamgeometry);

            if (sName=="resolution")      kipl::strings::String2Array(sValue,ProjectionInfo.fResolution,2);
            if (sName=="binning")         ProjectionInfo.fBinning        = std::stof(sValue);

	        if (sName=="firstindex")      ProjectionInfo.nFirstIndex     = atoi(sValue.c_str());
	        if (sName=="lastindex")       ProjectionInfo.nLastIndex          = atoi(sValue.c_str());
	        if (sName=="projectionstep")  ProjectionInfo.nProjectionStep = atoi(sValue.c_str());
            if (sName=="repeatedview")    ProjectionInfo.nRepeatedView   = std::stoul(sValue);
            if (sName=="averagemethod")   string2enum(sValue,ProjectionInfo.averageMethod);
            if (sName=="skiplistmode")    string2enum(sValue,ProjectionInfo.skipListMode);
			if (sName=="skipprojections") {
				kipl::strings::String2Set(sValue,ProjectionInfo.nlSkipList);
				msg<<"Skip list: "<<kipl::strings::Set2String(ProjectionInfo.nlSkipList);
				logger(kipl::logging::Logger::LogVerbose,msg.str());
			}

            if (sName=="repeatline")	  ProjectionInfo.bRepeatLine   = kipl::strings::string2bool(sValue);
			if (sName=="scantype")		  string2enum(sValue,ProjectionInfo.scantype);
            if (sName=="goldenstartidx")  ProjectionInfo.nGoldenStartIdx = std::stoul(sValue);
			if (sName=="imagetype")		  string2enum(sValue,ProjectionInfo.imagetype);
            if (sName=="center")          ProjectionInfo.fCenter       = std::stof(sValue);
            if (sName=="sod")             ProjectionInfo.fSOD          = std::stof(sValue);
            if (sName=="sdd")             ProjectionInfo.fSDD          = std::stof(sValue);
            if (sName=="pPoint")          kipl::strings::String2Array(sValue,ProjectionInfo.fpPoint,2);
            if (sName=="tiltangle")       ProjectionInfo.fTiltAngle    = std::stof(sValue);
            if (sName=="tiltpivot")       ProjectionInfo.fTiltPivotPosition = std::stof(sValue);

            if (sName=="correcttilt") 	ProjectionInfo.bCorrectTilt   = kipl::strings::string2bool(sValue);

            if (sName=="filemask")      ProjectionInfo.sFileMask      = sValue;
            if (sName=="path") 		    ProjectionInfo.sPath          = sValue;

            if (sName=="referencepath") ProjectionInfo.sReferencePath = sValue;
            if (sName=="obfilemask") 	ProjectionInfo.sOBFileMask    = sValue;
            if (sName=="obfirstindex")  ProjectionInfo.nOBFirstIndex  = std::stoul(sValue);
            if (sName=="obcount")       ProjectionInfo.nOBCount       = std::stoul(sValue);
            if (sName=="dcfilemask")    ProjectionInfo.sDCFileMask    = sValue;
            if (sName=="dcfirstindex")  ProjectionInfo.nDCFirstIndex  = std::stoul(sValue);
            if (sName=="dccount")       ProjectionInfo.nDCCount       = std::stoul(sValue);
            if (sName=="roi")           kipl::strings::String2Array(sValue,ProjectionInfo.roi,4);
            if (sName=="projroi")       kipl::strings::String2Array(sValue,ProjectionInfo.projection_roi,4);

            if (sName=="doseroi")       kipl::strings::String2Array(sValue,ProjectionInfo.dose_roi,4);
			if (sName=="scanarc") {
	        	size_t cnt=kipl::strings::String2Array(sValue,ProjectionInfo.fScanArc,2);
				if (cnt==1) {
					ProjectionInfo.fScanArc[1]=ProjectionInfo.fScanArc[0];
					ProjectionInfo.fScanArc[0]=0.0f;
				}
			}
            if (sName=="rotate")         string2enum(sValue,ProjectionInfo.eRotate);
            if (sName=="flip")           string2enum(sValue,ProjectionInfo.eFlip);
            if (sName=="direction")      string2enum(sValue,ProjectionInfo.eDirection);
    	}
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
    }

}

std::string ReconConfig::SanitySlicesCheck()
{
    int fS = static_cast<int>(ProjectionInfo.roi[1]);
    int lS = static_cast<int>(ProjectionInfo.roi[3]);
    std::string msg;

    if ((lS-fS)>=200)
    {
        msg=SanityMessage(true);

    }
    else
        msg="";


    return(msg);

}

std::string ReconConfig::SanityMessage(bool mess)
{
    std::ostringstream msg;
    if (mess)
    {
        msg<<"Trying to configure more than 200 slices. Continue?";
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
    else
        {
        msg<<"";
    }

    return(msg.str());

}

void ReconConfig::ParseMatrix(xmlTextReaderPtr reader)
{
	MatrixInfo.fGrayInterval[0]=0.0f;
	MatrixInfo.fGrayInterval[1]=0.0f;
	const xmlChar *name, *value;
	std::string sName,sValue;
    int ret = xmlTextReaderRead(reader);
    int depth=xmlTextReaderDepth(reader);
    while (ret == 1) {
    	if (xmlTextReaderNodeType(reader)==1) {
	        name = xmlTextReaderConstName(reader);
	        ret=xmlTextReaderRead(reader);
	        
	        value = xmlTextReaderConstValue(reader);
            if (name==nullptr) {
	            throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);
	        	       
            if (name==nullptr) {
	            throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        
	        sName=reinterpret_cast<const char *>(name);	                
	        
	        if (sName=="dims") 
	        	kipl::strings::String2Array(sValue,MatrixInfo.nDims,3);
			if (sName=="serialize") 		MatrixInfo.bAutomaticSerialize = kipl::strings::string2bool(sValue);
	        if (sName=="path") 				MatrixInfo.sDestinationPath    = sValue;
	        if (sName=="matrixname") 	  	MatrixInfo.sFileMask           = sValue;
			if (sName=="filetype")			string2enum(sValue,MatrixInfo.FileType);
            if (sName=="firstindex") 		MatrixInfo.nFirstIndex         = std::stoul(sValue);
	        if (sName=="grayinterval") 
	        	kipl::strings::String2Array(sValue,MatrixInfo.fGrayInterval,2);
            if (sName=="rotation")			MatrixInfo.fRotation           = std::stof(sValue);
			if (sName=="useroi")			MatrixInfo.bUseROI = kipl::strings::string2bool(sValue);
			if (sName=="roi")				kipl::strings::String2Array(sValue,MatrixInfo.roi,4);
            if (sName=="voxelsize")         kipl::strings::String2Array(sValue,MatrixInfo.fVoxelSize,3);
//            if (sName=="usevoi")            MatrixInfo.bUseVOI = kipl::strings::string2bool(sValue);
            if (sName=="voi")               kipl::strings::String2Array(sValue,MatrixInfo.voi, 6);
    	}
        ret = xmlTextReaderRead(reader);
    
        if (xmlTextReaderDepth(reader)<depth)
        	ret=0;
    }
}

void ReconConfig::ParseProcessChain(xmlTextReaderPtr reader)
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
            if (name==nullptr) {
	            throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
	        }
            if (value!=nullptr)
	        	sValue=reinterpret_cast<const char *>(value);
	        else
	        	sValue="Empty";
	        sName=reinterpret_cast<const char *>(name);

	        if (sName=="preprocessing") {
				logger(kipl::logging::Logger::LogVerbose,"Parsing preproc");
				int depth2=xmlTextReaderDepth(reader);
			    while (ret == 1) {
					if (xmlTextReaderNodeType(reader)==1) {
						name = xmlTextReaderConstName(reader);
						ret=xmlTextReaderRead(reader);

						value = xmlTextReaderConstValue(reader);
                        if (name==nullptr) {
							throw ReconException("Unexpected contents in parameter file",__FILE__,__LINE__);
						}
                        if (value!=nullptr)
        					sValue=reinterpret_cast<const char *>(value);
						else
        					sValue="Empty";
						sName=reinterpret_cast<const char *>(name);
						if (sName=="module") {
                            ModuleConfig module(m_sApplicationPath);
                            module.ParseModule(reader);
							modules.push_back(module);
						}
					}
					ret = xmlTextReaderRead(reader);
					if (xmlTextReaderDepth(reader)<depth2)
						ret=0;
				}
			}
			if (sName=="backprojector") {
				logger(kipl::logging::Logger::LogVerbose,"Parsing backproj");
                backprojector.setAppPath(m_sApplicationPath);
                backprojector.ParseModule(reader);
			}

		}
		ret = xmlTextReaderRead(reader);
		if (xmlTextReaderDepth(reader)<depth)
    		ret=0;
	}
}


//----------------------
// ReconConfig::cUserInformation::cUserInformation() :
// 	sOperator("Anders Kaestner"),
// 	sInstrument("ICON"),
// 	sProjectNumber("P11001"),
// 	sSample("Unknown item"),
// 	sComment("No comment")
// {
// }

// ReconConfig::cUserInformation::cUserInformation(const cUserInformation &info) :
// 	sOperator(info.sOperator),
// 	sInstrument(info.sInstrument),
// 	sProjectNumber(info.sProjectNumber),
// 	sSample(info.sSample),
//     sComment(info.sComment)
// {
// }

// ReconConfig::cUserInformation & ReconConfig::cUserInformation::operator = (const cUserInformation &info)
// {
// 	sOperator      = info.sOperator;
// 	sInstrument    = info.sInstrument;
// 	sProjectNumber = info.sProjectNumber;
// 	sSample        = info.sSample;
// 	sComment       = info.sComment;

// 	return * this;
// }

// std::string ReconConfig::cUserInformation::WriteXML(int indent)
// {
// 	using namespace std;
// 	ostringstream str;

//     str<<std::setw(indent)  <<" "<<"<userinformation>"<<std::endl;
//         str<<std::setw(indent+4)  <<" "<<"<operator>"<<sOperator<<"</operator>\n";
//         str<<std::setw(indent+4)  <<" "<<"<instrument>"<<sInstrument<<"</instrument>\n";
//         str<<std::setw(indent+4)  <<" "<<"<projectnumber>"<<sProjectNumber<<"</projectnumber>\n";
//         str<<std::setw(indent+4)  <<" "<<"<sample>"<<sSample<<"</sample>\n";
//         str<<std::setw(indent+4)  <<" "<<"<comment>"<<sComment<<"</comment>\n";
//     str<<std::setw(indent)  <<" "<<"</userinformation>"<<std::endl;

// 	return str.str();
// }


//----------------
ReconConfig::cSystem::cSystem(): 
	nMemory(1500ul),
    eLogLevel(kipl::logging::Logger::LogMessage),
    bValidateData(false),
    nMaxThreads(-1),
    eThreadMethod(kipl::base::threadingSTL)
{
    setNumberOfThreads(nMaxThreads);
}

ReconConfig::cSystem::cSystem(const cSystem &a) : 
	nMemory(a.nMemory), 
    eLogLevel(a.eLogLevel),
    bValidateData(a.bValidateData),
    nMaxThreads(a.nMaxThreads),
    eThreadMethod(a.eThreadMethod)
{
    setNumberOfThreads(nMaxThreads);
}

ReconConfig::cSystem & ReconConfig::cSystem::operator=(const cSystem &a) 
{
    nMemory       = a.nMemory;
    eLogLevel     = a.eLogLevel;
    bValidateData = a.bValidateData;
    nMaxThreads   = a.nMaxThreads;
    eThreadMethod = a.eThreadMethod;

    setNumberOfThreads(a.nMaxThreads);

	return *this;
}

std::string ReconConfig::cSystem::WriteXML(int indent)
{
	using namespace std;
	ostringstream str;
	
	str<<setw(indent)  <<" "<<"<system>"<<std::endl;
	str<<setw(indent+4)<<" "<<"<memory>"<<nMemory<<"</memory>"<<std::endl;
	str<<setw(indent+4)<<"  "<<"<loglevel>"<<eLogLevel<<"</loglevel>"<<std::endl;
    str<<setw(indent+4)<<"  "<<"<validate>"<<kipl::strings::bool2string(bValidateData)<<"</validate>"<<std::endl;
    str<<setw(indent+4)<<" "<<"<maxthreads>"<<nMaxThreads<<"</maxthreads>"<<std::endl;
    str<<setw(indent+4)<<" "<<"<threadmethod>"<<eThreadMethod<<"</threadmethod>"<<std::endl;
    str<<setw(indent)  <<"  "<<"</system>"<<std::endl;

	return str.str();
}		

void ReconConfig::cSystem::setNumberOfThreads(int N)
{
    int hwMaxThreads = std::thread::hardware_concurrency();

    if ((N<1) || (hwMaxThreads<N))
    {
        nMaxThreads = hwMaxThreads;
    }
    else
    {
        nMaxThreads = N;
    }
}

//---------
ReconConfig::cProjections::cProjections() :
    nDims(2,2048),
    beamgeometry(BeamGeometry_Parallel),
    fResolution(2,0.01f),
    fBinning(1),
    nMargin(2), // modify to 0
    nFirstIndex(1),
    nLastIndex(625),
    nProjectionStep(1),
    nRepeatedView(1),
    averageMethod(ImagingAlgorithms::AverageImage::ImageWeightedAverage),
    skipListMode(SkipMode_None),
    bRepeatLine(false),
    scantype(SequentialScan),
    nGoldenStartIdx(0),
    imagetype(ImageType_Projections),
    fCenter(1024.0f),
    fSOD(100.0f),
    fSDD(100.0f),
    fpPoint(2,500.0f),
    fTiltAngle(0.0f),
    fTiltPivotPosition(0.0f),
    bCorrectTilt(false),
    sFileMask("proj_####.fits"),
    sPath(""),
    sReferencePath(""),
    sOBFileMask("ob_####.fits"),
    nOBFirstIndex(1),
    nOBCount(5),
    sDCFileMask("dc_####.fits"),
    nDCFirstIndex(1),
    nDCCount(5),
    roi({0,0,2047,2047}),
    projection_roi({0,0,2047,2047}),
    dose_roi({0,0,10,10}),
    fScanArc({0.0f,360.0f}),
    eFlip(kipl::base::ImageFlipNone),
    eRotate(kipl::base::ImageRotateNone),
    eDirection(kipl::base::RotationDirCW) // default clockwise
{
}

ReconConfig::cProjections::cProjections(const cProjections & a) :
    nDims(a.nDims),
    beamgeometry(a.beamgeometry),
    fResolution(a.fResolution),
	fBinning(a.fBinning),
    nMargin(a.nMargin),
	nFirstIndex(a.nFirstIndex),
	nLastIndex(a.nLastIndex),
	nProjectionStep(a.nProjectionStep),
    nRepeatedView(a.nRepeatedView),
    averageMethod(a.averageMethod),
    skipListMode(a.skipListMode),
	nlSkipList(a.nlSkipList),
	bRepeatLine(a.bRepeatLine),
	scantype(a.scantype),
    nGoldenStartIdx(a.nGoldenStartIdx),
	imagetype(a.imagetype),
	fCenter(a.fCenter),
    fSOD(a.fSOD),
    fSDD(a.fSDD),
    fpPoint(a.fpPoint),
	fTiltAngle(a.fTiltAngle),
	fTiltPivotPosition(a.fTiltPivotPosition),
	bCorrectTilt(a.bCorrectTilt),
	sFileMask(a.sFileMask),
	sPath(a.sPath),
	sReferencePath(a.sReferencePath),
	sOBFileMask(a.sOBFileMask),
	nOBFirstIndex(a.nOBFirstIndex),
	nOBCount(a.nOBCount),
	sDCFileMask(a.sDCFileMask),
	nDCFirstIndex(a.nDCFirstIndex),
	nDCCount(a.nDCCount),
    roi(a.roi),
    projection_roi(a.projection_roi),
    dose_roi(a.dose_roi),
    fScanArc(a.fScanArc),
	eFlip(a.eFlip),
    eRotate(a.eRotate),
    eDirection(a.eDirection)
{	
	
}

ReconConfig::cProjections & ReconConfig::cProjections::operator=(const cProjections &a)
{
    beamgeometry    = a.beamgeometry;
	fBinning        = a.fBinning;
    nMargin         = a.nMargin;
	nFirstIndex     = a.nFirstIndex;
	nProjectionStep = a.nProjectionStep;
    nRepeatedView   = a.nRepeatedView;
    averageMethod   = a.averageMethod;
    skipListMode    = a.skipListMode;
	bRepeatLine     = a.bRepeatLine;
	scantype		= a.scantype;
    nGoldenStartIdx = a.nGoldenStartIdx;
	imagetype		= a.imagetype;
	nLastIndex		= a.nLastIndex;
	nlSkipList		= a.nlSkipList;
	fCenter         = a.fCenter;
    fSOD            = a.fSOD;
    fSDD            = a.fSDD;
	fTiltAngle      = a.fTiltAngle;
	fTiltPivotPosition = a.fTiltPivotPosition;
	bCorrectTilt    = a.bCorrectTilt;
	sFileMask       = a.sFileMask;
	sPath           = a.sPath;

	sReferencePath  = a.sReferencePath;
	sOBFileMask     = a.sOBFileMask;
	nOBFirstIndex   = a.nOBFirstIndex;
	nOBCount	    = a.nOBCount;
	sDCFileMask     = a.sDCFileMask;
	nDCFirstIndex   = a.nDCFirstIndex;
    nDCCount        = a.nDCCount;

    nDims = a.nDims;

    fResolution = a.fResolution;

    roi         = a.roi;
    fScanArc    = a.fScanArc;

    fpPoint     = a.fpPoint;
    projection_roi = a.projection_roi;
    dose_roi    = a.dose_roi;

    eFlip       = a.eFlip;
    eRotate     = a.eRotate;
    eDirection  = a.eDirection;

	return *this;
}

std::string ReconConfig::cProjections::WriteXML(int indent)
{
	using namespace std;
	ostringstream str;
	str<<setw(indent)  <<" "<<"<projections>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<dims>"<<nDims[0]<<" "<<nDims[1]<<"</dims>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<beamgeometry>"<<beamgeometry<<"</beamgeometry>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<resolution>"<<fResolution[0]<<" "<<fResolution[1]<<"</resolution>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<binning>"<<fBinning<<"</binning>\n";
    str<<setw(indent+4)  <<" "<<"<margin>"<<nMargin<<"</margin>\n";
	str<<setw(indent+4)  <<" "<<"<firstindex>"<<nFirstIndex<<"</firstindex>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<lastindex>"<<nLastIndex<<"</lastindex>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<projectionstep>"<<nProjectionStep<<"</projectionstep>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<repeatedview>"<<nRepeatedView<<"</repeatedview>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<averagemethod>"<<averageMethod<<"</averagemethod>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<skiplistmode>"<<skipListMode<<"</skiplistmode>"<<std::endl;
	if (!nlSkipList.empty()) {
		str<<setw(indent+4)  <<" "<<"<skipprojections>"<<kipl::strings::Set2String(nlSkipList)<<"</skipprojections>"<<std::endl;
	}
	str<<setw(indent+4)  <<" "<<"<repeatline>"<<kipl::strings::bool2string(bRepeatLine)<<"</repeatline>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<scantype>"<<scantype<<"</scantype>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<goldenstartidx>"<<nGoldenStartIdx<<"</goldenstartidx>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<imagetype>"<<enum2string(imagetype)<<"</imagetype>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<center>"<<fCenter<<"</center>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<sod>"<<fSOD<<"</sod>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<sdd>"<<fSDD<<"</sdd>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<direction>"<<eDirection<<"</direction>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<pPoint>"<<fpPoint[0]<<" "<<fpPoint[1]<<"</pPoint>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<tiltangle>"<<fTiltAngle<<"</tiltangle>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<tiltpivot>"<<fTiltPivotPosition<<"</tiltpivot>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<correcttilt>"<<(bCorrectTilt==true ? "true" : "false")<<"</correcttilt>"<<std::endl;

	str<<setw(indent+4)  <<" "<<"<filemask>"<<sFileMask<<"</filemask>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<path>"<<sPath<<"</path>"<<std::endl;

	str<<setw(indent+4)  <<" "<<"<referencepath>"<<sReferencePath<<"</referencepath>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<obfilemask>"<<sOBFileMask<<"</obfilemask>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<obfirstindex>"<<nOBFirstIndex<<"</obfirstindex>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<obcount>"<<nOBCount<<"</obcount>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<dcfilemask>"<<sDCFileMask<<"</dcfilemask>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<dcfirstindex>"<<nDCFirstIndex<<"</dcfirstindex>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<dccount>"<<nDCCount<<"</dccount>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<roi>"<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"</roi>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<projroi>"<<projection_roi[0]<<" "<<projection_roi[1]<<" "<<projection_roi[2]<<" "<<projection_roi[3]<<"</projroi>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<doseroi>"<<dose_roi[0]<<" "<<dose_roi[1]<<" "<<dose_roi[2]<<" "<<dose_roi[3]<<"</doseroi>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<scanarc>"<<fScanArc[0]<<" "<<fScanArc[1]<<"</scanarc>"<<std::endl;
	str<<setw(indent+4)  <<" "<<"<rotate>"<<eRotate<<"</rotate>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<flip>"<<eFlip<<"</flip>"<<std::endl;
	str<<setw(indent)  <<" "<<"</projections>"<<std::endl;
	return str.str();
}

//---------------------------------------------------------
void string2enum(const std::string str, ReconConfig::cProjections::eScanType &st)
{
	if (str=="sequential")
		st=ReconConfig::cProjections::SequentialScan;
	else if (str=="goldensection")
		st=ReconConfig::cProjections::GoldenSectionScan;
    else if (str=="invgoldensection")
        st=ReconConfig::cProjections::InvGoldenSectionScan;
	else
		throw ReconException("Undefined enum in string2enum (scantype)", __FILE__,__LINE__);
}

void string2enum(const std::string str, ReconConfig::cProjections::eImageType &it)
{
	if (str=="projections")
		it=ReconConfig::cProjections::ImageType_Projections;
	else if (str=="sinograms")
		it=ReconConfig::cProjections::ImageType_Sinograms;
	else if (str=="proj_repeatprojection")
		it=ReconConfig::cProjections::ImageType_Proj_RepeatProjection;
	else if (str=="proj_repeatsinogram")
		it=ReconConfig::cProjections::ImageType_Proj_RepeatSinogram;
    else {
        std::ostringstream msg;
        msg<<"Undefined enum in string2enum (imagetype). Got "<<str;
        throw ReconException(msg.str(), __FILE__,__LINE__);
    }
}

std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eScanType st)
{
    s<<enum2string(st);

    return s;
}


std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eImageType it)
{
    s<<enum2string(it);
	return s;
}


//-----------------------------------------------

ReconConfig::cMatrix::cMatrix() :
    nDims(3,0UL),
	fRotation(0.0f),
	sDestinationPath(""),
	bAutomaticSerialize(false),
	sFileMask("slice_####.tif"),
	nFirstIndex(0),
    fGrayInterval({0.0f,5.0f}),
	bUseROI(false),
    roi(4,0UL),
    voi(6,0UL),
//    bUseVOI(false),
    FileType(kipl::io::TIFF16bits),
    fVoxelSize(3,0.0f)
{
	bAutomaticSerialize=true;
}

ReconConfig::cMatrix::cMatrix(const cMatrix &a) :
    nDims(a.nDims),
	fRotation(a.fRotation),
	sDestinationPath(a.sDestinationPath),
	bAutomaticSerialize(a.bAutomaticSerialize),
	sFileMask(a.sFileMask),
	nFirstIndex(a.nFirstIndex),
    fGrayInterval(a.fGrayInterval),
	bUseROI(a.bUseROI),
    roi(a.roi),
    voi(a.voi),
//    bUseVOI(a.bUseVOI),
    FileType(a.FileType),
    fVoxelSize(a.fVoxelSize)
{
}

ReconConfig::cMatrix & ReconConfig::cMatrix::operator=(const cMatrix &a) 
{
	sDestinationPath = a.sDestinationPath; 
	sFileMask        = a.sFileMask;
	nFirstIndex      = a.nFirstIndex;

    nDims     = a.nDims;
    fRotation = a.fRotation;
    fGrayInterval = a.fGrayInterval;
	FileType = a.FileType;
	bAutomaticSerialize = a.bAutomaticSerialize;

	bUseROI=a.bUseROI;
    roi = a.roi;
    voi = a.voi;

    fVoxelSize = a.fVoxelSize;

	return *this;
}


std::string ReconConfig::cMatrix::WriteXML(int indent)
{
	using namespace std;
	ostringstream str;

	str<<setw(indent)  <<" "<<"<matrix>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<dims>"<<nDims[0]<<" "<<nDims[1]<<" "<<nDims[2]<<"</dims>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<rotation>"<<fRotation<<"</rotation>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<serialize>"<<(bAutomaticSerialize==true ? "true" : "false")<<"</serialize>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<path>"<<sDestinationPath<<"</path>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<matrixname>"<<sFileMask<<"</matrixname>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<filetype>"<<FileType<<"</filetype>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<firstindex>"<<nFirstIndex<<"</firstindex>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<grayinterval>"<<fGrayInterval[0]<<" "<<fGrayInterval[1]<<"</grayinterval>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<useroi>"<<kipl::strings::bool2string(bUseROI)<<"</useroi>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<roi>"<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<" "<<"</roi>"<<std::endl;
    str<<setw(indent+4)  <<" "<<"<voxelsize>"<< fVoxelSize[0] << " "<< fVoxelSize[1] <<" " <<fVoxelSize[2] << " " << "</voxelsize>"<< std::endl;
//    str<<setw(indent+4)  <<" "<<"<usevoi>"<<kipl::strings::bool2string(bUseVOI)<<"</usevoi>"<< std::endl;
    str<<setw(indent+4)  <<" "<<"<voi>"<< voi[0]<<" "<<voi[1] << " " << voi[2] << " " << voi[3] << " " << voi[4] <<" " << voi[5] << "</voi>" << std::endl;
	str<<setw(indent)  <<" "<<"</matrix>"<<std::endl;

	return str.str();
}


RECONFRAMEWORKSHARED_EXPORT std::string enum2string(ReconConfig::cProjections::eImageType &it)
{
	std::string str;
	
	switch (it) {
		case ReconConfig::cProjections::ImageType_Projections : str="projections"; break;
		case ReconConfig::cProjections::ImageType_Sinograms : str="sinograms"; break;
		case ReconConfig::cProjections::ImageType_Proj_RepeatProjection : str="proj_repeatprojection"; break;
		case ReconConfig::cProjections::ImageType_Proj_RepeatSinogram : str="proj_repeatsinogram"; break;
		default : throw ReconException("Unknown image type encountered in enum2string(imagetype)", __FILE__,__LINE__);
    }

	return str;
}

/// Converts a string to a beam geometry enum
/// \param str The string to convert
/// \param st an image type variable
void string2enum(const std::string str, ReconConfig::cProjections::eBeamGeometry &bg)
{
    std::map<std::string,ReconConfig::cProjections::eBeamGeometry> nameconv;

    nameconv["parallel"] = ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Parallel;
    nameconv["cone"]     = ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Cone;
    nameconv["helix"]    = ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Helix;

    std::string tmpstr=kipl::strings::toLower(str);


    if (nameconv.count(tmpstr)==0)
        throw ReconException("The key string does not exist for eBeamGeometry",__FILE__,__LINE__);

    bg=nameconv[tmpstr];
}

/// Converts a beam geometry enum to a string
/// \param str an image type variable
/// \returns The converted string
std::string enum2string(ReconConfig::cProjections::eBeamGeometry &bg)
{
    std::string str;

    switch (bg) {
    case ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Parallel : str="parallel"; break;
    case ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Cone     : str="cone"; break;
    case ReconConfig::cProjections::eBeamGeometry::BeamGeometry_Helix    : str="helix"; break;
    default                    : throw ReconException("Could not convert the beam geometry enum value to a string", __FILE__,__LINE__);
    }

    return str;
}

/// Writes the enum to a stream
/// \param s the target stream
/// \param st a scan type variable
/// \returns The updated stream
std::ostream & operator<<(std::ostream &s, ReconConfig::cProjections::eBeamGeometry bg)
{
    s<<enum2string(bg);

    return s;
}

std::string enum2string(const ReconConfig::cProjections::eScanType &st)
{
    std::string s;
    switch (st) {
        case ReconConfig::cProjections::SequentialScan    :    s="sequential"; break;
        case ReconConfig::cProjections::GoldenSectionScan :    s="goldensection"; break;
        case ReconConfig::cProjections::InvGoldenSectionScan : s="invgoldensection"; break;
        default : throw ReconException("Unknown scan type encountered in operator<<", __FILE__,__LINE__);
    };

    return s;
}

void string2enum(const std::string &str, ReconConfig::cProjections::eSkipListMode &mode)
{
    std::map<std::string,ReconConfig::cProjections::eSkipListMode> modes;

    modes["none"] = ReconConfig::cProjections::SkipMode_None;
    modes["skip"] = ReconConfig::cProjections::SkipMode_Skip;
    modes["drop"] = ReconConfig::cProjections::SkipMode_Drop;

    std::string tmpstr=kipl::strings::toLower(str);

    if (modes.count(tmpstr)==0)
        throw ReconException("The key string does not exist for eSkipMode",__FILE__,__LINE__);

    mode=modes[tmpstr];

}

std::string enum2string(const ReconConfig::cProjections::eSkipListMode &mode)
{
   std::string s;
    switch (mode)
    {
        case ReconConfig::cProjections::SkipMode_None: s="none"; break;
        case ReconConfig::cProjections::SkipMode_Skip: s="skip"; break;
        case ReconConfig::cProjections::SkipMode_Drop: s="drop"; break;
        default : throw ReconException("Unknown skip mode encountered in enum2string", __FILE__,__LINE__);
    }

    return s;
}

std::ostream & operator<<(std::ostream &s, const ReconConfig::cProjections::eSkipListMode &mode)
{
    s<<enum2string(mode);
 
    return s;
}
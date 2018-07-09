//<LICENSE>

#include "niqaconfig.h"

#include <iomanip>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/KiplException.h>

NIQAConfig::NIQAConfig() :
    logger("NIQAConfig")
{

}

NIQAConfig::NIQAConfig(NIQAConfig &c) :
    logger("NIQAConfig"),
    userInformation(c.userInformation),
    contrastAnalysis(c.contrastAnalysis),
    edgeAnalysis2D(c.edgeAnalysis2D),
    edgeAnalysis3D(c.edgeAnalysis3D),
    ballPackingAnalysis(c.ballPackingAnalysis)
{
}

const NIQAConfig & NIQAConfig::operator=(const NIQAConfig &c)
{
    userInformation     = c.userInformation;
    contrastAnalysis    = c.contrastAnalysis;
    edgeAnalysis2D      = c.edgeAnalysis2D;
    edgeAnalysis3D      = c.edgeAnalysis3D;
    ballPackingAnalysis = c.ballPackingAnalysis;
}

std::string NIQAConfig::WriteXML()
{
    std::ostringstream str;

    str<<"<niqa>\n";
    str<<userInformation.WriteXML(4);
    str<<contrastAnalysis.WriteXML(4);
    str<<edgeAnalysis2D.WriteXML(4);
    str<<edgeAnalysis3D.WriteXML(4);
    str<<ballPackingAnalysis.WriteXML(4);
    str<<"</niqa>\n";

    return str.str();
}

void NIQAConfig::LoadConfigFile(std::string configfile, std::string ProjectName)
{

}

void NIQAConfig::parseConfig(xmlTextReaderPtr reader, std::string sName)
{

    if (sName=="userinformation")
        parseUserInformation(reader);

    if (sName=="contrastanalysis")
        ParseContrastAnalysis(reader);

    if (sName=="edgeanalysis2d")
        parseEdgeAnalysis2D(reader);

    if (sName=="edgeanalysis3d")
        parseEdgeAnalysis3D(reader);

    if (sName=="ballpacking")
        parseBallPackingAnalysis(reader);
}

void NIQAConfig::parseUserInformation(xmlTextReaderPtr reader)
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
                logger.error("Unexpected contents in parameter file");
                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="operator") {
                userInformation.userName=sValue;
            }

            if (sName=="institute") {
                userInformation.institute=sValue;
            }

            if (sName=="instrument") {
                userInformation.instrument=sValue;
            }
            if (sName=="country") {
                userInformation.country=sValue;
            }
            if (sName=="experimentdate") {
                kipl::strings::String2Array(std::string(sValue),userInformation.experimentDate,3);
            }

            if (sName=="analysisdate") {
                kipl::strings::String2Array(std::string(sValue),userInformation.analysisDate,3);
            }

            if (sName=="version") {
                if (sValue!=std::string(VERSION))
                    logger.warning("Software version missmatch");
                userInformation.softwareVersion=VERSION;
            }

            if (sName=="comment") {
                userInformation.comment=sValue;
            }
        }
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }
}

void NIQAConfig::ParseContrastAnalysis(xmlTextReaderPtr reader)
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
                logger.error("Unexpected contents in parameter file");
                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="filemask") {
                contrastAnalysis.fileMask=sValue;
            }

            if (sName=="first") {
                contrastAnalysis.first=std::stoi(sValue);
            }

            if (sName=="last") {
                contrastAnalysis.last=std::stoi(sValue);
            }

            if (sName=="step") {
                contrastAnalysis.step=std::stoi(sValue);
            }

            if (sName=="pixelsize") {
                contrastAnalysis.pixelSize= std::stod(sValue);
            }

            if (sName=="intensityslope") {
                contrastAnalysis.intensitySlope= std::stod(sValue);
            }

            if (sName=="intensityintercept") {
                contrastAnalysis.intensityIntercept = std::stod(sValue);
            }

            if (sName=="makereport") {
                contrastAnalysis.makeReport = kipl::strings::string2bool(sValue);
            }

        }
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }
}

void NIQAConfig::parseEdgeAnalysis2D(xmlTextReaderPtr reader)
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
                logger.error("Unexpected contents in parameter file");
                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="singlefilename") {
                edgeAnalysis2D.singleImageFileName=sValue;
            }

            if (sName=="multiimagelist"){
                std::list<std::string> sl;
                kipl::strings::String2List(sValue,sl);
                for (auto it=sl.begin(); it!=sl.end(); ++it) {
                    float pos=std::stof(*it); ++it;
                    std::string &fname=*it;
                    edgeAnalysis2D.multiImageList.insert(std::make_pair(pos,fname));
                }
            }

            if (sName=="first") {
                edgeAnalysis2D.first=std::stoi(sValue);
            }

            if (sName=="last") {
                edgeAnalysis2D.last=std::stoi(sValue);
            }

            if (sName=="step") {
                edgeAnalysis2D.step=std::stoi(sValue);
            }

            if (sName=="obmask") {
                edgeAnalysis2D.obMask=sValue;
            }

            if (sName=="obfirst") {
                edgeAnalysis2D.obFirst=std::stoi(sValue);
            }

            if (sName=="oblast") {
                edgeAnalysis2D.obLast=std::stoi(sValue);
            }

            if (sName=="obstep") {
                edgeAnalysis2D.obStep=std::stoi(sValue);
            }

            if (sName=="dcmask") {
                edgeAnalysis2D.dcMask=sValue;
            }

            if (sName=="dcfirst") {
                edgeAnalysis2D.obFirst=std::stoi(sValue);
            }

            if (sName=="dclast") {
                edgeAnalysis2D.obLast=std::stoi(sValue);
            }

            if (sName=="dcstep") {
                edgeAnalysis2D.obStep=std::stoi(sValue);
            }


            if (sName=="pixelsize") {
                edgeAnalysis2D.pixelSize= std::stod(sValue);
            }

            if (sName=="normalize") {
                edgeAnalysis2D.normalize=kipl::strings::string2bool(sValue);
            }

            if (sName=="useroi") {
                edgeAnalysis2D.useROI=kipl::strings::string2bool(sValue);
            }

            if (sName=="roi") {
                edgeAnalysis2D.roi.fromString(sValue);
            }

            if (sName=="makereport") {
                edgeAnalysis2D.makeReport=kipl::strings::string2bool(sValue);
            }
        }

        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }

}

void NIQAConfig::parseEdgeAnalysis3D(xmlTextReaderPtr reader)
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
                logger.error("Unexpected contents in parameter file");
                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="filemask") {
                edgeAnalysis3D.fileMask=sValue;
            }

            if (sName=="first") {
                edgeAnalysis3D.first=std::stoi(sValue);
            }

            if (sName=="last") {
                edgeAnalysis3D.last=std::stoi(sValue);
            }

            if (sName=="step") {
                edgeAnalysis3D.step=std::stoi(sValue);
            }

            if (sName=="pixelsize") {
                edgeAnalysis3D.pixelSize= std::stod(sValue);
            }

            if (sName=="precision") {
                edgeAnalysis3D.precision = std::stod(sValue);
            }

            if (sName=="makereport") {
                edgeAnalysis2D.makeReport=kipl::strings::string2bool(sValue);
            }
        }

        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }
}

void NIQAConfig::parseBallPackingAnalysis(xmlTextReaderPtr reader)
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
                logger.error("Unexpected contents in parameter file");
                throw kipl::base::KiplException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="filemask") {
                ballPackingAnalysis.fileMask=sValue;
            }

            if (sName=="first") {
                ballPackingAnalysis.first=std::stoi(sValue);
            }

            if (sName=="last") {
                ballPackingAnalysis.last=std::stoi(sValue);
            }

            if (sName=="step") {
                ballPackingAnalysis.step=std::stoi(sValue);
            }

            if (sName=="usecrop") {
                ballPackingAnalysis.useCrop = kipl::strings::string2bool(sValue);
            }

            if (sName=="crop") {
                ballPackingAnalysis.roi.fromString(sValue);
            }

            if (sName=="roilist") {
                std::list<std::string> strlist;
                // todo roi list
                //kipl::strings::String2List(sValue,strlist);

                ballPackingAnalysis.analysisROIs.clear();

                if ((strlist.size() % 4 != 0) || (strlist.empty()==true)){
                    logger.error("Incomplete roi list");
                    throw kipl::base::KiplException("Incomplete ROI list",__FILE__,__LINE__);
                }

                for (auto it=strlist.begin(); it!=strlist.end(); ) {
                    kipl::base::RectROI roi(std::stoul(*it),std::stoul(*(++it)),std::stoul(*(++it)),std::stoul(*(++it)));
                    ballPackingAnalysis.analysisROIs.push_back(roi);
                }

            }
            if (sName=="makereport") {
                contrastAnalysis.makeReport = kipl::strings::string2bool(sValue);
            }

        }
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }

    //        str<<std::setw(indent+4)  <<" "<<"<roilist>";
    //        for (auto it=analysisROIs.begin(); it!=analysisROIs.end(); ++it)
    //            str<<" "<<it->toString();
    //        str<<"</roilist>\n";
}

NIQAConfig::UserInformation::UserInformation() :
        logger("UserInformation"),
        userName("John Doe"),
        institute("Institute"),
        instrument("beamline"),
        country("Universe"),
        softwareVersion(VERSION)
{
    experimentDate[0]=2018;
    experimentDate[1]=7;
    experimentDate[2]=2;

    analysisDate[0]=2018;
    analysisDate[1]=7;
    analysisDate[2]=2;

}

NIQAConfig::UserInformation::UserInformation(const NIQAConfig::UserInformation &c) :
    logger("UserInformation"),
    userName(c.userName),
    institute(c.institute),
    instrument(c.instrument),
    country(c.country),
    softwareVersion(c.softwareVersion)
{
    std::copy_n(c.experimentDate,3,experimentDate);
    std::copy_n(c.analysisDate,3,analysisDate);
}

const NIQAConfig::UserInformation & NIQAConfig::UserInformation::operator=(const NIQAConfig::UserInformation &c)
{
    userName        = c.userName;
    institute       = c.institute;
    instrument      = c.instrument;
    country         = c.country;
    std::copy_n(c.experimentDate,3,experimentDate);
    std::copy_n(c.analysisDate,3,analysisDate);
    softwareVersion = c.softwareVersion;

    return *this;
}

std::string NIQAConfig::UserInformation::WriteXML(size_t indent)
{
    using namespace std;
    ostringstream str;

    str<<setw(indent)  <<" "<<"<userinformation>"<<endl;
        str<<setw(indent+4)  <<" "<<"<operator>"<<userName<<"</operator>\n";
        str<<setw(indent+4)  <<" "<<"<institute>"<<institute<<"</institute>\n";
        str<<setw(indent+4)  <<" "<<"<instrument>"<<instrument<<"</instrument>\n";
        str<<setw(indent+4)  <<" "<<"<country>"<<country<<"</country>\n";
        str<<setw(indent+4)  <<" "<<"<experimentdate>"<<experimentDate[0]<<" "<<experimentDate[1]<<" "<<experimentDate[2]<<" "<<"</experimentdate>\n";
        str<<setw(indent+4)  <<" "<<"<analysisdate>"<<analysisDate[0]<<" "<<analysisDate[1]<<" "<<analysisDate[2]<<" "<<"</analysisdate>\n";
        str<<setw(indent+4)  <<" "<<"<version>"<<softwareVersion<<"</version>\n";
        str<<setw(indent+4)  <<" "<<"<comment>"<<comment<<"</comment>\n";
    str<<setw(indent)  <<" "<<"</userinformation>"<<endl;

    return str.str();
}


NIQAConfig::ContrastAnalysis::ContrastAnalysis() :
    logger("ContrastAnalysisConfig"),
    fileMask("img_####.fits"),
    first(0),
    last(1),
    step(1),
    pixelSize(0.1),
    intensitySlope(1.0),
    intensityIntercept(0.0),
    makeReport(false)
{

}

NIQAConfig::ContrastAnalysis::ContrastAnalysis(const ContrastAnalysis & c) :
    logger("ContrastAnalysisConfig"),
    fileMask(c.fileMask),
    first(c.first),
    last(c.last),
    step(c.step),
    pixelSize(c.pixelSize),
    intensitySlope(c.intensitySlope),
    intensityIntercept(c.intensityIntercept),
    makeReport(c.makeReport)
{

}

const NIQAConfig::ContrastAnalysis & NIQAConfig::ContrastAnalysis::operator=(const NIQAConfig::ContrastAnalysis &c)
{
        fileMask=c.fileMask;
        first=c.first;
        last=c.last;
        step=c.step;
        pixelSize=c.pixelSize;
        intensitySlope=c.intensitySlope;
        intensityIntercept=c.intensityIntercept;
        makeReport=c.makeReport;

        return *this;
}

std::string NIQAConfig::ContrastAnalysis::WriteXML(size_t indent)
{
    std::ostringstream str;

    str<<std::setw(indent)  <<" "<<"<contrastanalysis>"<<std::endl;
        str<<std::setw(indent+4)  <<" "<<"<filemask>"<<fileMask<<"</filemask>\n";
        str<<std::setw(indent+4)  <<" "<<"<first>"<<first<<"</first>\n";
        str<<std::setw(indent+4)  <<" "<<"<last>"<<last<<"</last>\n";
        str<<std::setw(indent+4)  <<" "<<"<step>"<<step<<"</step>\n";
        str<<std::setw(indent+4)  <<" "<<"<pixelsize>"<<kipl::strings::value2string(pixelSize)<<"</pixelsize>\n";
        str<<std::setw(indent+4)  <<" "<<"<intensityslope>"<<kipl::strings::value2string(intensitySlope)<<"</intensityslope>\n";
        str<<std::setw(indent+4)  <<" "<<"<intensityintercept>"<<kipl::strings::value2string(intensityIntercept)<<"</intensityintercept>\n";
        str<<std::setw(indent+4)  <<" "<<"<makereport>"<<kipl::strings::bool2string(makeReport)<<"</makereport>\n";
    str<<std::setw(indent)  <<" "<<"</contrastanalysis>"<<std::endl;

    return str.str();
}

NIQAConfig::EdgeAnalysis2D::EdgeAnalysis2D() :
    logger("EdgeAnalysisConfig"),
    singleImageFileName("image.fits"),
    first(0),
    last(1),
    step(1),
    normalize(false),
    obMask("ob_#####.fits"),
    obFirst(0),
    obLast(4),
    obStep(1),
    dcMask("dc_#####.fits"),
    dcFirst(0),
    dcLast(1),
    dcStep(1),
    useROI(false),
    roi(0,0,1,1),
    pixelSize(0.1),
    makeReport(false)
{

}

NIQAConfig::EdgeAnalysis2D::EdgeAnalysis2D(const NIQAConfig::EdgeAnalysis2D &e) :
                   logger("EdgeAnalysisConfig"),
                   singleImageFileName(e.singleImageFileName),
                   multiImageList(e.multiImageList),
                   first(e.first),
                   last(e.last),
                   step(e.step),
                   normalize(e.normalize),
                   obMask(e.obMask),
                   obFirst(e.obFirst),
                   obLast(e.obLast),
                   obStep(e.obStep),
                   dcMask(e.dcMask),
                   dcFirst(e.dcFirst),
                   dcLast(e.dcLast),
                   dcStep(e.dcStep),
                   useROI(e.useROI),
                   roi(e.roi),
                   pixelSize(e.pixelSize),
                   makeReport(e.makeReport)
{
}

const NIQAConfig::EdgeAnalysis2D & NIQAConfig::EdgeAnalysis2D::operator=(const NIQAConfig::EdgeAnalysis2D &e)
{
    singleImageFileName = e.singleImageFileName;
    multiImageList = e.multiImageList;
    first     = e.first;
    last      = e.last;
    step      = e.step;
    normalize = e.normalize;
    obMask    = e.obMask;
    obFirst   = e.obFirst;
    obLast    = e.obLast;
    obStep    = e.obStep;
    dcMask    = e.dcMask;
    dcFirst   = e.dcFirst;
    dcLast    = e.dcLast;
    dcStep    = e.dcStep;
    useROI    = e.useROI;
    roi       = e.roi;
    pixelSize = e.pixelSize;
    makeReport= e.makeReport;

    return *this;
}

std::string NIQAConfig::EdgeAnalysis2D::WriteXML(size_t indent)
{
    std::ostringstream str;

    str<<std::setw(indent)  <<" "<<"<edgeanalysis2d>"<<std::endl;
        str<<std::setw(indent+4)  <<" "<<"<singlefilename>"<<singleImageFileName<<"</singlefilename>\n";
        if (multiImageList.empty()==false) {
            str<<std::setw(indent+4)  <<" "<<"<multiimagelist>";
            for (auto it=multiImageList.begin(); it!=multiImageList.end(); ++it)
                str<<" "<<it->first<<" "<<it->second;

            str<<"</multiimagelist>\n";
        }
        str<<std::setw(indent+4)  <<" "<<"<first>"<<first<<"</first>\n";
        str<<std::setw(indent+4)  <<" "<<"<last>"<<last<<"</last>\n";
        str<<std::setw(indent+4)  <<" "<<"<step>"<<step<<"</step>\n";
        str<<std::setw(indent+4)  <<" "<<"<normalize>"<<kipl::strings::bool2string(normalize)<<"</normalize>\n";
        str<<std::setw(indent+4)  <<" "<<"<obmask>"<<obMask<<"</obmask>\n";
        str<<std::setw(indent+4)  <<" "<<"<obfirst>"<<obFirst<<"</obfirst>\n";
        str<<std::setw(indent+4)  <<" "<<"<oblast>"<<obLast<<"</oblast>\n";
        str<<std::setw(indent+4)  <<" "<<"<obstep>"<<obStep<<"</obstep>\n";
        str<<std::setw(indent+4)  <<" "<<"<dcmask>"<<dcMask<<"</dcmask>\n";
        str<<std::setw(indent+4)  <<" "<<"<dcfirst>"<<dcFirst<<"</dcfirst>\n";
        str<<std::setw(indent+4)  <<" "<<"<dclast>"<<dcLast<<"</dclast>\n";
        str<<std::setw(indent+4)  <<" "<<"<dcstep>"<<dcStep<<"</dcstep>\n";
        str<<std::setw(indent+4)  <<" "<<"<useroi>"<<kipl::strings::bool2string(useROI)<<"</useroi>\n";
        str<<std::setw(indent+4)  <<" "<<"<roi>"<<roi.toString()<<"</roi>\n";
        str<<std::setw(indent+4)  <<" "<<"<pixelsize>"<<kipl::strings::value2string(pixelSize)<<"</pixelsize>\n";
        str<<std::setw(indent+4)  <<" "<<"<makereport>"<<kipl::strings::bool2string(makeReport)<<"</makereport>\n";
    str<<std::setw(indent)  <<" "<<"</edgeanalysis2d>"<<std::endl;

    return str.str();
}

NIQAConfig::EdgeAnalysis3D::EdgeAnalysis3D() :
    logger("EdgeAnalysis3DConfig"),
    fileMask("slice_#####.tif"),
    first(0),
    last(1),
    step(1),
    pixelSize(0.1),
    radius(10.0),
    precision(0.1),
    makeReport(false)
{

}

NIQAConfig::EdgeAnalysis3D::EdgeAnalysis3D(const NIQAConfig::EdgeAnalysis3D &e):
    logger("EdgeAnalysis3DConfig"),
    fileMask(e.fileMask),
    first(e.first),
    last(e.last),
    step(e.step),
    pixelSize(e.pixelSize),
    radius(e.radius),
    precision(e.precision),
    makeReport(e.makeReport)
{

}


const NIQAConfig::EdgeAnalysis3D & NIQAConfig::EdgeAnalysis3D::operator=(const NIQAConfig::EdgeAnalysis3D &e)
{
    fileMask=e.fileMask;
    first=e.first;
    last=e.last;
    step=e.step;
    pixelSize=e.pixelSize;
    radius=e.radius;
    precision=e.precision;
    makeReport=e.makeReport;

    return *this;
}

std::string NIQAConfig::EdgeAnalysis3D::WriteXML(size_t indent)
{
    std::ostringstream str;

    str<<std::setw(indent)  <<" "<<"<edgeanalysis3d>"<<std::endl;
        str<<std::setw(indent+4)  <<" "<<"<filemask>"<<fileMask<<"</filemask>\n";
        str<<std::setw(indent+4)  <<" "<<"<first>"<<first<<"</first>\n";
        str<<std::setw(indent+4)  <<" "<<"<last>"<<last<<"</last>\n";
        str<<std::setw(indent+4)  <<" "<<"<step>"<<step<<"</step>\n";
        str<<std::setw(indent+4)  <<" "<<"<pixelsize>"<<kipl::strings::value2string(pixelSize)<<"</pixelsize>\n";
        str<<std::setw(indent+4)  <<" "<<"<radius>"<<kipl::strings::value2string(radius)<<"</radius>\n";
        str<<std::setw(indent+4)  <<" "<<"<precision>"<<kipl::strings::value2string(precision)<<"</precision>\n";
        str<<std::setw(indent+4)  <<" "<<"<makereport>"<<kipl::strings::bool2string(makeReport)<<"</makereport>\n";
    str<<std::setw(indent)  <<" "<<"</edgeanalysis3d>"<<std::endl;

    return str.str();
}


NIQAConfig::BallPackingAnalysis::BallPackingAnalysis() :
    logger("BallPackingAnalysisConfig"),
    fileMask("slice_#####.tif"),
    first(0),
    last(1),
    step(1),
    useCrop(false),
    roi(0,0,1,1),
    makeReport(false)
{}

NIQAConfig::BallPackingAnalysis::BallPackingAnalysis(const NIQAConfig::BallPackingAnalysis &c) :
    logger("BallPackingAnalysisConfig"),
    fileMask(c.fileMask),
    first(c.first),
    last(c.last),
    step(c.step),
    useCrop(c.useCrop),
    roi(c.roi),
    makeReport(c.makeReport)
{
    std::copy(c.analysisROIs.begin(),c.analysisROIs.end(),analysisROIs.begin());
}

const NIQAConfig::BallPackingAnalysis & NIQAConfig::BallPackingAnalysis::operator =(const NIQAConfig::BallPackingAnalysis & c)
{
    fileMask=c.fileMask;
    first=c.first;
    last=c.last;
    step=c.step;
    useCrop=c.useCrop;
    roi=c.roi;
    makeReport=c.makeReport;

    std::copy(c.analysisROIs.begin(),c.analysisROIs.end(),analysisROIs.begin());
    return *this;
}

std::string NIQAConfig::BallPackingAnalysis::WriteXML(size_t indent)
{
    std::ostringstream str;

    str<<std::setw(indent)  <<" "<<"<ballpacking>"<<std::endl;
        str<<std::setw(indent+4)  <<" "<<"<filemask>"<<fileMask<<"</filemask>\n";
        str<<std::setw(indent+4)  <<" "<<"<first>"<<first<<"</first>\n";
        str<<std::setw(indent+4)  <<" "<<"<last>"<<last<<"</last>\n";
        str<<std::setw(indent+4)  <<" "<<"<step>"<<step<<"</step>\n";
        str<<std::setw(indent+4)  <<" "<<"<usecrop>"<<kipl::strings::bool2string(useCrop)<<"</usecrop>\n";
        str<<std::setw(indent+4)  <<" "<<"<crop>"<<roi.toString()<<"</crop>\n";
        str<<std::setw(indent+4)  <<" "<<"<roilist>";
        for (auto it=analysisROIs.begin(); it!=analysisROIs.end(); ++it)
            str<<" "<<it->toString();
        str<<"</roilist>\n";
        str<<std::setw(indent+4)  <<" "<<"<makereport>"<<kipl::strings::bool2string(makeReport)<<"</makereport>\n";
    str<<std::setw(indent)  <<" "<<"</ballpacking>"<<std::endl;

    return str.str();
}


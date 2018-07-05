#include "niqaconfig.h"

#include <strings/miscstring.h>

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
    userInformation.WriteXML(4);
    contrastAnalysis.WriteXML(4);
    edgeAnalysis2D.WriteXML(4);
    edgeAnalysis3D.WriteXML(4);
    ballPackingAnalysis.WriteXML(4);

}

void NIQAConfig::LoadConfigFile(std::string configfile, std::string ProjectName)
{

}

void NIQAConfig::ParseConfig(xmlTextReaderPtr reader, std::string sName)
{

//    str<<setw(indent)  <<" "<<"<userinformation>"<<endl;
//        str<<setw(indent+4)  <<" "<<"<operator>"<<userName<<"</operator>\n";
//        str<<setw(indent+4)  <<" "<<"<institute>"<<institute<<"</institute>\n";
//        str<<setw(indent+4)  <<" "<<"<instrument>"<<instrument<<"</instrument>\n";
//        str<<setw(indent+4)  <<" "<<"<country>"<<country<<"</country>\n";
//        str<<setw(indent+4)  <<" "<<"<experimentdate>"<<experimentDate[0]<<" "<<experimentDate[1]<<" "<<experimentDate[2]<<" "<<"</experimentdate>\n";
//        str<<setw(indent+4)  <<" "<<"<analysisdate>"<<analysisDate[0]<<" "<<analysisDate[1]<<" "<<analysisDate[2]<<" "<<"</analysisdate>\n";
//        str<<setw(indent+4)  <<" "<<"<version>"<<softwareVersion<<"</version>\n";
//        str<<setw(indent+4)  <<" "<<"<comment>"<<comment<<"</comment>\n";
//    str<<setw(indent)  <<" "<<"</userinformation>"<<endl;

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
//    std::copy(e.multiImageList.begin(),e.multiImageList.end(),multiImageList.begin());
}

const NIQAConfig::EdgeAnalysis2D & NIQAConfig::EdgeAnalysis2D::operator=(const NIQAConfig::EdgeAnalysis2D &e)
{
    singleImageFileName = e.singleImageFileName;
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

  //  std::copy(e.multiImageList.begin(),e.multiImageList.end(),multiImageList.begin());

    return *this;
}

std::string NIQAConfig::EdgeAnalysis2D::WriteXML(size_t indent)
{

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
//    std::string fileMask;
//    int first;
//    int last;
//    int step;
//    bool useCrop;
//    kipl::base::RectROI roi;
//    std::list<kipl::base::RectROI> analysisROIs;
//    bool makeReport;
    str<<setw(indent)  <<" "<<"<ballpacking>"<<endl;
        str<<setw(indent+4)  <<" "<<"<filemask>"<<fileMask<<"</filemask>\n";
        str<<setw(indent+4)  <<" "<<"<first>"<<first<<"</first>\n";
        str<<setw(indent+4)  <<" "<<"<last>"<<last<<"</last>\n";
        str<<setw(indent+4)  <<" "<<"<step>"<<step<<"</step>\n";
        str<<setw(indent+4)  <<" "<<"<usecrop>"<<experimentDate[0]<<" "<<experimentDate[1]<<" "<<experimentDate[2]<<" "<<"</experimentdate>\n";
        str<<setw(indent+4)  <<" "<<"<analysisdate>"<<analysisDate[0]<<" "<<analysisDate[1]<<" "<<analysisDate[2]<<" "<<"</analysisdate>\n";
        str<<setw(indent+4)  <<" "<<"<version>"<<softwareVersion<<"</version>\n";
        str<<setw(indent+4)  <<" "<<"<report>"<<kipl::strings::bool2string(makeReport)<<"</report>\n";
    str<<setw(indent)  <<" "<<"</ballpacking>"<<endl;
}


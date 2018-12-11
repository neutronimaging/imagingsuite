#include <string>
#include <sstream>
#include <regex>

#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/kiplenums.h>

#include "datasetbase.h"

int ImageLoader::cnt=0;

ImageLoader::ImageLoader() :
    id(cnt++),
    m_sFilemask("image_####.tif"),
    m_sVariableName("image"),
    m_nFirst(0),
    m_nLast(99),
    m_nRepeat(1),
    m_nStride(1),
    m_nStep(1),
    m_Flip(kipl::base::ImageFlipNone),
    m_Rotate(kipl::base::ImageRotateNone),
    m_bUseROI(false)
{
    m_ROI[0]=0;
    m_ROI[1]=0;
    m_ROI[2]=100;
    m_ROI[3]=100;
}

ImageLoader::ImageLoader(const ImageLoader &cfg) :
    id(cnt++),
    m_sFilemask(cfg.m_sFilemask),
    m_sVariableName(cfg.m_sVariableName),
    m_nFirst(cfg.m_nFirst),
    m_nLast(cfg.m_nLast),
    m_nRepeat(cfg.m_nRepeat),
    m_nStride(cfg.m_nStride),
    m_nStep(cfg.m_nStep),
    m_Flip(cfg.m_Flip),
    m_Rotate(cfg.m_Rotate),
    m_bUseROI(cfg.m_bUseROI)
{
    std::copy_n(cfg.m_ROI,4,m_ROI);
}

ImageLoader::~ImageLoader()
{

}

const ImageLoader &ImageLoader::operator=(const ImageLoader &cfg)
{
    m_sFilemask=cfg.m_sFilemask;
    m_sVariableName=cfg.m_sVariableName;
    m_nFirst=cfg.m_nFirst;
    m_nLast=cfg.m_nLast;
    m_nRepeat=cfg.m_nRepeat;
    m_nStride=cfg.m_nStride;
    m_nStep=cfg.m_nStep;
    m_Flip=cfg.m_Flip;
    m_Rotate=cfg.m_Rotate;
    m_bUseROI=cfg.m_bUseROI;

    std::copy_n(cfg.m_ROI,4,m_ROI);

    return *this;
}

int ImageLoader::getId() {
    return id;
}

std::string ImageLoader::WriteXML(int indent)
{
    ostringstream xml;

    xml<<std::setw(indent+4)<<"<filemask>"     << m_sFilemask     << "</filemask>\n";
    xml<<std::setw(indent+4)<<"<variablename>" << m_sVariableName << "</variablename>\n";
    xml<<std::setw(indent+4)<<"<first>"        << m_nFirst        << "</first>\n";
    xml<<std::setw(indent+4)<<"<last>"         << m_nLast         << "</last>\n";
    xml<<std::setw(indent+4)<<"<repeat>"       << m_nRepeat       << "</repeat>\n";
    xml<<std::setw(indent+4)<<"<stride>"       << m_nStride       << "</stride>\n";
    xml<<std::setw(indent+4)<<"<step>"         << m_nStep         << "</step>\n";
    xml<<std::setw(indent+4)<<"<flip>"         << enum2string(m_Flip)   <<"</flip>\n";
    xml<<std::setw(indent+4)<<"<rotate>"       << enum2string(m_Rotate) <<"</flip>\n";
    xml<<std::setw(indent+4)<<"<useroi>"       << kipl::strings::bool2string(m_bUseROI)<<"</useroi>\n";
    xml<<std::setw(indent+4)<<"<roi>"<< m_ROI[0]<<" "<< m_ROI[1]<<" "<< m_ROI[2]<<" "<< m_ROI[3]<<" "<<"</roi>\n";

    xml<<std::setw(indent+4)<<"<skiplist>"     << kipl::strings::List2String(m_nSkipList)     << "</skiplist>\n";

    return xml.str();
}

int ImageLoader::ParseXML(std::string xml)
{
    regex reg("<(.*)>(.*)</(\\1)>");

     auto pos=xml.cbegin();
     auto end=xml.cend();

     smatch m;
     std::string match;
     std::string tag;
     std::string value;
     for (; regex_search(pos,end,m,reg); pos=m.suffix().first) {
         match=m.str();
         tag=m.str(1);
         value=m.str(2);

         if (tag=="filemask") {
             m_sFilemask=value;
         }
         if (tag=="variablename") {
             m_sVariableName=value;
         }
         if (tag=="first") {
            m_nFirst=std::atoi(value.c_str());
         }
         if (tag=="last") {
            m_nLast=std::atoi(value.c_str());
         }
         if (tag=="repeat") {
            m_nRepeat=std::atoi(value.c_str());
         }
         if (tag=="stride") {
            m_nStride=std::atoi(value.c_str());
         }
         if (tag=="step") {
            m_nStep=std::atoi(value.c_str());
         }

         if (tag=="flip") {
            string2enum(value.c_str(),m_Flip);
         }

         if (tag=="rotate") {
            string2enum(value.c_str(),m_Rotate);
         }

         if (tag=="useroi") {
            m_bUseROI=kipl::strings::string2bool(value.c_str());
         }
//         xml<<std::setw(indent+4)<<"<flip>"<< m_Flip<<"</flip>\n";
//         xml<<std::setw(indent+4)<<"<rotate>"<< m_Rotate<<"</flip>\n";
//         xml<<std::setw(indent+4)<<"<useroi>"<< kipl::strings::bool2string(m_bUseROI)<<"</useroi>\n";
//         xml<<std::setw(indent+4)<<"<roi>"<< m_ROI[0]<<" "<< m_ROI[1]<<" "<< m_ROI[2]<<" "<< m_ROI[3]<<" "<<"</roi>\n";
         if (tag=="skiplist") {
            kipl::strings::String2List(value,m_nSkipList);
         }
     }
     return 0;
}

std::ostream & operator<<(std::ostream &s, ImageLoader &il)
{
    s<<"FileMask:"<<il.m_sFilemask<<", variable name="<<il.m_sVariableName<<", interval ["<<il.m_nFirst<<", "<<il.m_nLast<<"]\n"
    << il.m_Flip<<" " << il.m_Rotate << "\n"
    << "Use roi "<<kipl::strings::bool2string(il.m_bUseROI)<<" ["<< il.m_ROI[0]<<" "<< il.m_ROI[1]<<" "<< il.m_ROI[2]<<" "<< il.m_ROI[3]<<"]\n";

    return s;
}

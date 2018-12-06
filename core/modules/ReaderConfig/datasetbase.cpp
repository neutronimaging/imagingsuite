#include <string>
#include <sstream>
#include <regex>

#include <strings/filenames.h>
#include <strings/string2array.h>

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

ImageLoader::~ImageLoader()
{

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
         if (tag=="skiplist") {
            kipl::strings::String2List(value,m_nSkipList);
         }
     }
     return 0;
}

std::ostream & operator<<(std::ostream &s, ImageLoader &il)
{
    s<<"FileMask:"<<il.m_sFilemask<<", interval ["<<il.m_nFirst<<", "<<il.m_nLast<<"]";

    return s;
}

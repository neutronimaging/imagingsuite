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
    m_nStep(1)

{
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

kipl::base::TImage<float,2> ImageLoader::Load(int idx,kipl::base::eImageRotate rot,kipl::base::eImageFlip flip, size_t *crop)
{
    kipl::base::TImage<float,2> img;

    std::string fname;
//    kipl::strings::filenames::MakeFileName()
    return img;
}

kipl::base::TImage<float,3> ImageLoader::Load(kipl::base::eImageRotate rot, kipl::base::eImageFlip flip, size_t *crop)
{
    kipl::base::TImage<float,3> volume;
    kipl::base::TImage<float,2> slice;

    size_t dims[3]={0,0,0};
    for (int i=m_nFirst; i<=m_nLast; i+=m_nStep) {\
        slice=Load(i,rot,flip,crop);
        if (i==m_nFirst) {
            dims[0]=slice.Size(0);
            dims[1]=slice.Size(1);
            dims[2]=(m_nLast-m_nFirst+1)/m_nStep;
            volume.Resize(dims);
        }
        memcpy(volume.GetLinePtr(i-m_nFirst),slice.GetDataPtr(),slice.Size()*sizeof(float));
    }

    return volume;
}

std::ostream & operator<<(std::ostream &s, ImageLoader &il)
{
    s<<"FileMask:"<<il.m_sFilemask<<", interval ["<<il.m_nFirst<<", "<<il.m_nLast<<"]";

    return s;
}

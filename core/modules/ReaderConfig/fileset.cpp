#include <string>
#include <sstream>
#include <regex>

#include <strings/filenames.h>
#include <strings/miscstring.h>
#include <strings/string2array.h>
#include <base/kiplenums.h>
#include <strings/xmlstrings.h>

#include "readerexception.h"
#include "fileset.h"

int FileSet::cnt=0;

FileSet::FileSet() :
    id(cnt++),
    m_sFilemask("image_####.tif"),
    m_sVariableName("image"),
    m_nFirst(0),
    m_nLast(99),
    m_nCount(1),
    m_nStride(1),
    m_nStep(1),
    m_fBinning(1.0f),
    m_Flip(kipl::base::ImageFlipNone),
    m_Rotate(kipl::base::ImageRotateNone),
    m_AverageMethod(ImagingAlgorithms::AverageImage::ImageAverage),
    m_bUseROI(false),
    m_ROI({0,0,100,100})
{
}

FileSet::FileSet(const FileSet &cfg) :
    id(cnt++),
    m_sFilemask(cfg.m_sFilemask),
    m_sVariableName(cfg.m_sVariableName),
    m_nFirst(cfg.m_nFirst),
    m_nLast(cfg.m_nLast),
    m_nCount(cfg.m_nCount),
    m_nStride(cfg.m_nStride),
    m_nStep(cfg.m_nStep),
    m_fBinning(cfg.m_fBinning),
    m_Flip(cfg.m_Flip),
    m_Rotate(cfg.m_Rotate),
    m_AverageMethod(cfg.m_AverageMethod),
    m_bUseROI(cfg.m_bUseROI),
    m_ROI(cfg.m_ROI)
{
}

FileSet::~FileSet()
{

}

const FileSet &FileSet::operator=(const FileSet &cfg)
{
    m_sFilemask     = cfg.m_sFilemask;
    m_sVariableName = cfg.m_sVariableName;
    m_nFirst        = cfg.m_nFirst;
    m_nLast         = cfg.m_nLast;
    m_nCount       = cfg.m_nCount;
    m_nStride       = cfg.m_nStride;
    m_nStep         = cfg.m_nStep;
    m_fBinning      = cfg.m_fBinning;
    m_Flip          = cfg.m_Flip;
    m_Rotate        = cfg.m_Rotate;
    m_AverageMethod = cfg.m_AverageMethod;
    m_bUseROI       = cfg.m_bUseROI;
    m_ROI           = cfg.m_ROI;

    return *this;
}

int FileSet::getId() {
    return id;
}

std::string FileSet::WriteXML(int indent)
{
    ostringstream xml;

    xml<<kipl::strings::xmlString("filemask",      m_sFilemask,                 indent+4);
    xml<<kipl::strings::xmlString("variablename",  m_sVariableName,             indent+4);
    xml<<kipl::strings::xmlString("first",         m_nFirst,                    indent+4);
    xml<<kipl::strings::xmlString("last",          m_nLast,                     indent+4);
    xml<<kipl::strings::xmlString("repeat",        m_nCount,                   indent+4);
    xml<<kipl::strings::xmlString("stride",        m_nStride,                   indent+4);
    xml<<kipl::strings::xmlString("step",          m_nStep,                     indent+4);
    xml<<kipl::strings::xmlString("binning",       m_fBinning,                  indent+4);
    xml<<kipl::strings::xmlString("flip",          enum2string(m_Flip),         indent+4);
    xml<<kipl::strings::xmlString("rotate",        enum2string(m_Rotate),       indent+4);
    xml<<kipl::strings::xmlString("averagemethod", enum2string(m_AverageMethod), indent+4);

    xml<<kipl::strings::xmlString("useroi",        m_bUseROI,                   indent+4);

    xml<<std::setw(indent+4)<<"<roi>"<< m_ROI[0]<<" "<< m_ROI[1]<<" "<< m_ROI[2]<<" "<< m_ROI[3]<<" "<<"</roi>\n";

    xml<<std::setw(indent+4)<<"<skiplist>"     << kipl::strings::List2String(m_nSkipList)     << "</skiplist>\n";

    return xml.str();
}

int FileSet::ParseXML(std::string xml)
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
            m_nCount=std::atoi(value.c_str());
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

         if (tag=="averagemethod") {
            string2enum(value.c_str(),m_AverageMethod);
         }

         if (tag=="useroi") {
            m_bUseROI=kipl::strings::string2bool(value.c_str());
         }

         if (tag=="skiplist") {
            kipl::strings::String2List(value,m_nSkipList);
         }
     }
     return 0;
}

int FileSet::ParseXML(xmlTextReaderPtr reader)
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
                throw ReaderException("Unexpected contents in parameter file",__FILE__,__LINE__);
            }
            if (value!=nullptr)
                sValue=reinterpret_cast<const char *>(value);
            else
                sValue="Empty";
            sName=reinterpret_cast<const char *>(name);

            if (sName=="filemask") {
                m_sFilemask=sValue;
            }
            if (sName=="variablename") {
                m_sVariableName=sValue;
            }
            if (sName=="first") {
               m_nFirst=std::atoi(sValue.c_str());
            }
            if (sName=="last") {
               m_nLast=std::atoi(sValue.c_str());
            }
            if (sName=="repeat") {
               m_nCount=std::atoi(sValue.c_str());
            }
            if (sName=="stride") {
               m_nStride=std::atoi(sValue.c_str());
            }
            if (sName=="step") {
               m_nStep=std::atoi(sValue.c_str());
            }

            if (sName=="flip") {
               string2enum(sValue.c_str(),m_Flip);
            }

            if (sName=="rotate") {
               string2enum(sValue.c_str(),m_Rotate);
            }

            if (sName=="averagemethod") {
               string2enum(sValue.c_str(),m_AverageMethod);
            }

            if (sName=="useroi") {
               m_bUseROI=kipl::strings::string2bool(sValue.c_str());
            }
        }
        ret = xmlTextReaderRead(reader);
        if (xmlTextReaderDepth(reader)<depth)
            ret=0;
    }

    return ret;
}

std::string FileSet::makeFileName(int idx) const
{
    std::string name;
    std::string ext;

    kipl::strings::filenames::MakeFileName(m_sFilemask,idx, name, ext,'#');

    return name;
}

int FileSet::lastIndex() const
{
    return (m_nLast - m_nFirst) / (m_nStep *m_nStride);
}

int FileSet::fileIndex(int idx, int cntIdx) const
{
    return m_nFirst + idx * m_nStep * m_nStride + cntIdx;
}


std::ostream & operator<<(std::ostream &s, FileSet &il)
{
    s<<"FileMask:"<<il.m_sFilemask<<", variable name="<<il.m_sVariableName<<", interval ["<<il.m_nFirst<<", "<<il.m_nLast<<"]\n"
    << il.m_Flip<<" " << il.m_Rotate << "\n"
    << "Use roi "<<kipl::strings::bool2string(il.m_bUseROI)<<" ["<< il.m_ROI[0]<<" "<< il.m_ROI[1]<<" "<< il.m_ROI[2]<<" "<< il.m_ROI[3]<<"]\n";

    return s;
}

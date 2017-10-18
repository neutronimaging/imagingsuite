#include <string>
#include <sstream>
#include <regex>

#include <strings/filenames.h>

#include "datasetbase.h"

ImageLoader::ImageLoader() :
    m_sFilemask("image_####.tif"),
    m_nFirst(0),
    m_nLast(99),
    m_nStep(1),
    m_nStride(1)
{

}

ImageLoader::~ImageLoader()
{

}

std::string ImageLoader::WriteXML(int indent)
{
    ostringstream xml;

    return xml.str();
}

int ImageLoader::ParseXML(std::string xml)
{
    regex reg("<(.*)>(.*)</(\\1)>");

     auto pos=xml.cbegin();
     auto end=xml.cend();

     smatch m;

     for (; regex_search(pos,end,m,reg); pos=m.suffix().first) {
         cout<<"match: "<< m.str()<<endl;
         cout<<"tag: "<< m.str(1)<<endl;
         cout<<"value: "<< m.str(2)<<endl;

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

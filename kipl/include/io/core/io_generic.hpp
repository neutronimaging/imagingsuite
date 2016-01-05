#ifndef IO_GENERIC_HPP
#define IO_GENERIC_HPP
#include <fstream>
#include <sstream>

#include "../../kipl_global.h"
#include "../../base/kiplenums.h"
#include "../../base/KiplException.h"

#include <list>
namespace kipl { namespace io {

template <typename ImgType>
int ReadGeneric(kipl::base::TImage<ImgType,2> &img,
                char const * const fname,
                size_t size_x,
                size_t size_y,
                size_t offset,
                size_t stride,
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t imageindex,
                size_t const * const nCrop)
{
    std::ifstream datfile(fname, ios_base::binary);

    kipl::base::TImage<unsigned short,2> tmp;

    ReadGeneric(datfile,tmp,size_x,size_y,offset,stride,imagesperfile,dt,endian, imageindex,nCrop);

    for (int i=0; i<img.Size(); i++)
        img[i]=static_cast<ImgType>(tmp[i]);

    return 0;
}

template <typename ImgType>
int ReadGeneric(std::list<kipl::base::TImage<ImgType,2> > &imglist,
                char const * const fname,
                size_t size_x,
                size_t size_y,
                size_t offset,
                size_t stride,
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t const * const nCrop)
{
    std::ifstream datfile(fname, ios_base::binary);

    if (!datfile.good()) {
        std::ostringstream msg;
        msg<<"ReadGeneric failed to open file: "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    kipl::base::TImage<unsigned short,2> tmp;
    kipl::base::TImage<ImgType,2> otmp;
    imglist.clear();

    for (size_t i=0; i<imagesperfile; i++) {
        ReadGeneric(datfile,tmp,size_x,size_y,offset,stride,imagesperfile,dt,endian,i,nCrop);

        otmp.Resize(tmp.Dims());
        for (size_t i=0; i<tmp.Size(); i++)
            otmp[i]=static_cast<ImgType>(tmp[i]);

        imglist.push_back(otmp);
    }

    return 0;
}

}}
#endif // IO_GENERIC_HPP


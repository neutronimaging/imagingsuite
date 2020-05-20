//<LICENCE>

#ifndef IO_GENERIC_HPP
#define IO_GENERIC_HPP
#include <fstream>
#include <sstream>

#include "../../kipl_global.h"
#include "../../base/kiplenums.h"
#include "../../base/KiplException.h"
#include "../../logging/logger.h"
#include "../../base/timage.h"

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

    if (!datfile.good()) {
        std::ostringstream msg;
        msg<<"ReadGeneric failed to open file: "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    ReadGeneric(datfile,img,size_x,size_y,offset,stride,imagesperfile,dt,endian, imageindex,nCrop);


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

    kipl::base::TImage<ImgType,2> tmp;
    imglist.clear();

    for (size_t i=0; i<imagesperfile; i++) {
        ReadGeneric(datfile,tmp,size_x,size_y,offset,stride,imagesperfile,dt,endian,i,nCrop);

        imglist.push_back(tmp);
    }

    return 0;
}

template <typename ImgType>
int  ReadGeneric(ifstream &file,kipl::base::TImage<ImgType,2> &img,
                size_t size_x,
                size_t size_y,
                size_t offset, //< Given in bytes
                size_t stride, //< Given in bytes
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t imageindex,
                size_t const * const nCrop)
{
    kipl::logging::Logger logger("ReadGeneric");
    std::ostringstream msg;
    if (imagesperfile<=imageindex) {
        throw kipl::base::KiplException("Tried to access a non-existing image",__FILE__,__LINE__);
    }

    size_t file_offset=imageindex*size_y*stride+offset;
    msg<<"Offset="<<file_offset<<", stride="<<stride;

    logger(logger.LogMessage,msg.str());

    file.seekg(file_offset);

    std::vector<size_t> dims={size_x, size_y};
    img.resize(dims);

    img=(unsigned short)0;
    converter data[3];

    char * buffer = new char[stride+(32-stride % 32)];

    unsigned short *shortbuffer = reinterpret_cast<unsigned short *>(buffer);
    float *floatbuffer = reinterpret_cast<float *>(buffer);

    for (size_t i=0; i<size_y; i++) {
        for (size_t j=0; j<stride; j++)
            buffer[j]=file.get();
        ImgType * pLine = img.GetLinePtr(i);
        switch (dt) {
        case kipl::base::UInt4 :
            logger(logger.LogMessage,"UInt4");
            for (size_t j=0, k=0; j<size_x; j+=2, k++) {
                  data[0].c=buffer[k];

                  pLine[j]   = static_cast<unsigned short>(data[0].nibbles.lo);
                  pLine[j+1] = static_cast<unsigned short>(data[0].nibbles.hi);
            }
            break;
        case kipl::base::UInt8 :
            logger(logger.LogMessage,"UInt8");
            for (size_t j=0; j<size_x; j++)
                pLine[j]=static_cast<unsigned char>(buffer[j]);
            break;
        case kipl::base::UInt12 :
            logger(logger.LogMessage,"UInt12");
            for (size_t j=0, k=0; j<size_x; j+=2, k+=3) {
                // Careful with signed unsigned...
                  data[0].c=buffer[k];
                  data[1].c=buffer[k+1];
                  data[2].c=buffer[k+2];

                  pLine[j]   = static_cast<unsigned short>(data[1].nibbles.hi + (data[0].b<<4));
                  pLine[j+1] = static_cast<unsigned short>(data[1].nibbles.lo + (data[2].b<<4));
            }
            break;
        case kipl::base::UInt16 :
            logger(logger.LogMessage,"UInt16");

            for (size_t j=0; j<size_x; j++) {
                  pLine[j]   = static_cast<ImgType>(shortbuffer[j]);
            }
            break;
        case kipl::base::Float32 :
            logger(logger.LogMessage,"Float32");

            for (size_t j=0; j<size_x; j++) {
                  pLine[j]   = static_cast<ImgType>(floatbuffer[j]);
            }
            break;
        }
    }

    delete [] buffer;
    file.close();
    return 0;
}

}}
#endif // IO_GENERIC_HPP


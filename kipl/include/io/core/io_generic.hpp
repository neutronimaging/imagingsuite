#ifndef IO_GENERIC_HPP
#define IO_GENERIC_HPP
#include <fstream>
#include "../../base/kiplenums.h"
namespace kipl { namespace io {

union converter
{
  // for reading
  char c;

  // for convertion
  unsigned char b;
  struct
  {
    unsigned int hi: 4;
    unsigned int lo: 4;
  } nibbles;
};

kipl::base::TImage<unsigned short,2> ReadDat(std::string fname, size_t x, size_t y, size_t idx)
{
    size_t dims[2]={x+(x%16),y};
    kipl::base::TImage<unsigned short,2> img(dims);

    ifstream datfile(fname.c_str(), ios_base::binary);

    unsigned short *pImg=img.GetDataPtr();
    size_t offset=(img.Size()+img.Size(0))*1.5*idx;
    datfile.seekg(offset);

    size_t N=img.Size();
    converter data[3];

    for (size_t i=0; i<N; i+=2)
    {
        datfile.get(data[0].c);
        datfile.get(data[1].c);
        datfile.get(data[2].c);

        pImg[i]=data[1].nibbles.hi + (data[0].b<<4);
        pImg[i+1]=data[1].nibbles.lo + (data[2].b<<4);
    }

    dims[0]=x;

    kipl::base::TImage<unsigned short,2>  res(dims);

    for (size_t i=0; i<img.Size(1); i++) {
        memcpy(res.GetLinePtr(i),img.GetLinePtr(i),res.Size(0)*sizeof(unsigned short));
    }

    return res;
}

int ReadGeneric(ifstream &file,kipl::base::TImage<unsigned short> &img,
                size_t size_x,
                size_t size_y,
                size_t offset, //< Given in bytes
                size_t stride, //< Given in bytes
                size_t imagesperfile,
                kipl::base::eDataType dt,
                kipl::base::eEndians endian,
                size_t imageindex,
                size_t const * const nCrop=NULL)
{
    if (imagesperfile<=imageindex) {
        throw kipl::base::KiplException("Tried to access a non-existing image",__FILE__,__LINE__);
    }

    size_t file_offset=imageindex*size_y*stride+offset;
    std::cout<<"Offset="<<file_offset<<std::endl;
    file.seekg(file_offset);

    size_t dims[2]={size_x, size_y};
    img.Resize(dims);
    converter data[3];

    char * buffer = new char[stride];

    unsigned short *sbuffer= reinterpret_cast<unsigned short *>(buffer);
    for (size_t i=0; i<size_y; i++) {
        for (size_t j=0; j<stride; j++)
            file.get(buffer[j]);
        unsigned short * pLine = img.GetLinePtr(i);
        switch (dt) {
        case kipl::base::UInt4 :
            for (size_t j=0, k=0; j<size_x; j+=2, k++) {
                  data[0].c=buffer[k];

                  pLine[j]   = static_cast<unsigned short>(data[0].nibbles.lo);
                  pLine[j+1] = static_cast<unsigned short>(data[0].nibbles.hi);
            }
            break;
        case kipl::base::UInt8 :
            for (size_t j=0; j<size_x; j++)
                pLine[j]=static_cast<unsigned short>(buffer[i]);
            break;
        case kipl::base::UInt12 :
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

            for (size_t j=0, k=0; j<size_x; j++) {
                  pLine[j]   = static_cast<unsigned short>(sbuffer[j]);

            }
            break;
        }
    }

    return 0;
}

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
    ifstream datfile(fname, ios_base::binary);

    kipl::base::TImage<unsigned short> tmp;

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

    return 0;
}

}}
#endif // IO_GENERIC_HPP


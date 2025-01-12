//<LICENCE>

#include <list>

#include "../../include/kipl_global.h"
#include "../../include/base/timage.h"
#include "../../include/base/kiplenums.h"
#include "../../include/io/io_generic.h"

namespace kipl { namespace io {


kipl::base::TImage<unsigned short,2> KIPLSHARED_EXPORT ReadDat(std::string fname, size_t x, size_t y, size_t idx)
{
    std::vector<size_t> dims={x+(x%16),y};
    kipl::base::TImage<unsigned short,2> img(dims);

    ifstream datfile(fname.c_str(), ios_base::binary);
    if (!datfile.good()) {
        std::ostringstream msg;
        msg<<"ReadDat failed to open file: "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

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

//int  KIPLSHARED_EXPORT ReadGeneric(ifstream &file,kipl::base::TImage<unsigned short,2> &img,
//                size_t size_x,
//                size_t size_y,
//                size_t offset, //< Given in bytes
//                size_t stride, //< Given in bytes
//                size_t imagesperfile,
//                kipl::base::eDataType dt,
//                kipl::base::eEndians endian,
//                size_t imageindex,
//                size_t const * const nCrop)
//{
//    if (imagesperfile<=imageindex) {
//        throw kipl::base::KiplException("Tried to access a non-existing image",__FILE__,__LINE__);
//    }

//    size_t file_offset=imageindex*size_y*stride+offset;
//    file.seekg(file_offset);

//    size_t dims[2]={size_x, size_y};
//    img.Resize(dims);

//    img=(unsigned short)0;
//    converter data[3];

//    char * buffer = new char[stride];

//    unsigned short *sbuffer= reinterpret_cast<unsigned short *>(buffer);
//    for (size_t i=0; i<size_y; i++) {
//        //for (size_t j=0; j<stride; j++)
//        file.get(buffer,stride);
//        unsigned short * pLine = img.GetLinePtr(i);
//        switch (dt) {
//        case kipl::base::UInt4 :
//            for (size_t j=0, k=0; j<size_x; j+=2, k++) {
//                  data[0].c=buffer[k];

//                  pLine[j]   = static_cast<unsigned short>(data[0].nibbles.lo);
//                  pLine[j+1] = static_cast<unsigned short>(data[0].nibbles.hi);
//            }
//            break;
//        case kipl::base::UInt8 :
//            for (size_t j=0; j<size_x; j++)
//                pLine[j]=static_cast<unsigned short>(buffer[i]);
//            break;
//        case kipl::base::UInt12 :
//            for (size_t j=0, k=0; j<size_x; j+=2, k+=3) {
//                // Careful with signed unsigned...
//                  data[0].c=buffer[k];
//                  data[1].c=buffer[k+1];
//                  data[2].c=buffer[k+2];

//                  pLine[j]   = static_cast<unsigned short>(data[1].nibbles.hi + (data[0].b<<4));
//                  pLine[j+1] = static_cast<unsigned short>(data[1].nibbles.lo + (data[2].b<<4));
//            }
//            break;
//        case kipl::base::UInt16 :
//            memcpy(pLine,buffer,stride);
////            for (size_t j=0; j<size_x; j++) {
////                  pLine[j]   = static_cast<unsigned short>(sbuffer[j]);

//            //}
//            break;
//        }
//    }

//    delete [] buffer;
//    file.close();
//    return 0;
//}
}}

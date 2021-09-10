//<LICENSE>

#include <fstream>
#include <sstream>
#include <vector>
#include <ios>
#include <algorithm>
#include "../../include/io/io_vivaseq.h"
#include "../../include/base/KiplException.h"


namespace kipl { namespace io {
template <typename T>
inline float castshort2float(unsigned short a) { return static_cast<float>(a);}

ViVaSEQHeader::ViVaSEQHeader() :
    headerSize(sizeof(ViVaSEQHeader))
{

}

int GetViVaSEQHeader(std::string fname,ViVaSEQHeader *header)
{
    std::ifstream file(fname.c_str(),ios::binary | ios::in);
    if (!file.bad())
        file.read(reinterpret_cast<char *>(header),sizeof(ViVaSEQHeader));
    else {
        std::ostringstream msg;
        msg<<"GetViVaSEQHeader: Failed to open "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    return 0;
}

std::vector<size_t> GetViVaSEQDims(const string &fname)
{
    ViVaSEQHeader header;
    GetViVaSEQHeader(fname,&header);

    std::vector<size_t> dims({ header.imageWidth,
                               header.imageHeight,
                               header.numberOfFrames});

    return dims;
}

int ReadViVaSEQ(std::string fname, kipl::base::TImage<float,3> &img, const std::vector<size_t> & roi, int first_frame, int last_frame, int frame_step)
{
    std::ifstream file(fname.c_str(),ios::binary | ios::in);
    if (file.bad()) {
        std::ostringstream msg;
        msg<<"GetViVaSEQHeader: Failed to open "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    ViVaSEQHeader header;

    file.read(reinterpret_cast<char *>(&header),sizeof(ViVaSEQHeader));

    const size_t framesize = header.imageWidth * header.imageHeight * header.bytesPerPixel;
    if (last_frame<first_frame)
        throw kipl::base::KiplException("ReadViVaSEQ: last frame < first_frame",__FILE__,__LINE__);

    int first = first_frame;
    int last  = last_frame;
    int step  = frame_step;

    if (first_frame == -1) {
        first = 0;
        last = header.numberOfFrames;
    }


    if (roi.empty()) {
        std::vector<size_t> dims={static_cast<size_t>(header.imageWidth),
                        static_cast<size_t>(header.imageHeight),
                        static_cast<size_t>((last-first)/frame_step)};

        img.resize(dims);

        char * buffer = new char[framesize];
        unsigned short *sbuffer = reinterpret_cast<unsigned short *>(buffer);
        for (int i=first; i<last; i+=step) {
            file.read(buffer,framesize);
            float* pImg=img.GetLinePtr(0,(i-first)/frame_step);

            std::copy(sbuffer,sbuffer+img.Size(0)*img.Size(1),pImg);
        }
        delete [] buffer;
    }
    else {
        std::vector<size_t> dims={roi[2]-roi[0],
                        roi[3]-roi[0],
                        static_cast<size_t>((last-first)/frame_step)};

        img.resize(dims);

        const size_t buffersize = img.Size(0) * header.bytesPerPixel;
        char * buffer = new char[buffersize];

        for (int i=first; i<last; i+=frame_step) {
            for (int j=roi[1]; j<static_cast<int>(roi[3]); ++j ) {
                file.seekg(header.headerSize+                               // header offset
                           i*framesize +                                    // frame offset
                           j*header.imageWidth*header.bytesPerPixel +       // row offset
                           roi[0]*header.bytesPerPixel,                     // indent
                           ios_base::beg);
                file.read(buffer,buffersize);

                float* pImg=img.GetLinePtr(j-roi[1],(i-first)/frame_step);

                std::copy(buffer,buffer+img.Size(0),pImg);
            }
        }

        delete [] buffer;
    }

    return 0;
}

int ReadViVaSEQ(std::string fname, kipl::base::TImage<float,2> &img, int idx, const std::vector<size_t> &roi)
{
    std::ifstream file(fname.c_str(),ios::binary | ios::in);
    if (file.bad()) {
        std::ostringstream msg;
        msg<<"GetViVaSEQHeader: Failed to open "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    ViVaSEQHeader header;

    file.read(reinterpret_cast<char *>(&header),sizeof(ViVaSEQHeader));

    const size_t framesize = header.imageWidth * header.imageHeight * header.bytesPerPixel;
    const size_t linesize  = header.imageWidth * header.bytesPerPixel;

    char *buffer = new char[framesize];
    unsigned short *sbuffer = reinterpret_cast<unsigned short *>(buffer);

    if (roi.empty()) {
        std::vector<size_t> dims={header.imageWidth,header.imageHeight};
        img.resize(dims);

        file.seekg(static_cast<size_t>(idx)*framesize+header.headerSize,ios_base::beg);
        file.read(buffer,framesize);


        std::copy(sbuffer,sbuffer+img.Size(),img.GetDataPtr());
    }
    else {
        std::vector<size_t> dims={roi[2]-roi[0],roi[3]-roi[0]};
        img.resize(dims);

        for (size_t j=roi[1]; j<roi[3]; ++j ) {
            file.seekg(header.headerSize+                               // header offset
                       idx*framesize +                                    // frame offset
                       j*header.imageWidth*header.bytesPerPixel +       // row offset
                       roi[0]*header.bytesPerPixel,                     // indent
                       ios_base::beg);
            file.read(buffer,linesize);

            float* pImg=img.GetLinePtr(j-roi[1]);

            std::copy(buffer,buffer+img.Size(0),pImg);
        }
    }

    delete [] buffer;
    return 0;
}

int WriteViVaSEQ(std::string fname, kipl::base::TImage<float,3> & img, float low, float high)
{
    std::ofstream file(fname.c_str(),ios::binary | ios::out);
    if (file.bad()) {
        std::ostringstream msg;
        msg<<"WriteViVaSEQ: Failed to open "<<fname;
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
    ViVaSEQHeader header;

    header.imageWidth  = img.Size(0);
    header.imageHeight = img.Size(1);
    header.numberOfFrames = img.Size(2);
    header.bytesPerPixel = 2;

    float slope     = 1.0f;
    float intercept = 0.0f;

    if (low<high) {
        slope = 65565.0f/(high-low);
        intercept = low;
    }

    file.write(reinterpret_cast<char *>(&header), sizeof(ViVaSEQHeader));
    kipl::base::TImage<unsigned short,2> tmp(img.dims());
    char *pTmp = reinterpret_cast<char *>(tmp.GetDataPtr());
    for (size_t i=0 ; i<img.Size(2); ++i) {
        float *pImg = img.GetLinePtr(0,i);
        for (size_t j = 0; j<tmp.Size(); ++j) {
            if ((low<=pImg[j]) && (pImg[j]<=high))
                tmp[j]=static_cast<unsigned short>((pImg[j]-intercept)*slope);
            else
                tmp[j]=static_cast<unsigned short>((high<pImg[i])*65565);
        }
        file.write(pTmp,tmp.Size()*sizeof(unsigned short));
    }

    return 0;
}
}}

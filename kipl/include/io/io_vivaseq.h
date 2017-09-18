#ifndef VIVASEQ_H
#define VIVASEQ_H

#include "../kipl_global.h"
#include <string>

#include "../base/timage.h"
namespace kipl { namespace io {

class KIPLSHARED_EXPORT ViVaSEQHeader
{
public:
    ViVaSEQHeader();
    unsigned int headerSize;
    char headerVersion[12];
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int bytesPerPixel;
    unsigned int numberOfFrames;
    char fileType[12];
    unsigned int value1;
    unsigned int value2;
    unsigned int maxValue;
    unsigned int minValue;
    char date[24];
    char comment[1024];
    char receptorVersion[36];
    char systemSoftwareVersion[48];
    float frameRate; // 1196
    float analogGain; // val 2
    unsigned int linesPerPixel; // val 1
    unsigned int columnsPerPixel; // val 1
    unsigned int receptorType; // val 44
    unsigned int offsetCorrection; // val 0
    unsigned int gainCorrection; // val 1
    unsigned int defectMapCorrection; // val 1
    unsigned int lineNoiseCorrection; // val 1
    unsigned int value12; // val 0
    unsigned int value13; // val 1
    unsigned int value14; // val 0
    double fvalue6; // val 2.99989
    unsigned int value15; // val 0
    unsigned int value16; // val 2
    char buffer2[412];
    char buffer3[260];

    int imageType;
    int imageFrameID;
    char boardID[12];
    float temperature[8];
    float voltage[16];
};

int KIPLSHARED_EXPORT GetViVaSEQHeader(std::string fname, ViVaSEQHeader *header);

int KIPLSHARED_EXPORT GetViVaSEQDims(std::string fname,size_t *dims, int &numframes);

int KIPLSHARED_EXPORT ReadViVaSEQ(std::string fname, kipl::base::TImage<float,3> &img, size_t *roi=nullptr, int first_frame=-1, int last_frame=0, int frame_step=1);

int KIPLSHARED_EXPORT ReadViVaSEQ(std::string fname, kipl::base::TImage<float,2> &img, int idx, size_t *roi=nullptr);

int KIPLSHARED_EXPORT WriteViVaSEQ(std::string fname, kipl::base::TImage<float,3> & img, float low=0.0f, float high=0.0f);

}
}
#endif // VIVASEQ_H

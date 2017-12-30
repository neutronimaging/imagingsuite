#ifndef ROI_H
#define ROI_H

#include "../kipl_global.h"
#include <string>

#include "timage.h"
#include "../math/statistics.h"
#include "../logging/logger.h"

namespace kipl {

namespace base {

//class KIPLSHARED_EXPORT ROIBase
//{
//    kipl::logging::Logger logger;
//public:
//    ROIBase(std::string name, int dims);
//    ROIBase(ROIBase &roi);
//    virtual ~ROIBase() = 0;

//    virtual ROIBase & operator=(ROIBase & roi);

////    virtual void load(std::string fname);
////    virtual void save(std::string fname)=0;

//    virtual int getBox(size_t *roi)=0;
//    int getDimensions();
//    int getID();
//protected:
//    static int cnt;
//    int id;
//    std::string roiName;
//    int dimensions;
//    size_t position[3];
//};

// todo Implement a ROI item class

class KIPLSHARED_EXPORT RectROI
{
        kipl::logging::Logger logger;
    public:
        RectROI(size_t *roi);
        RectROI(size_t x0, size_t y0, size_t x1, size_t y1);

        RectROI(const RectROI &roi);
        virtual ~RectROI();

        virtual RectROI & operator=(const RectROI & roi);
        virtual int getBox(size_t *roi);
        int getDimensions();
        int getID();
        std::string getName();
     protected:
        static int cnt;
        int id;
        std::string roiName;
        int dimensions;
        size_t coords[4];

};


}
}

#endif // ROI_H

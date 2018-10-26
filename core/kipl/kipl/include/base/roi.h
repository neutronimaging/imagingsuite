#ifndef ROI_H
#define ROI_H

#include "../kipl_global.h"
#include <string>

#include "../logging/logger.h"

namespace kipl {
namespace base {

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
        std::string toString();
        void fromString(std::string str);
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

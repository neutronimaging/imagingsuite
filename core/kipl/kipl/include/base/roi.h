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
        RectROI(size_t *roi, const std::string &lbl="0");
        RectROI(size_t x0, size_t y0, size_t x1, size_t y1,const std::string &lbl="0");

        RectROI(const RectROI &roi);
        virtual ~RectROI();

        virtual const RectROI & operator=(const RectROI & roi);
        virtual int getBox(size_t *roi) const;
        size_t const * box() const;
        int getDimensions();
        int getID();
        std::string toString();
        void fromString(std::string str);
        std::string label();
        void setLabel(const std::string &lbl);
     protected:
        static int cnt;
        int id;
        std::string roiLabel;
        int dimensions;
        size_t coords[4];

};


}
}

#endif // ROI_H

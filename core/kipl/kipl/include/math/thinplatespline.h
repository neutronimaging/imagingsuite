#ifndef _THINPLATESPLINE_H
#define _THINPLATESPLINE_H

#include "../kipl_global.h"
#include "../base/timage.h"

#include <vector>

namespace kipl {
    namespace math {
        class KIPLSHARED_EXPORT ThinPlateSpline 
        {
        public:
            ThinPlateSpline();
            ThinPlateSpline(const std::vector<float>& x, const std::vector<float>& y, const std::vector<float>& w);

            ~ThinPlateSpline();

            void setPoints(const std::vector<float>& x, const std::vector<float>& y, const std::vector<float>& w);
            float interpolate(float x, float y);
            float interpolate(size_t x, size_t y);
            
            kipl::base::TImage<float,2> render(const std::vector<size_t> dims, const std::vector<size_t> &fov = {});            
        
            size_t size() const { return this->x.size(); }

        private:
            std::vector<float> x;
            std::vector<float> y;
            std::vector<float> w;

            void fit();
            void checkSize(const std::vector<float>& x, const std::vector<float>& y, const std::vector<float>& w);
        };
    }
}

#endif

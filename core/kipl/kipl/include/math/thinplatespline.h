#ifndef _THINPLATESPLINE_H
#define _THINPLATESPLINE_H

#include "../kipl_global.h"
#include "../base/timage.h"

#include <vector>
#include <armadillo>

namespace kipl {
    namespace math {
        class KIPLSHARED_EXPORT ThinPlateSpline 
        {
        public:
            ThinPlateSpline();
            ThinPlateSpline(const std::vector<float>  &x, const std::vector<float>  &y, const std::vector<float>  &values);
            ThinPlateSpline(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &values);

            ~ThinPlateSpline();

            void setPoints(const std::vector<float>  &x, const std::vector<float>  &y, const std::vector<float>  &values);
            void setPoints(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &values);

            float interpolate(float x, float y);
            float interpolate(double x, double y);
            float interpolate(size_t x, size_t y);
            
            kipl::base::TImage<float,2> render(const std::vector<size_t> dims, const std::vector<size_t> &fov = {});            
        
            size_t size() const { return this->m_points.size(); }

        private:
            std::vector<double> m_x;
            std::vector<double> m_y;
            std::vector<double> m_values;
            std::vector<arma::vec> m_points;
            arma::vec m_w;
            arma::vec m_c;   

            void fit();
        };
    }
}

#endif

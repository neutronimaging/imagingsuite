//<LICENSE>

#include "../include/math/thinplatespline.h"

#include <vector>
#include <cmath>
#include <armadillo>
#include <iostream>

// Thin Plate Spline Radial Basis Function

namespace kipl { namespace math {

float tps_rbf(float r) 
{
    if (r == 0.0f) 
        return 0.0f;

    return r * r * std::log(r);
}
    
ThinPlateSpline::ThinPlateSpline() 
{    }

ThinPlateSpline::ThinPlateSpline( const std::vector<float>& x, 
                                  const std::vector<float>& y, 
                                  const std::vector<float>& values)
{    
    setPoints(x, y, values);
}

ThinPlateSpline::ThinPlateSpline( const std::vector<double>& x, 
                                  const std::vector<double>& y, 
                                  const std::vector<double>& values)
{    
    setPoints(x, y, values);
}

ThinPlateSpline::~ThinPlateSpline() 
{    

}

void ThinPlateSpline::setPoints( const std::vector<float>& x, 
                                 const std::vector<float>& y, 
                                 const std::vector<float>& values) 
{    
    if (x.size() != y.size() || x.size() != values.size()) 
    {
        throw std::invalid_argument("Vectors x, y, and w must have the same size");
    }

    m_points.resize(x.size());
    m_values.resize(x.size());

    for (size_t i = 0; i < x.size(); ++i) 
    {
        m_points[i] = arma::vec({static_cast<double>(x[i]) , static_cast<double>(y[i]) });
        m_values[i] = static_cast<double>(values[i]);
    }

    fit();
}

void ThinPlateSpline::setPoints( const std::vector<double>& x, 
                                 const std::vector<double>& y, 
                                 const std::vector<double>& values) 
{    
    if (x.size() != y.size() || x.size() != values.size()) 
    {
        throw std::invalid_argument("Vectors x, y, and w must have the same size");
    }

    m_points.resize(x.size());
    m_values.resize(x.size());

    for (size_t i = 0; i < x.size(); ++i) 
    {
        m_points[i] = arma::vec({x[i] , y[i] });
        m_values[i] = static_cast<double>(values[i]);
    }

    fit();
}

float ThinPlateSpline::interpolate(double x, double y) 
{    
    if (m_points.size() == 0) 
    {
        throw std::invalid_argument("No points have been set");
    }

    arma::vec p = {x, y};
    double result = m_c(0) + m_c(1) * x + m_c(2) * y;
    for (size_t i = 0; i < m_points.size(); ++i) {
        result += m_w(i) * tps_rbf(arma::norm(p - m_points[i]));
    }

    return result;
}

float ThinPlateSpline::interpolate(float x, float y) 
{    
    return interpolate(static_cast<double>(x), static_cast<double>(y));
}

float ThinPlateSpline::interpolate(size_t x, size_t y) 
{    
    return interpolate(static_cast<double>(x), static_cast<double>(y));
}

kipl::base::TImage<float,2> ThinPlateSpline::render(const std::vector<size_t> dims, const std::vector<size_t> &fov) 
{    
    kipl::base::TImage<float,2> img(dims);

    for (size_t i = 0; i < dims[1]; ++i) 
    {
        for (size_t j = 0; j < dims[0]; ++j) 
        {
            img(i, j) = interpolate(i, j);
        }
    }

    return img;
}

void ThinPlateSpline::fit() 
{    
    size_t n = m_points.size();
        
    // Create the K matrix
    arma::mat K(n, n, arma::fill::zeros);
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j) 
        {
            K(i, j) = tps_rbf(arma::norm(m_points[i] - m_points[j]));
        }
    }

    // Create the P matrix
    arma::mat P(n, 3, arma::fill::ones);
    for (size_t i = 0; i < n; ++i) 
    {
        P(i, 1) = m_points[i][0] ;
        P(i, 2) = m_points[i][1] ;
    }

    // Form the system of equations
    arma::mat A = arma::join_vert(
        arma::join_horiz(K, P),
        arma::join_horiz(P.t(), arma::zeros<arma::mat>(3, 3))
    );

    arma::vec B = arma::join_vert(arma::vec(m_values), arma::zeros<arma::vec>(3));

    // Solve the system
    arma::vec params = arma::solve(A, B);

    // std::cout << "params: " << params << std::endl;

    // Extract the weights and coefficients
    m_w = params.subvec(0, n - 1);
    m_c = params.subvec(n, n + 2);

}

}}

/*
#include <iostream>
#include <vector>
#include <cmath>
#include <armadillo>

// Thin Plate Spline Radial Basis Function
double tps_rbf(double r) {
    if (r == 0.0) return 0.0;
    return r * r * std::log(r);
}

// Thin Plate Spline Fitting
class ThinPlateSpline {
public:
    ThinPlateSpline(const std::vector<arma::vec> &points, const std::vector<double> &values) {
        int n = points.size();
        
        // Create the K matrix
        arma::mat K(n, n, arma::fill::zeros);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                K(i, j) = tps_rbf(arma::norm(points[i] - points[j]));
            }
        }

        // Create the P matrix
        arma::mat P(n, 3, arma::fill::ones);
        for (int i = 0; i < n; ++i) {
            P(i, 0) = points ;
            P(i, 1) = points ;
        }

        // Form the system of equations
        arma::mat A = arma::join_vert(
            arma::join_horiz(K, P),
            arma::join_horiz(P.t(), arma::zeros<arma::mat>(3, 3))
        );

        arma::vec B = arma::join_vert(arma::vec(values), arma::zeros<arma::vec>(3));

        // Solve the system
        arma::vec params = arma::solve(A, B);

        // Extract the weights and coefficients
        w_ = params.subvec(0, n - 1);
        c_ = params.subvec(n, n + 2);

        points_ = points;
    }

    double interpolate(double x, double y) const {
        arma::vec p = {x, y};
        double result = c_(0) + c_(1) * x + c_(2) * y;
        for (size_t i = 0; i < points_.size(); ++i) {
            result += w_(i) * tps_rbf(arma::norm(p - points_[i]));
        }
        return result;
    }

private:
    std::vector<arma::vec> points_;
    arma::vec w_;
    arma::vec c_;
};

int main() {
    // Example data points
    std::vector<arma::vec> points = {
        {0, 0}, {1, 0}, {0, 1}, {1, 1}, {0.5, 0.5}
    };

    // Values at data points
    std::vector<double> values = {0, 1, 1, 0, 0.5};

    // Fit the TPS model
    ThinPlateSpline tps(points, values);

    // Interpolate new data points
    for (double x = 0; x <= 1; x += 0.1) {
        for (double y = 0; y <= 1; y += 0.1) {
            double value = tps.interpolate(x, y);
            std::cout << "TPS(" << x << ", " << y << ") = " << value << std::endl;
        }
    }

    return 0;
}

*/
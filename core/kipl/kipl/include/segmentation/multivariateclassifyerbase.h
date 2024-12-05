

#ifndef MULTIVARIATECLASSIFYERBASE_H
#define MULTIVARIATECLASSIFYERBASE_H

#include "../kipl_global.h"

#include <armadillo>

namespace kipl {
namespace segmentation {
    class KIPLSHARED_EXPORT ClassDescriptor {
    public:
        enum eDistanceMetric {
            Euclidean = 0,
            Euclidean2,
            Mahalanobis,
            Bhattacharyya
        };
        /// \brief Creates an empty class descriptor
        /// \param _name Optional class name
        ClassDescriptor(std::string _name="noname");

        /// \brief Create an initialized class descriptor
        /// \param _m mean value vector
        ClassDescriptor(arma::vec _m,
                        arma::mat _C,
                        std::string _name="noname");

        /// \brief Copy c'tor
        /// \param cd A class descriptor object
        ClassDescriptor(ClassDescriptor &cd);

        /// \brief Copy c'tor
        /// \param cd A class descriptor object
        ClassDescriptor & operator=(ClassDescriptor &cd);

        /// \brief Computes the statistical distance between two ClassDescriptors
        /// \param cd A class descriptor object
        /// \param metric Selects the metric to calculate the distance
        double Distance(ClassDescriptor &cd, eDistanceMetric metric = Bhattacharyya);

        arma::vec m; ///<! Mean value matrix [Nx1]
        arma::mat C; ///<! Covariance Matrix
        std::string className; ///<! Name of the class descriptor
    private:
        double EuclideanDistance(ClassDescriptor &cd);
        double EuclideanDistance2(ClassDescriptor &cd);
        double MahalanobisDistance(ClassDescriptor &cd);
        double BhattacharyyaDistance(ClassDescriptor &cd);

    };

    class KIPLSHARED_EXPORT MultiVariateClassifyerBase
    {
    public:
        MultiVariateClassifyerBase();
    };
}
}



#endif // MULTIVARIATECLASSIFYERBASE_H

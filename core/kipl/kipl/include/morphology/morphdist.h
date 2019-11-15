//<LICENCE>

#ifndef __MORPHDIST_H
#define __MORPHDIST_H

#include "../kipl_global.h"
#include <limits>
#include <deque>
#include <iostream>

#include "../base/timage.h"
#include "morphology.h"

using namespace std;
namespace kipl { namespace morphology {

/// \brief Base class for the implementation of distance metrics used by the distance transform
class KIPLSHARED_EXPORT CMetricBase
{
 public:
    /// Initialization of the metric
     CMetricBase(std::string _name="MetricBase");
     /// \brief Forward scanning distance
     /// \param p pointer to a pixel
     ///
     /// \returns the shortest neighborhood distance

     float forward(float *p);
     /// \brief Forward scanning distance
     /// \param data pointer to the image data
     /// \param pos position index in the image
     ///
     /// \returns the shortest neighborhood distance
     float forward(float *data, ptrdiff_t pos);

     float forward(float *data,int x, int y, int z);
     /// \brief Backward scanning distance
     /// \param p pointer to a pixel
     ///
     ///\returns the shortest neighborhood distance
     float backward(float *p);

     /// \brief Backward scanning distance
     /// \param data pointer to the image data
     /// \param pos position index in the image
     ///
     /// \returns the shortest neighborhood distance
     float backward(float *data, ptrdiff_t pos);

     float backward(float *data,int x, int y, int z);

     /// \brief Initializes the metric with the dimension of the image
     /// \param d array with image dimensions
     virtual int initialize(size_t const * const d);

     /// returns the start index for a line (edge processing)
     int start();

     /// Destructor to free memory
     virtual ~CMetricBase();

     /// returns the dimension rank of the image
     int dim() {return NDim;}

     std::string getName();
 protected:
     /// Keeps the edge distance
     int index_start;
     /// Size of the directed neighborhood
     int size;
     /// \brief Array containing the dimensions of the image to be processed
     unsigned int dims[3];
     /// Size of the neighborhood
     int ndata;
     /// \brief Array containing the weights for the forward scan
     float *w_fwd;
     /// \brief Array containing the weights for the forward scan
     float *w_bck;
     /// \brief Array containing the indices for the forward scan
     int *i_fwd;
     /// \brief Array containing the indices for the backward scan
     int *i_bck;
     /// \brief dimension supported by the metric
     int NDim;
     /// \brief string to describe the metric
     std::string name;
     /// Keeps the edge distance
     int edge_dist;
};

/// \brief Implementation of the 2D chessboard metric
class KIPLSHARED_EXPORT CMetric8connect: public kipl::morphology::CMetricBase
{
    public:
        /// Constructor for the chessboard metric
        CMetric8connect();

        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);

        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric8connect();
};

/// \brief Class that implements a 3x3 window metric
class KIPLSHARED_EXPORT CMetric3x3w: public kipl::morphology::CMetricBase
{
    public:
        /// \brief Base constructor
        CMetric3x3w();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric3x3w();
};

/// \brief Implementation of the 3D chessboard metric
class KIPLSHARED_EXPORT CMetric26conn: public kipl::morphology::CMetricBase
{
    public:
        /// Constructor for the chessboard metric
        CMetric26conn();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);

        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric26conn();
};

/// Distance metric according to Svensson-Borgefors.
/// Chamfer metric that well approximates the Euclidean distance
class KIPLSHARED_EXPORT CMetricSvensson: public kipl::morphology::CMetricBase
{
    public:
        /// Constructor for the Svensson metric
        CMetricSvensson();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetricSvensson();
};

/// \brief Implementation of the 2D cityblock metric
class KIPLSHARED_EXPORT CMetric4connect: public kipl::morphology::CMetricBase
{
    public:
        /// Constructor for the cityblock metric
        CMetric4connect();

        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric4connect();
};

/// \brief Computes the Euclidean distance map of 2D and 3D images
/// \param mask bw image containing the structures
/// \param dist resulting distance map
/// \param conn neighborhood connectivity
///
/// \todo Something locks for large distances/large images
/// \todo Does not compute the correct distance for long distances (100)
template<typename MaskType, typename DistType, size_t NDim>
int EuclideanDistance(kipl::base::TImage<MaskType,NDim> &mask,
        kipl::base::TImage<DistType,NDim> &dist, bool complement=false,
        MorphConnect conn= NDim==3 ? conn26 : conn8);

/// \brief Computes the distance transform based a given metric
/// \param img input image, must be black and white
/// \param dist output image containing the distance map of img
/// \param metric an instance of the desired metric
/// \param complement Compute the distance map of the complementary image
template<typename MaskType>
int DistanceTransform3D(kipl::base::TImage<MaskType,3> &img,
        kipl::base::TImage<float,3> &dist,
        kipl::morphology::CMetricBase &metric, bool complement=false);

/// Computes a 2D distance map using different metrics
/// \param img mask image
/// \param dist Resulting distnace map
/// \param metric A metric object
template<typename ImgType, typename DistType>
int DistanceTransform2D(kipl::base::TImage<ImgType,2> &img,
        kipl::base::TImage<DistType,2> &dist,
        kipl::morphology::CMetricBase &metric);
}}

namespace kipl { namespace morphology { namespace old {

/// \brief Base class for the implementation of distance metrics used by the distance transform
class KIPLSHARED_EXPORT CMetricBase
{
 public:
 	/// Initialization of the metric
     CMetricBase(std::string _name="MetricBase");
     /// \brief Forward scanning distance
     /// \param p pointer to a pixel
     ///	
     /// \returns the shortest neighborhood distance
     
     float forward(float *p);
     /// \brief Forward scanning distance
     /// \param data pointer to the image data
     /// \param pos position index in the image
     ///	
     /// \returns the shortest neighborhood distance
     float forward(float *data, ptrdiff_t pos);
     
     float forward(float *data,int x, int y, int z);
     /// \brief Backward scanning distance
     /// \param p pointer to a pixel
     ///	
     ///\returns the shortest neighborhood distance
     float backward(float *p);
     
     /// \brief Backward scanning distance
     /// \param data pointer to the image data
     /// \param pos position index in the image
     ///	
     /// \returns the shortest neighborhood distance
     float backward(float *data, ptrdiff_t pos);
     
     float backward(float *data,int x, int y, int z);

     /// \brief Initializes the metric with the dimension of the image
     /// \param d array with image dimensions
     virtual int initialize(size_t const * const d);
     
     /// returns the start index for a line (edge processing)
     int start(); 
     
     /// Destructor to free memory
     virtual ~CMetricBase();
     
     /// returns the dimension rank of the image
     int dim() {return NDim;}
     
     std::string getName();
 protected:
 	 /// Keeps the edge distance
     int index_start;
     /// Size of the directed neighborhood
     int size;
     /// \brief Array containing the dimensions of the image to be processed
     unsigned int dims[3];
     /// Size of the neighborhood
     int ndata;
     /// \brief Array containing the weights for the forward scan
     float *w_fwd;
     /// \brief Array containing the weights for the forward scan
     float *w_bck;
     /// \brief Array containing the indices for the forward scan
     int *i_fwd;
     /// \brief Array containing the indices for the backward scan
     int *i_bck;
     /// \brief dimension supported by the metric
     int NDim;
     /// \brief string to describe the metric
     std::string name;
     /// Keeps the edge distance
     int edge_dist;
};

/// \brief Implementation of the 2D chessboard metric
class KIPLSHARED_EXPORT CMetric8connect: public kipl::morphology::CMetricBase
{
	public:
		/// Constructor for the chessboard metric
        CMetric8connect();
        
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric8connect();
};

/// \brief Class that implements a 3x3 window metric
class KIPLSHARED_EXPORT CMetric3x3w: public kipl::morphology::CMetricBase
{
	public:
		/// \brief Base constructor
        CMetric3x3w();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric3x3w();
};

/// \brief Implementation of the 3D chessboard metric
class KIPLSHARED_EXPORT CMetric26conn: public kipl::morphology::CMetricBase
{
    public:
    	/// Constructor for the chessboard metric
        CMetric26conn();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
	
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric26conn();
};

/// Distance metric according to Svensson-Borgefors. 
/// Chamfer metric that well approximates the Euclidean distance
class KIPLSHARED_EXPORT CMetricSvensson: public kipl::morphology::CMetricBase
{
    public:
    	/// Constructor for the Svensson metric
        CMetricSvensson();
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetricSvensson();
};

/// \brief Implementation of the 2D cityblock metric
class KIPLSHARED_EXPORT CMetric4connect: public kipl::morphology::CMetricBase
{
	public:
		/// Constructor for the cityblock metric
        CMetric4connect();
        
        /// \brief Initialize the metric with the target image
        /// \param d array containing the dimension lengths
        virtual int initialize(size_t const * const d);
        /// \brief Destructor to clean the allocated memory
        virtual ~CMetric4connect();
};

/// \brief Computes the Euclidean distance map of 2D and 3D images
/// \param mask bw image containing the structures
/// \param dist resulting distance map
/// \param conn neighborhood connectivity
/// 	
/// \todo Something locks for large distances/large images
/// \todo Does not compute the correct distance for long distances (100)
template<typename MaskType, typename DistType, size_t NDim>
int EuclideanDistance(kipl::base::TImage<MaskType,NDim> &mask, 
		kipl::base::TImage<DistType,NDim> &dist, bool complement=false,
		MorphConnect conn= NDim==3 ? conn26 : conn8);

/// \brief Computes the distance transform based a given metric 
/// \param img input image, must be black and white
/// \param dist output image containing the distance map of img
/// \param metric an instance of the desired metric
/// \param complement Compute the distance map of the complementary image
template<typename MaskType>
int DistanceTransform3D(kipl::base::TImage<MaskType,3> &img, 
		kipl::base::TImage<float,3> &dist,
        kipl::morphology::CMetricBase &metric, bool complement=false);

/// Computes a 2D distance map using different metrics
/// \param img mask image
/// \param dist Resulting distance map
/// \param metric A metric object
template<typename ImgType, typename DistType>
int DistanceTransform2D(kipl::base::TImage<ImgType,2> &img, 
		kipl::base::TImage<DistType,2> &dist, 
        kipl::morphology::CMetricBase &metric);
}}}

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &os, kipl::morphology::CMetricBase &mb);

#include "core/morphdist.hpp" 
#endif


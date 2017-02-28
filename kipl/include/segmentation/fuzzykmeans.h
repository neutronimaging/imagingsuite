//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//


#ifndef SEGMENTATIONFUZZYCMEANS_H
#define SEGMENTATIONFUZZYCMEANS_H

#include <base/timage.h>
#include "segmentationbase.h"
#include <logging/logger.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <logging/logger.h>

namespace kipl { namespace segmentation {

/// \brief The class performs a fuzzy K-means segmentation of an N-dimensional image
/// @author Anders Kaestner
template<class ImgType, class SegType , size_t NDim>
class FuzzyKMeans : public SegmentationBase<ImgType,SegType,NDim>
{
public:
	/// \brief Constructor that initializes the output and some variables
	///	\param os stream for text output
    FuzzyKMeans();

    /// Empty destructor
    ~FuzzyKMeans();
    
    /// \brief Set an initial guess of the center values
    ///	\param cVec A vector containing te guesses
    ///	
    ///	\note If size(cVec)!=nClasses, an initial guess will be randomized
	int setCenters(float const * const cVec) {memcpy(centers,cVec,sizeof(float)*this->nClasses); haveCenters=true; return 0;}
	
    float const * const getCenters() const { return centers; }
	/// Removes the initial center guess
	int clearCenters() {memset(centers,0,sizeof(float)*this->nClasses); haveCenters=false; return 0;}
	
	/// \brief Setting processing parameters
	///	\param NClasses Number of classes to find
	///	\param fuz Fuzziness parameter
	///	\param maxIt Maximal number of iterations to find the solution
	int set(int NClasses, float fuz=1.5f, int maxIt=250,bool bSaveIterations=false, std::string sFname="" );
	
    
	/// \brief Segments an image
	///	\param img Image to be segmented
	///	\param seg Segmented image
    int operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg);

	/// \brief Segments an image
	///	\param img Image to be segmented
	///	\param seg Segmented image
	int parallel(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> & seg);

	/// \brief Segments an image
	///	\param img Image to be segmented
	///	\param seg Segmented image
    /// \param mask mask image for stayaway regions
    int operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg, const kipl::base::TImage<bool,NDim> & mask);


    /// \brief Interface function that is intended to perform the segmentation
    ///	\param img Input graylevel image
    ///	\param seg Segmented output image
    virtual int operator()(kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg) { }

    /// Set the parameters for the segmentation
    virtual void setParameters(std::map<std::string,std::string> parameters) { }

protected:
	/// \brief Finds initial guesses using random locations in the image
	///	\param img Original image used to find the guesses
	int initCenters(const kipl::base::TImage<ImgType,NDim> &img);
	int initU(kipl::base::TImage<float,2> &img);
	
	/// Limits the iteration process
	int maxIterations;
	/// indicates that a initial guess has been provided
	bool haveCenters;
	/// vector containing the initial guesses
	float *centers;
	/// Fuzziness coefficient
	float fuzziness;
	/// Support variable for the fuzziness
	float mpower;
	bool m_bSaveIterations;
	std::string m_sIterationFileName;
};

}}

#include "core/fuzzykmeans.hpp"

#endif

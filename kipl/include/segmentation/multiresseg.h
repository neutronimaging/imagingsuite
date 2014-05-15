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
#ifndef IMAGETHRESHOLDMULTIRESSEG_H
#define IMAGETHRESHOLDMULTIRESSEG_H

#include <base/timage.h>
#include <vector>
#include <iostream>
#include <string>
#include "segmentationbase.h"

namespace akipl { namespace segmentation {

/// \brief Base class for segment update tasks 
///	@autor Anders Kaestner
template <class ImgType, class SegType, size_t NDim>
class SegmentUpdateBase {
protected:
	/// Stream for processing output
	ostream &logstream;
public: 	
	/// \brief Constructor to initialize the outputstream and some other variables
	/// \param os Stream resource for processing message output
	SegmentUpdateBase(ostream & os) : logstream(os) {quiet=true; useStayAway=false; name="BaseUpdater"; }
	virtual ~SegmentUpdateBase() {}
	/// \brief Interface function that is intended to do the class undates
	/// \param parent Graylevel image containing the parent 
	/// \param parentSeg Segment image containing the segmented parent
	/// \param child Graylevel image containing the child
	/// \param childSeg Segment image containing the segmented child (processing output)
	virtual int update(const kipl::base::TImage<ImgType, NDim> &parent, 
			   const kipl::base::TImage<SegType, NDim> & parentSeg, 
			   const kipl::base::TImage<ImgType, NDim> &child, 
			   kipl::base::TImage<SegType, NDim> &childSeg)=0;
			   
	/// Tells the name of the current class
	const std::string & getName() {return name;}
	
	/// \brief Sets the number of classes in the segmented image
	/// \param NC Number of classes
	int setClasses(int NC) {nClasses=NC; return 0;}
	
	int setSampler(int s) {this->sampler=s; return 0;}
	virtual int Quiet(bool q) { quiet=q; return 0; }
	
protected:
	/// \brief Computes the class statistics of the parent image
	/// \param img The parent image (graylevel)
	/// \param seg Segment image of the parent
	int ComputeClassStatistics(const kipl::base::TImage<ImgType,NDim> &img, 
							const kipl::base::TImage<SegType,NDim> &seg);
							
	/// No used
	bool useStayAway;
	/// NUmber of supported classes
	int nClasses;
	/// Name of the class
	std::string name;
	/// Switch to control the message output
	bool quiet;
	
	/// Vector containing the class mean values
	vector<double> mean;
	/// Vector containing the class variances
	vector<double> var;
	/// Vector containing the class probabilities
	vector<double> cnt;	
};

/// \brief Framework class for pyramid based segmentation
///
/// The class performs a segmentation of an image using a scale pyramid. The Top level is segmented by an SegmentationBase object and the pyramid levels are updated with a SegmentUpdateBase object.
/// 
/// @author Anders Kaestner
template <class ImgType, class SegType, size_t NDim>
class MultiResolution: public akipl::segmentation::SegmentationBase<ImgType,SegType,NDim> {
	/// Member object the performs the top level segmentation
	akipl::segmentation::SegmentationBase<ImgType,SegType,NDim> &segmenter;
	/// Member object the performs the segment update task
	akipl::segmentation::SegmentUpdateBase<ImgType,SegType,NDim> &updater;
	/// Stream for processing output
public:
	/// \brief Initializes the pyramid with top level segmenter, level updater and log stream
	/// \param Seg Top level segmenter object
	///	\param Upd Level updater
	///	\param os Stream resouce for messages
    MultiResolution(SegmentationBase<ImgType,SegType,NDim> &Seg,
    				SegmentUpdateBase<ImgType,SegType,NDim> &Upd, 
    				std::ostream &os=std::cout);

	/// Empty destructor
    virtual ~MultiResolution() ;
    
    /// \brief Performs the segmentation on an image
    int process(const kipl::base::TImage<ImgType, NDim> &img, 
    		kipl::base::TImage<SegType,NDim> &seg, 
    		int levels, 
    		int classes=2);
    /// Clear the scale pyramid from scale images (save memory)
	int clear() {scalevec.clear(); return 0;}

    /// Unused parameter
    bool doHistogram;
    /// Currently not used 
    int setStayAway() {
    	useStayAway=true; 
    	return 1;
    }
    /// \brief 
    virtual int operator()(const kipl::base::TImage<ImgType,NDim> & img, kipl::base::TImage<SegType,NDim> &seg)
    {
    	return process(img, seg, nLevels, this->nClasses);
    }
    /// Currently not used 
    void clearStayAway() {useStayAway=false;}
    
    /// Currently not used 
    ///	\param m Mask image
    int setMask(kipl::base::TImage<char,2> &m) {mask=m; return 0;}
    
    /// \brief Limiting the dynamics interval. Currently not used 
    /// \param lo lowest allowed graylevel
    /// \param hi highest allowed graylevel
    int setInterval(ImgType lo, ImgType hi) 
    	{intervalLo=lo<hi ? lo : hi; intervalHi=lo>hi ? lo : hi; return 0;}
    
    /// \brief sets the number of pyramid levels
    ///	\param l the number of levels
    int setLevels(int l) {nLevels=abs(l); return 0;}
    
    int setSampler(int samplerType, float par=1.0) {sampler=samplerType; samplerSigma=par; return 0;}

    /// \brief Toggles the amount of message output
	///	\param q boolean variable that toggles the message output mode
	virtual bool Quiet(bool q) {this->quiet=q; segmenter.Quiet(q) ; updater.Quiet(q); return 0; }
protected:

	/// \brief Builds the pyramid
	/// \param img original image
	/// \param levels number of scale levels in the pyramid
	int PrepareScales(const kipl::base::TImage<ImgType,NDim> img, int levels);
	
	/// Vector containing the scale images
	vector<kipl::base::TImage<ImgType, NDim> > scalevec;

	/// Switch to select if a mask should be used or not
	bool useStayAway;
	/// Upper dynamics limiter
	ImgType intervalHi;
	/// Lower dynamics limiter
	ImgType intervalLo;
	
	/// maximum class label
	SegType maxCVal;
	/// minimum class label
	SegType minCVal;
	
	/// Mask image
	kipl::base::TImage<char,2> mask;
	
	/// NUmber of pyramid levels
	int nLevels;
	
	/// Sigma of the Gaussian sampler
	double samplerSigma;
	
	/// Sampler selector
	int sampler;
};


}}
#include "core/multiresseg.hpp"

#endif



#ifndef __SEGMENTATIONWORKER_H
#define __SEGMENTATIONWORKER_H

//#include <misc/cparameters.h>
#include <queuefilter/basequeueworker.h>
#include <iostream>
#include <string>
#include <vector>
#include <image/image.h>
#include <segmentation/segmentationbase.h>
#ifdef USE_MPI
#include <mpi.h>
#endif


namespace QueueFilter {

template<class ImgType>
class SegmentationWorker: public BaseQueueWorker<ImgType>
{
	Segmentation::SegmentationBase<ImgType,char,3> & segmenter;
public:
	/** \brief Constructor that initializes the segmentation class
		\param s the segmenter instance to perform the segmenation task
		\param os stream for logmessages
		
	*/
	SegmentationWorker(Segmentation::SegmentationBase<ImgType,char,3> & s, std::ostream &os=std::cout);
	
	/** \brief Does the segmentation work
		\param img the image to be processed
	*/
	virtual int proc(Image::CImage<ImgType,3> & img);
	
	/// Empty destructor
	virtual ~SegmentationWorker() {}

	/** \brief insert has no meaning for the segmenter
		\param img input image
		\param res result image
	*/
    virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res) {return -1;}	
	/// Reset has no meaning for the segmenter
	virtual int reset(){return -1;};
	/// Size has no meaning to the segmenter
    virtual int size() {return -1;}
    /// Kernel size has no meaning for the segmenter
	virtual int kernelsize() {return -1;}
	
	/// Returns the parameters used by the segmentation worker
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
	
	virtual int saveProgressImages(const string &name) {return segmenter.saveProgressImages(name);}
	virtual bool Quiet(bool q) { this->quiet=q; segmenter.Quiet(q); return 0;}
protected:
	
};

template <class ImgType>
SegmentationWorker<ImgType>::SegmentationWorker(Segmentation::SegmentationBase<ImgType,char,3> & s, std::ostream &os) : BaseQueueWorker<ImgType>(os), segmenter(s)
{
	this->workername="segmentationworker with " + segmenter.getName();
    this->isfactory=false;
    this->procblock=true;
}

template <class ImgType>
int SegmentationWorker<ImgType>::proc(Image::CImage<ImgType,3> &img)
{
	CImage<char,3> seg;
	this->segmenter(img,seg);
	for (long i=0; i<img.N(); i++) 
		img[i]=(ImgType)seg[i];
		
	return 0;
}

template <class ImgType>
int SegmentationWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear(); parvals.clear();
	parnames.push_back("nClasses");
	parvals.push_back(segmenter.getClasses());
	
	return 0;
}

}

#endif

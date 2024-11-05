

#ifndef __MODIFIERWORKER_H
#define __MODIFIERWORKER_H

#include <queuefilter/basequeueworker.h>
#include <iostream>
namespace QueueFilter {

template <class ImgType>
class ModifierWorker : public BaseQueueWorker<ImgType>
{
public:
	ModifierWorker(std::ostream & os);
	ModifierWorker(float s, float o, const std::vector<float> &intv, std::ostream & os);
	
	/** \brief Does the segmentation work
		\param img the image to be processed
	*/
	virtual int proc(Image::CImage<ImgType,3> & img);
	
	/// Empty destructor
	virtual ~ModifierWorker() {}

	int setInterval(float  Lo, float  Hi);
	int setInterval(const std::vector<float> & interv);
	int setScale(float & s, float & o);
	/** \brief insert has no meaning for the segmenter
		\param img input image
		\param res result image
	*/
    virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res) {return -1;}	
	/// Reset has no meaning for the modifier
	virtual int reset(){return -1;};
	/// Size has no meaning to the modifier
    virtual int size() {return -1;}
    /// Kernel size has no meaning for the modifier
	virtual int kernelsize() {return -1;}
	
	/// Returns the parameters used by the modifier worker
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
	
private:
	ImgType hi;
	ImgType lo;
	
	float scale;
	float offset;
	
};

template <class ImgType>
ModifierWorker<ImgType>::ModifierWorker(std::ostream &os) : BaseQueueWorker<ImgType>(os)
{
	this->workername="modifierworker";
    this->isfactory=false;
    this->procblock=true;
    hi=lo=0;
    scale=1;
    offset=0;
    
}

template <class ImgType>
ModifierWorker<ImgType>::ModifierWorker(float s, float o, const vector<float> &intv, std::ostream & os) : BaseQueueWorker<ImgType>(os)
{
	this->workername="modifierworker";
    this->isfactory=false;
    this->procblock=true;
    hi=lo=0;
    scale=1;
    offset=0;
    setInterval(intv);
    setScale(s,o);
}

template <class ImgType>
int ModifierWorker<ImgType>::setInterval(float Lo, float Hi) 
{
	lo=min(Hi,Lo); 
	hi=max(Hi,Lo); 
	return 0;
}


	
template <class ImgType>
int ModifierWorker<ImgType>::setInterval(const vector<float> & interv) 
{
	if (interv.size()>1) {
		setInterval(interv[0],interv[1]); 
		return 0;
	} 
	return -1;
}

template <class ImgType>
int ModifierWorker<ImgType>::setScale(float & s, float & o)
{
	scale=s;
	offset=o;
	return 0;
}

template <class ImgType>
int ModifierWorker<ImgType>::proc(Image::CImage<ImgType,3> &img)
{
	CImage<char,3> seg;
	ImgType tmp;
	if (!this->quiet)
		this->logstream<<"Modifier worker processing"<<endl;
		
	if (hi!=lo) { // Limiting the dynamics
		if (!this->quiet) 
			this->logstream<<"Limiting the dynamics to the interval ["<<lo<<", "<<hi<<"]"<<endl;
		
		
		for (long i=0; i<img.N(); i++) {
			tmp=img[i];
			if (tmp<lo) {
				img[i]=lo;
				continue;
			}
			if (tmp>hi)
				img[i]=hi;
		}
	}
	
	if (scale!=1) {
		if (offset) {
			if (!this->quiet) 
				this->logstream<<"Scaling by "<<scale<<" and offset by "<<offset<<endl;
			for (long i=0; i<img.N(); i++) 
				img[i]=scale*img[i]+offset;
		}
		else {
			if (!this->quiet) 
				this->logstream<<"Scaling by "<<scale<<endl;
			for (long i=0; i<img.N(); i++) 
				img[i]*=scale;
		}
	}
	else {
		if (offset) {
			if (!this->quiet) 
				this->logstream<<"Offset by "<<offset<<endl;
			for (long i=0; i<img.N(); i++) 
				img[i]+=offset;
		}
	}
	
		
	return 0;
}

template <class ImgType>
int ModifierWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear(); parvals.clear();
	parnames.push_back("lo");
	parvals.push_back(lo);
	parnames.push_back("hi");
	parvals.push_back(hi);
	parnames.push_back("scale");
	parvals.push_back(scale);
	parnames.push_back("offset");
	parvals.push_back(offset);
	
	return 0;
}

}
#endif

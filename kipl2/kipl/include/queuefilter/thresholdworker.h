//<LICENCE>

#ifndef __THRESHOLDWORKER_H
#define __THRESHOLDWORKER_H

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
class ThresholdWorker: public BaseQueueWorker<ImgType>
{
	//Segmentation::SegmentationBase<ImgType,char,3> & segmenter;
public:
	/** \brief Constructor that initializes the segmentation class
		\param s the segmenter instance to perform the segmenation task
		\param os stream for logmessages
		
	*/
	ThresholdWorker(std::ostream &os=std::cout);

	int setThreshold(vector<ImgType> &v) {thVec=v; return 0;}
	int setOperation(string & op) {operation=op; return 0;}
	virtual int proc(Image::CImage<ImgType,3> &img);
    virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res) {return -1;}
    virtual int reset() {return -1;}
    virtual int size() {return -1;}
	virtual int kernelsize() {return -1;}
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
protected:
	vector<ImgType> thVec;
	string operation;
	int less(Image::CImage<ImgType,3> &img); 
	int greater(Image::CImage<ImgType,3> &img); 
	int lesseq(Image::CImage<ImgType,3> &img); 
	int greatereq(Image::CImage<ImgType,3> &img); 
	int equal(Image::CImage<ImgType,3> &img); 
	int notequal(Image::CImage<ImgType,3> &img); 
	int closedInterval(Image::CImage<ImgType,3> &img);
	int openInterval(Image::CImage<ImgType,3> &img); 
	int openClosedInterval(Image::CImage<ImgType,3> &img); 
	int closedOpenInterval(Image::CImage<ImgType,3> &img); 
	
};

template <class ImgType>
ThresholdWorker<ImgType>::ThresholdWorker(std::ostream &os) : BaseQueueWorker<ImgType>(os)
{
	this->workername="thresholdworker";
    this->isfactory=false;
    this->procblock=true;
    thVec.push_back((ImgType)0);
    thVec.push_back((ImgType)1);
    operation=">";
}

template <class ImgType>
int ThresholdWorker<ImgType>::proc(Image::CImage<ImgType,3> &img)
{
	map<string,int> selectorMap;
	selectorMap["less"]=0;
	selectorMap["greater"]=1;
	selectorMap["leq"]=2;
	selectorMap["geq"]=3;
	selectorMap["eq"]=4;
	selectorMap["neq"]=5;
	selectorMap["[]"]=6;
	selectorMap["()"]=7;
	selectorMap["(]"]=8;
	selectorMap["[)"]=9;
	
	switch (selectorMap[operation]) {
		default : this->logstream<<"Threshold worker: unknown operation using <"<<endl;
		case 0 : less(img); break;
		case 1 : greater(img); break;
		case 2 : lesseq(img); break;
		case 3 : greatereq(img); break;
		case 4 : equal(img); break;
		case 5 : notequal(img); break;
		case 6 : closedInterval(img); break;
		case 7 : openInterval(img); break;
		case 8 : openClosedInterval(img); break;
		case 9 : closedOpenInterval(img); break;
	}
	

	return 0;
}

template <class ImgType>
int ThresholdWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();
	parnames.push_back(operation);
	parvals.push_back(thVec.front());
	
	return 0;
}

template <class ImgType>
int ThresholdWorker<ImgType>::less(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]<thval;
		
	return 0;
}

template <class ImgType>
int ThresholdWorker<ImgType>::greater(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]>thval;
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::lesseq(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]<=thval;
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::greatereq(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]>=thval;
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::equal(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]==thval;
		
	return 0;
}

	
template <class ImgType>
int ThresholdWorker<ImgType>::notequal(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	ImgType thval=thVec[0];
	
	for (long i=0; i<img.N(); i++)
		pImg[i]=pImg[i]!=thval;
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::closedInterval(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	if (thVec.size()<2) {
		this->logstream<<"ThresholdWorker: interval requires two values"<<endl;
		return -1;	
	}
	
	ImgType thlo=thVec[0];
	ImgType thhi=thVec[1];
	
	for (long i=0; i<img.N(); i++) 
		pImg[i]=((pImg[i]>=thlo) && (pImg[i]<=thhi));
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::openInterval(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	if (thVec.size()<2) {
		this->logstream<<"ThresholdWorker: interval requires two values"<<endl;
		return -1;	
	}
	
	ImgType thlo=thVec[0];
	ImgType thhi=thVec[1];
	
	for (long i=0; i<img.N(); i++) 
		pImg[i]=((pImg[i]>thlo) && (pImg[i]<thhi));
		
	return 0;
}


template <class ImgType>
int ThresholdWorker<ImgType>::openClosedInterval(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	if (thVec.size()<2) {
		this->logstream<<"ThresholdWorker: interval requires two values"<<endl;
		return -1;	
	}
	
	ImgType thlo=thVec[0];
	ImgType thhi=thVec[1];
	
	for (long i=0; i<img.N(); i++) 
		pImg[i]=((pImg[i]>thlo) && (pImg[i]<=thhi));
		
	return 0;
}

template <class ImgType>
int ThresholdWorker<ImgType>::closedOpenInterval(Image::CImage<ImgType,3> & img)
{
	ImgType *pImg=img.getDataptr();
	if (thVec.size()<2) {
		this->logstream<<"ThresholdWorker: interval requires two values"<<endl;
		return -1;	
	}
	
	ImgType thlo=thVec[0];
	ImgType thhi=thVec[1];
	
	for (long i=0; i<img.N(); i++) 
		pImg[i]=((pImg[i]>=thlo) && (pImg[i]<thhi));
		
	return 0;
}

}

#endif

//<LICENCE>

#ifndef __DISTANCEWORKER_H
#define __DISTANCEWORKER_H

#include <queuefilter/basequeueworker.h>
#include <iostream>
#include <string>
#include <vector>
#include <image/image.h>
#include <image/Morphology.h>
#include <map>
#include <image/morphdist.h>
#ifdef USE_MPI
#include <mpi.h>
#endif


namespace QueueFilter {

template<class ImgType>
class DistanceWorker: public BaseQueueWorker<ImgType>
{
	//Segmentation::SegmentationBase<ImgType,char,3> & segmenter;
public:
	/** \brief Constructor that initializes the distance class
		\param os stream for logmessages
	*/
	DistanceWorker(std::ostream &os=std::cout);
	
	int setAlgorithm(const string &aStr) {algStr=aStr; return 0;}
	int setMetric(const string &mStr) { metricStr=mStr; return 0;}
	int setComplement(bool c) {complement=c; return 0;}
	int setConnectivity(const string &cStr);
	
	virtual int proc(Image::CImage<ImgType,3> &img);
	
    virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res) {return -1;}
    virtual int reset() {return -1;}
    virtual int size() {return -1;}
	virtual int kernelsize() {return -1;}
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
protected:
	int EuclideanDist(Image::CImage<ImgType,3> &img, Image::CImage<float,3> &dist);
	int ChamferDist(Image::CImage<ImgType,3> &img, Image::CImage<float,3> &dist);
	
	string algStr;
	string metricStr;
	bool complement;
	Morphology::MorphConnect conn;
};

template <class ImgType>
DistanceWorker<ImgType>::DistanceWorker(std::ostream &os) : BaseQueueWorker<ImgType>(os)
{
	this->workername="distanceworker";
    this->isfactory=false;
    this->procblock=true;
    algStr="euclidean";
    metricStr="euclidean";
    complement=false;
    conn=Morphology::conn26;
}

template <class ImgType>
int DistanceWorker<ImgType>::setConnectivity(const string &cStr)
{
	if ((cStr=="26") || (cStr=="conn26") || (cStr=="26conn")) {
		conn=Morphology::conn26;
		return 0;
	}
	
	if ((cStr=="6") || (cStr=="conn6") || (cStr=="6conn")) {
		conn=Morphology::conn6;
		return 0;
	}
	
	if ((cStr=="18") || (cStr=="conn18") || (cStr=="18conn")) {
		conn=Morphology::conn18;
		return 0;
	}
	
	this->logstream<<"Unknown connectivity, using 26-connectivity"<<endl;
	conn=Morphology::conn26;
	return -1;
}

template <class ImgType>
int DistanceWorker<ImgType>::proc(Image::CImage<ImgType,3> &img)
{
	map<string,int> algMap;
	
	algMap["euclidean"]=0;
	algMap["chamfer"]=1;
	Image::CImage<float,3> dist;
	switch (algMap[algStr]) {
		case 0:	EuclideanDist(img,dist); break; 
		case 1: ChamferDist(img,dist); break;
		default: this->logstream<<"Unknown distance algorithm"<<endl;
	}

	ImgType *pImg=img.getDataptr();
	float *pDist=dist.getDataptr();
	for (long i=0; i<img.N(); i++)
		pImg[i]=(ImgType)pDist[i];
		
	return 0;
}

template <class ImgType>
int DistanceWorker<ImgType>::EuclideanDist(Image::CImage<ImgType,3> &img, Image::CImage<float,3> &dist)
{
	EuclideanDistance(img,dist,conn);
	return 0;
}

template <class ImgType>
int DistanceWorker<ImgType>::ChamferDist(Image::CImage<ImgType,3> &img, Image::CImage<float,3> &dist)
{
	map<string,int> metricMap;
	
	metricMap["svensson"]=0;
	metricMap["26conn"]=1;
	
	Morphology::CMetricBase *metric;
	switch (metricMap[metricStr]) {
	case 0: metric=new Morphology::CMetricSvensson; break;
	case 1: metric=new Morphology::CMetric26conn; break;
	default : this->logstream<<"Unknown distance metric"<<endl; return -1;
	}
	
	Morphology::DistanceTransform3D(img,dist,*metric,complement);

	delete metric;
	
	return 0;
}

template <class ImgType>
int DistanceWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parvals.clear();
	parnames.push_back(algStr);
	parvals.push_back(0);
	if (algStr=="chamfer") {
		parnames.push_back(metricStr);
		parvals.push_back(0);
	}
	
	return 0;
}
}

#endif

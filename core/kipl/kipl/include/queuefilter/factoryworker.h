//<LICENCE>

#ifndef __FACTORYWORKER_H
#define __FACTORYWORKER_H

#include <misc/cparameters.h>
#include <queuefilter/basequeueworker.h>
#include <queuefilter/rankfilterworker.h>
#include <queuefilter/linearfilterworker.h>
#include <queuefilter/shockworker.h>
#include <queuefilter/ioworker.h>
#include <queuefilter/diffusionworker.h>
#include <queuefilter/maskworker.h>
#include <queuefilter/segmentationworker.h>
#include <segmentation/mapupdater.h>
#include <segmentation/multiresseg.h>
#include <segmentation/fuzzykmeans.h>

#include <iostream>
namespace QueueFilter {

template <class ImgType>
class FactoryWorker : public BaseQueueWorker<ImgType>
{
protected:
	Parenc::CParameters & pars;
public:
	FactoryWorker(Parenc::CParameters & Pars, std::ostream & os=cout);
	int reset() {if (currentworker) delete currentworker; workerCnt=-1; return 0;}
	virtual int GetNextWorker(BaseQueueWorker<ImgType> ** worker);
	virtual int GetCurrentWorker(BaseQueueWorker<ImgType> ** worker);
	virtual int GetWorkerParameters(vector<string> &parnames,vector<double> &parvals);
	virtual int size() {return pars.count("worker");}
	~FactoryWorker();
protected:
	BaseQueueWorker<ImgType> *currentworker;
	virtual int ProduceWorker(const string & description);
	virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res)
	 {logstream<<"Insert should not be called in the factory worker"<<endl; return -2;}
	virtual int kernelsize() {return -1;}
	int workerCnt;
};



template <class ImgType>
FactoryWorker<ImgType>::FactoryWorker(Parenc::CParameters & Pars, std::ostream & os):pars(Pars), BaseQueueWorker<ImgType>(os)
{
    this->currentworker=nullptr;
	workerCnt=-1;
	workername="FactoryWorker";
 	this->isfactory=true;
}

template <class ImgType>
FactoryWorker<ImgType>::~FactoryWorker()
{
	if (currentworker)
		delete currentworker;
	
}

	
template <class ImgType>
int FactoryWorker<ImgType>::GetNextWorker(BaseQueueWorker<ImgType> ** worker)
{
	workerCnt++;
	string description;
	if (pars.getparString("worker",description,workerCnt)<0) {
		logstream<<"Worker "<<workerCnt<<" could not be created, using current worker"<<endl;
		return GetCurrentWorker(worker);
	}
	
	ProduceWorker(description);
	
	*worker=this->currentworker;

	int NIterations;
	
	if (Parenc::ParseString(description,"nit",NIterations))
		NIterations=1;
		
	return NIterations;
}

template <class ImgType>
int FactoryWorker<ImgType>::GetCurrentWorker(BaseQueueWorker<ImgType> ** worker)
{
	
	*worker=(this->currentworker);	

	return workerCnt;

}

template <class ImgType>
int FactoryWorker<ImgType>::ProduceWorker(const string & description)
{
	string classstr;
	if (Parenc::ParseString(description,"class",classstr)==-1) {
		logstream<<"FactoryWorker: worker description missing class name"<<endl;
		return -1;
	}
	
	map<string,int> classmap;
	classmap["gaussian"]=0;
	classmap["median"]=1;
	classmap["rank"]=2;
	classmap["shock"]=3;
	classmap["diffusion"]=4;
	classmap["ioworker"]=5;
	classmap["maskworker"]=6;
	classmap["segmentation"]=7;
	
	//cout<<"Description string: "<<description<<endl;
    BaseQueueWorker<ImgType> * tmpworker=nullptr;
	
	int filtdims[3]={3,3,3}, rank;
	float kernel[256];
	vector<int> filtdimsvec;
	vector<int> shapepars;
	float sigma, tau, amount,sum, support, lambda;
	int NKernel2,i,j;
	float maskval;
	ShockType st;
	bool filemode=true; // save slices
	bool iomode=false;  // save image
	
	string tmpstr;
		
	switch (classmap[classstr]) {
		case 0 : logstream<<"Creating a gaussian worker"<<endl; 
				if (Parenc::ParseString(description,"sigma",sigma))
					sigma=1.0f;
					
				if (Parenc::ParseString(description,"support",support))
					support=2.5f;
				
				NKernel2=(int)floor(support*sigma);
				sigma=sigma*sigma;
				sum=0.0f;
				for (i=0, j=-NKernel2; j<=NKernel2; i++,j++) {
					kernel[i]=exp(-j*j/(2*sigma));
					sum+=kernel[i];
				}
					
				for (i=0; i<2*NKernel2+1; i++) 
					kernel[i]/=sum;
					
				tmpworker=new LinearFilterWorker<float>(kernel,2*NKernel2+1, logstream);
				
				break;
		case 1 : logstream<<"Creating a median filter worker"<<endl; 
				if (Parenc::ParseString(description,"dims",filtdimsvec))
					if (filtdimsvec.size()>2) {
						filtdims[0]=filtdimsvec[0];
						filtdims[1]=filtdimsvec[1];
						filtdims[2]=filtdimsvec[2];
					}
				rank=(filtdims[0]*filtdims[1]*filtdims[2])/2;
				tmpworker=(BaseQueueWorker<ImgType> *)new RankFilterWorker<float>(filtdims,rank);
				break;
		case 2 : logstream<<"Creating a rank filter worker"<<endl; 
				if (Parenc::ParseString(description,"dims",filtdimsvec))
					if (filtdimsvec.size()>2) {
						filtdims[0]=filtdimsvec[0];
						filtdims[1]=filtdimsvec[1];
						filtdims[2]=filtdimsvec[2];
					}
				
				if (Parenc::ParseString(description,"order",rank))
					rank=0;
										
				tmpworker=new RankFilterWorker<float>(filtdims,rank);
				break;
		case 3 : logstream<<"Creating a shock worker"<<endl; 
				if (Parenc::ParseString(description,"tau",tau))
					tau=0.125f;
					
				if (Parenc::ParseString(description,"sigma",sigma))
					sigma=2.5f;
					
				if (Parenc::ParseString(description,"amount",amount))
					amount=0.005f;
					
				if (Parenc::ParseString(description,"type",tmpstr))
					st=Osher_Filter;
				else 
					if (tmpstr=="Alvarez")
						st=Alvarez_Mazorra_Filter;
					else
						st=Osher_Filter;
						
				tmpworker=new ShockWorker<float>(tau, sigma, amount, st,logstream);
				break;
		case 4 : logstream<<"Creating a diffusion worker"<<endl; 
				tau=0.125f;
				sigma=1.0;
				lambda=10;
				tmpworker=new DiffusionWorker<float>(tau, sigma, lambda, logstream);

				break;
				
		case 5 : logstream<<"Creating an IO worker"<<endl;
				Parenc::ParseString(description,"file_mode",tmpstr);
				if (tmpstr=="block") filemode=false;
				
				Parenc::ParseString(description,"io_mode",tmpstr);
				if (tmpstr=="load") iomode=true;
					
				if (Parenc::ParseString(description,"filename",tmpstr)) {
					logstream<<"Filename missing"<<endl;
					break;
				}
				
				tmpworker=new ioworker<ImgType>(tmpstr,iomode,filemode,logstream); 
				break;
		
		case 6 : logstream<<"Creating a maskworker"<<endl;
				Parenc::ParseString(description,"type",tmpstr);
				if (Parenc::ParseString(description,"maskval",maskval))
					maskval=0.0f;
					
				if (tmpstr=="file") {
					if(Parenc::ParseString(description,"filename",tmpstr)) {
						logstream<<"Filename is not specified"<<endl;
                        tmpworker=nullptr;
					}
					
					tmpworker=new MaskWorker<ImgType>(tmpstr,(ImgType)maskval,logstream);
				}
				else {
					shapepars.clear();
					if(Parenc::ParseString(description,"shape",tmpstr)) 
						tmpstr="disk";
					else 
						Parenc::ParseString(description,"parameters",shapepars);
						
					tmpworker=new MaskWorker<ImgType>(tmpstr, shapepars,(ImgType)maskval,logstream);
				}
				
				break;
				
		case 7 : {
					int nClasses=3;
					logstream<<"Creating a segmentationworker"<<endl;
					Segmentation::FuzzyCMeans<ImgType,char,3> topseg(logstream);
					Segmentation::MAPUpdaterNeighborhood2<ImgType,char,3> updater(nClasses,logstream);
					Segmentation::MultiResolution<ImgType,char,3> seg(topseg,updater,logstream);
					
					tmpworker=new SegmentationWorker<ImgType>(seg);
				}
				break;
				
        default : logstream<<"Unknown worker, keeping previous"<<endl;tmpworker=nullptr;
	}
	
	if (tmpworker) {
		if (currentworker)
			delete this->currentworker;
		this->currentworker=tmpworker;
	}
	
	int Nit;
	if (Parenc::ParseString(description,"nit",Nit))
		Nit=1;
	
	if (!Parenc::ParseString(description,"quiet",tmpstr))
		if (tmpstr=="no")
			this->currentworker->Quiet(false);
		
	return Nit;
}

template <class ImgType>
int FactoryWorker<ImgType>::GetWorkerParameters(vector<string> &parnames,vector<double> &parvals)
{
	
	parnames.clear();
	parvals.clear();
	if (this->currentworker)
		return this->currentworker->GetWorkerParameters(parnames,parvals);

	return 0;
}

}

#endif

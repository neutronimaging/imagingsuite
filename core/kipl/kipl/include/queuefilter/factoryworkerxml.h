//<LICENCE>

#ifndef __FACTORYWORKER_H
#define __FACTORYWORKER_H

//#include <misc/cparameters.h>
#include <queuefilter/basequeueworker.h>
#include <queuefilter/rankfilterworker.h>
#include <queuefilter/linearfilterworker.h>
#include <queuefilter/shockworker.h>
#include <queuefilter/ioworker.h>
#include <queuefilter/diffusionworker.h>
#include <queuefilter/maskworker.h>
#include <queuefilter/segmentationworker.h>
#include <queuefilter/modifierworker.h>
#include <queuefilter/thresholdworker.h>
#include <queuefilter/distanceworker.h>
#include <segmentation/mapupdater.h>
#include <segmentation/multiresseg.h>
#include <segmentation/fuzzykmeans.h>

#include <mxml.h>
#include <iostream>

namespace QueueFilter {

template <class ImgType>
class FactoryWorker : public BaseQueueWorker<ImgType>
{
protected:
	mxml_node_t *pars, *workerNode, *procTree;
public:
	FactoryWorker(mxml_node_t *xmltree, std::ostream & os=cout);
	int reset() {if (currentworker) delete currentworker; workerCnt=-1; return 0;}
	virtual int GetNextWorker(BaseQueueWorker<ImgType> ** worker);
	virtual int GetCurrentWorker(BaseQueueWorker<ImgType> ** worker);
	virtual int GetWorkerParameters(vector<string> &parnames,vector<double> &parvals);
	virtual int size() {return workerTotal;}
	~FactoryWorker();
protected:
	BaseQueueWorker<ImgType> *currentworker;
	virtual int ProduceWorker();
	virtual int insert(Image::CImage<ImgType,2> &img, Image::CImage<ImgType,2> &res)
	 {this->logstream<<"Insert should not be called in the factory worker"<<endl; return -2;}
	virtual int kernelsize() {return -1;}
	int CreateThresholdWorker(BaseQueueWorker<ImgType> **tmpWorker);
	int CreateDistanceWorker(BaseQueueWorker<ImgType> **tmpWorker);
	int CreateSegmentationWorker(BaseQueueWorker<ImgType> **tmpworker);
	
	int workerCnt;
	int workerTotal;
	Segmentation::SegmentationBase<ImgType,char,3> *topseg;
	Segmentation::SegmentationBase<ImgType,char,3> *seg;
	Segmentation::SegmentUpdateBase<ImgType,char,3> *updater;
};


template <class ImgType>
FactoryWorker<ImgType>::FactoryWorker(mxml_node_t *xmltree, std::ostream & os) : BaseQueueWorker<ImgType>(os)
{
	if (!xmltree) {
		cerr<<"XML parameters missing"<<endl;
		throw std::invalid_argument("XML parameters missing");
	}
	this->pars=xmltree;
	mxml_node_t *node;
	workerCnt=-1;
	workerTotal=0;
    this->procTree = mxmlFindElement(xmltree, xmltree, "processing", nullptr, nullptr, MXML_DESCEND);
	this->workerNode=this->procTree;
	
	if (this->procTree) {
	
		// Count number of defined workers
        for( node = mxmlFindElement(this->procTree, this->procTree, "worker", nullptr, nullptr, MXML_DESCEND);
         node != nullptr;
         node = mxmlFindElement(node, this->procTree, "worker", nullptr, nullptr, MXML_DESCEND))
			workerTotal++;
	}
	
    this->currentworker=nullptr;
    this->updater=nullptr;
    this->topseg=nullptr;
    this->seg=nullptr;
	
	this->workername="FactoryWorker";
 	this->isfactory=true;
}

template <class ImgType>
FactoryWorker<ImgType>::~FactoryWorker()
{
	if (this->currentworker)
		delete this->currentworker;
		
	if (this->seg)
		delete this->seg;
		
	if (this->topseg)
		delete this->topseg;
	
	if (this->updater)
		delete this->updater;	
}

	
template <class ImgType>
int FactoryWorker<ImgType>::GetNextWorker(BaseQueueWorker<ImgType> ** worker)
{
	workerCnt++;

    if ((workerNode = mxmlFindElement(workerNode, procTree, "worker", nullptr, nullptr, MXML_DESCEND))==nullptr) {
		this->logstream<<"Worker "<<workerCnt<<" could not be created, using current worker"<<endl;
		return GetCurrentWorker(worker);
	}
	
	int NIterations=ProduceWorker();
	
	*worker=this->currentworker;

	return NIterations;
}

template <class ImgType>
int FactoryWorker<ImgType>::GetCurrentWorker(BaseQueueWorker<ImgType> ** worker)
{
	*worker=(this->currentworker);	

	return workerCnt;
}

template <class ImgType>
int FactoryWorker<ImgType>::ProduceWorker()
{
	const char * classStr=mxmlElementGetAttr(workerNode,"class");
	if (!classStr) {
		this->logstream<<"FactoryWorker: worker description missing class name"<<endl;
		return -1;
	}
	string classstr=classStr;
	
	map<string,int> classmap;
	classmap["gaussian"]=0;
	classmap["median"]=1;
	classmap["rank"]=2;
	classmap["shock"]=3;
	classmap["diffusion"]=4;
	classmap["ioworker"]=5;
	classmap["maskworker"]=6;
	classmap["segmentation"]=7;
	classmap["modifier"]=8;
	classmap["threshold"]=9;
	classmap["distance"]=10;
	
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
	
	string tmpstr,valStr;
		
	switch (classmap[classstr]) {
		case 0 : {
					this->logstream<<"Creating a gaussian worker"<<endl; 
					const char *sigmaStr=mxmlElementGetAttr(workerNode,"sigma");
					sigma= !sigmaStr ? 1.0f : atof(sigmaStr);
						
					const char *supportStr=mxmlElementGetAttr(workerNode,"support");
					support= !supportStr ? 2.5f : atof(supportStr);
									
					NKernel2=(int)floor(support*sigma);
					sigma=sigma*sigma;
					sum=0.0f;
					for (i=0, j=-NKernel2; j<=NKernel2; i++,j++) {
						kernel[i]=exp(-j*j/(2*sigma));
						sum+=kernel[i];
					}
						
					for (i=0; i<2*NKernel2+1; i++) 
						kernel[i]/=sum;
						
					tmpworker=new LinearFilterWorker<float>(kernel,2*NKernel2+1, this->logstream);
				}
				break;
		case 1 : {
					this->logstream<<"Creating a median filter worker"<<endl; 
					const char * dimsStr=mxmlElementGetAttr(workerNode,"dims");
					
					if (dimsStr)
						string2vector(dimsStr,filtdimsvec);
						
					if (filtdimsvec.size()>2) {
						filtdims[0]=filtdimsvec[0];
						filtdims[1]=filtdimsvec[1];
						filtdims[2]=filtdimsvec[2];
					}
					else {
						this->logstream<<"FactoryWorker::ProduceWorker : Median filter dims are not specified, using 3x3x3"<<endl;
						filtdims[0]=3;
						filtdims[1]=3;
						filtdims[2]=3;
					}
					
					rank=(filtdims[0]*filtdims[1]*filtdims[2])/2;
					tmpworker=(BaseQueueWorker<ImgType> *)new RankFilterWorker<float>(filtdims,rank);
				}
				break;
		case 2 : {
					this->logstream<<"Creating a rank filter worker"<<endl; 
					
					const char * dimsStr=mxmlElementGetAttr(workerNode,"dims");
					
					if (dimsStr)
						string2vector(valStr,filtdimsvec);
						
					if (filtdimsvec.size()>2) {
						filtdims[0]=filtdimsvec[0];
						filtdims[1]=filtdimsvec[1];
						filtdims[2]=filtdimsvec[2];
					}
					else {
						this->logstream<<"FactoryWorker::ProduceWorker : Rank filter dims are not specified, using 3x3x3"<<endl;
						filtdims[0]=3;
						filtdims[1]=3;
						filtdims[2]=3;
					}
								
					const char * rankStr=mxmlElementGetAttr(workerNode,"rank");
					
					if (rankStr)
						rank=abs(atoi(rankStr));
					else
						rank=0;
											
					tmpworker=new RankFilterWorker<float>(filtdims,rank);
				}
				break;
		case 3 : {
					this->logstream<<"Creating a shock worker"<<endl; 
					const char * tauStr=mxmlElementGetAttr(workerNode,"tau");
					if (!tauStr)
						tau=0.125f;
					else
						tau=fabs(atof(tauStr));
						
					const char * sigmaStr=mxmlElementGetAttr(workerNode,"sigma");
					if (!sigmaStr)
						sigma=2.5f;
					else
						sigma=atof(sigmaStr);
						
					const char * amountStr=mxmlElementGetAttr(workerNode,"amount");
					if (!amountStr)
						amount=0.005f;
					else
						amount=atof(amountStr);
						
					const char * typeStr=mxmlElementGetAttr(workerNode,"type");
					if (!typeStr)
						st=Osher_Filter;
					else 
						if (!strcmp(typeStr,"alvarez"))
							st=Alvarez_Mazorra_Filter;
						else
							st=Osher_Filter;
							
					tmpworker=new ShockWorker<float>(tau, sigma, amount, st,this->logstream);
				}
				break;
		case 4 :{ 
					this->logstream<<"Creating a diffusion worker"<<endl; 
					const char *tauStr=mxmlElementGetAttr(workerNode,"tau");
					if (!tauStr)
						tau=0.125f;
					else
						tau=fabs(atof(tauStr));
						
					const char *sigmaStr=mxmlElementGetAttr(workerNode,"sigma");
					if (sigmaStr)
						sigma=2.5f;
					else
						sigma=atof(sigmaStr);
						
					const char *lambdaStr=mxmlElementGetAttr(workerNode,"lambda");
					if (!lambdaStr)
						lambda=0.005f;
					else
						lambda=atof(lambdaStr);
					
					tmpworker=new DiffusionWorker<float>(tau, sigma, lambda, this->logstream);
				}
				break;
				
		case 5 : { 
					this->logstream<<"Creating an IO worker"<<endl;
					const char *filemodeStr=mxmlElementGetAttr(workerNode,"file_mode");
					
					if (filemodeStr && !strcmp(filemodeStr,"block")) filemode=false;
					
					const char * iomodeStr=mxmlElementGetAttr(workerNode,"io_mode");
					if (iomodeStr && !strcmp(iomodeStr,"load")) iomode=true;
						
					const char * fnameStr=mxmlElementGetAttr(workerNode,"filename");
					if (!fnameStr) {
						this->logstream<<"Filename missing"<<endl;
						break;
					}
					
					tmpworker=new ioworker<ImgType>(fnameStr,iomode,filemode,this->logstream); 
				}
				break;
		
		case 6 : {
					this->logstream<<"Creating a maskworker"<<endl;
					
	
					const char * bgStr=mxmlElementGetAttr(workerNode,"background");	
					if (bgStr)
						maskval=atof(bgStr);
					else
						maskval=0;
					
					const char * typeStr=mxmlElementGetAttr(workerNode,"type");	
					if (typeStr && !strcmp(typeStr,"file")) {
						const char *fnameStr=mxmlElementGetAttr(workerNode,"filename");	
						if(!fnameStr) {
							this->logstream<<"MaskWorker initialization: Filename is not specified"<<endl;
							break;
						}
						
						tmpworker=new MaskWorker<ImgType>(fnameStr,(ImgType)maskval,this->logstream);
					}
					else {
						const char * parStr=mxmlElementGetAttr(workerNode,"parameters");	
						if (parStr)
							string2vector(parStr,shapepars);
						
						const char * shapeStr=mxmlElementGetAttr(workerNode,"shape");	
						if (!shapeStr)
							valStr="disk";
						else 
							valStr=shapeStr;
							
						tmpworker=new MaskWorker<ImgType>(valStr, shapepars,(ImgType)maskval,this->logstream);
					}
				}
				break;
				
		case 7 : CreateSegmentationWorker(&tmpworker); break;
				
		case 8 : {
					float scale, offset;
					const char * scaleStr=mxmlElementGetAttr(workerNode,"scale");	
					if (scaleStr)
						scale=atof(scaleStr);
					else
						scale=1;
						
					const char *offsetStr=mxmlElementGetAttr(workerNode,"offset");	
					if (offsetStr)
						offset=atof(offsetStr);
					else
						offset=0;
						
						
					vector<float> interv;
					const char *intervStr=mxmlElementGetAttr(workerNode,"limits");
					if (intervStr)
						string2vector(intervStr,interv);
					
					tmpworker=new ModifierWorker<ImgType>(scale,offset,interv,this->logstream);
				}
				break;
		case 9 : CreateThresholdWorker(&tmpworker); break;
		
		case 10 : CreateDistanceWorker(&tmpworker); break;
		
        default : this->logstream<<"Unknown worker, keeping previous"<<endl;tmpworker=nullptr;
	}
	
	if (tmpworker) {
		if (this->currentworker)
			delete this->currentworker;
		this->currentworker=tmpworker;
	}
	
	int Nit;
	const char *itStr=mxmlElementGetAttr(workerNode,"nit");	
	
	if (itStr)
		Nit=atoi(itStr);
	else
		Nit=1;
	
	const char *quietStr=mxmlElementGetAttr(workerNode,"quiet");
	
	if (quietStr) {
		valStr=quietStr;
		if ((valStr=="no") || (valStr=="NO") || (valStr=="No") ) {
			this->currentworker->Quiet(false);
			this->Quiet(false);
		}
	}
	else {
		this->currentworker->Quiet(true);
		this->Quiet(true);
	}
	
	const char *prognameStr=mxmlElementGetAttr(workerNode,"logimg");
	
	if (prognameStr) {
		string fname=prognameStr;
		this->currentworker->saveProgressImages(fname);
	}
	
	return Nit;
}

template <class ImgType>
int FactoryWorker<ImgType>::CreateThresholdWorker(BaseQueueWorker<ImgType> **tmpworker)
{
	*tmpworker=new ThresholdWorker<ImgType>(this->logstream);
					
	const char *opStr=mxmlElementGetAttr(workerNode,"operation");
	if (opStr) {
		string valStr=opStr;
		((ThresholdWorker<ImgType>*)(*tmpworker))->setOperation(valStr);
	}
		
	const char *valueStr=mxmlElementGetAttr(workerNode,"value");
	if (valueStr) {
		vector<ImgType> v;
		string2vector(valueStr,v);
		((ThresholdWorker<ImgType>*)(*tmpworker))->setThreshold(v);
	}
	return 0;
}

template <class ImgType>
int FactoryWorker<ImgType>::CreateDistanceWorker(BaseQueueWorker<ImgType> **tmpworker)
{
	*tmpworker=new DistanceWorker<ImgType>(this->logstream);
	const char *algStr=mxmlElementGetAttr(workerNode,"algorithm");
	if (algStr) {
	
		if (!strcmp(algStr,"chamfer")) {
			((DistanceWorker<ImgType>*)(*tmpworker))->setAlgorithm(string("chamfer"));
			const char *mStr=mxmlElementGetAttr(workerNode,"metric");
			if (mStr)
				((DistanceWorker<ImgType>*)(*tmpworker))->setMetric(mStr);
				
			const char *cStr=mxmlElementGetAttr(workerNode,"complement");
			if (cStr && (!strcmp(cStr,"yes"))) 
				((DistanceWorker<ImgType>*)(*tmpworker))->setComplement(true);
			else
				((DistanceWorker<ImgType>*)(*tmpworker))->setComplement(false);
		
			return 0;
		}
		
		if (!strcmp(algStr,"euclidean")) {
			((DistanceWorker<ImgType>*)(*tmpworker))->setAlgorithm(string("euclidean"));
			const char *connStr=mxmlElementGetAttr(workerNode,"conn");
			if (connStr)
				((DistanceWorker<ImgType>*)(*tmpworker))->setConnectivity(connStr);
			else
				((DistanceWorker<ImgType>*)(*tmpworker))->setConnectivity(string("conn26"));
			return 0;
		}
	}

	this->logstream<<"Algorithm is not specified, using chamfer with Svensson metric"<<endl;
	((DistanceWorker<ImgType>*)(*tmpworker))->setAlgorithm("chamfer");
	((DistanceWorker<ImgType>*)(*tmpworker))->setMetric("svensson");
	((DistanceWorker<ImgType>*)(*tmpworker))->setComplement(false);

	return 0;
}

template <class ImgType>
int FactoryWorker<ImgType>::CreateSegmentationWorker(BaseQueueWorker<ImgType> **tmpworker)
{
	if (this->seg)
		delete this->seg;
	if (this->updater)
		delete this->updater;
	if (this->topseg)
		delete this->topseg;
	
		
	const char * classStr=mxmlElementGetAttr(workerNode,"classes");	
	int nClasses=2;
	if (classStr)
		nClasses=abs(atoi(classStr));
	
	this->logstream<<"Creating a segmentationworker for "<<endl;
	
	this->topseg=new Segmentation::FuzzyCMeans<ImgType,char,3>(this->logstream);
	
	const char *fuzzyStr=mxmlElementGetAttr(workerNode,"fuzzy");	
	float fuzzy=1.25;
	if (fuzzyStr) {
		fuzzy=atoi(fuzzyStr);
	}
	((Segmentation::FuzzyCMeans<ImgType,char,3>*)(this->topseg))->set(nClasses, fuzzy);
	
	const char * updStr=mxmlElementGetAttr(workerNode,"updater");	
	string updaterStr="bayes";
	if (updStr)
		updaterStr=updStr;
		
	map<string,int> updaterMAP;
	updaterMAP["likelihood"]=0;
	updaterMAP["map"]=1;
	updaterMAP["bayes"]=2;
	switch (updaterMAP[updaterStr]) {
	case 0: 
		this->updater=new Segmentation::LikelihoodUpdater<ImgType,char,3>(nClasses,this->logstream);
		break;
	case 1: 
		this->updater=new Segmentation::MAPUpdaterNeighborhood<ImgType,char,3>(nClasses,this->logstream);
		break;
	case 2: {
			this->updater=new Segmentation::MAPUpdaterNeighborhood2<ImgType,char,3>(nClasses,this->logstream);
			const char * voterStr=mxmlElementGetAttr(workerNode,"voters");
			if (voterStr) {
				int nVoters=atoi(voterStr);
				((Segmentation::MAPUpdaterNeighborhood2<ImgType,char,3> *)(this->updater))->setVotingClasses(nVoters);
			}
		}
		break;
	default: 
		this->logstream<<"Factory::Create segmentation worker failed, unknown worker"<<endl;
		return -1;
	}
	
	//Segmentation::MAPUpdaterNeighborhood2<ImgType,char,3> updater(nClasses,logstream);
	this->seg=new Segmentation::MultiResolution<ImgType,char,3>(*(this->topseg),*(this->updater),this->logstream);
	
	this->seg->setClasses(nClasses);
	const char * samplerStr=mxmlElementGetAttr(workerNode,"sampler");
	if (samplerStr) {
		if (!strcmp(samplerStr,"gaussian")) {
			
			const char *sigmaStr=mxmlElementGetAttr(workerNode,"sigma");

			float sigma=1.0;
			if (sigmaStr)
				sigma=atof(sigmaStr);
				
			((Segmentation::MultiResolution<ImgType,char,3>*)this->seg)->setSampler(1,sigma);
		}
		else {
			((Segmentation::MultiResolution<ImgType,char,3>*)this->seg)->setSampler(0);
		}
	}
	else
		((Segmentation::MultiResolution<ImgType,char,3>*)this->seg)->setSampler(0);
		
	
	int nLevels=4;
	const char * levelStr=mxmlElementGetAttr(workerNode,"levels");
	if (levelStr)
		nLevels=atoi(levelStr);
	
	((Segmentation::MultiResolution<ImgType,char,3>*)this->seg)->setLevels(nLevels);
	
	
	*tmpworker=new SegmentationWorker<ImgType>(*(this->seg), this->logstream);
	
	return 0;
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

//<LICENCE>

#ifndef QUEUEFILTERQUEUEFILTERMANAGER_H
#define QUEUEFILTERQUEUEFILTERMANAGER_H

#include <base/timage.h>
#include <base/textractor.h>
#include <base/kiplenums.h>
#include <base/KiplException.h>
#include <logging/logger.h>
#include <vector>
#include "basequeueworker.h"
#include <io/io_matlab.h>

namespace akipl { namespace queuefilter {

/**
@author Anders Kaestner
*/
template <class ImgType>
class QueueFilterManager{
protected:
	BaseQueueWorker<ImgType> &worker; // Must be the first attibute due to initialization order
	kipl::logging::Logger logger;
public:
    QueueFilterManager(BaseQueueWorker<ImgType> & w, ostream & os=cout);
    
    virtual int operator()(kipl::base::TImage<ImgType,3> &img, int Niterations=1);
    bool Quiet(bool q=true) {worker.Quiet(q); this->quiet=q; return q;}
    ~QueueFilterManager(){}
	
protected:
	/** \brief Processing an image using a specified worker
		\param img A three dimensional image to be processed, the variable will contain the result upon successful completion
		\param Niterations Number of iterations to repeat the operation
		\param w The worker instance to use for the proccessing
	*/
	int ProcSingle(kipl::base::TImage<ImgType,3> &img, int Niterations, BaseQueueWorker<ImgType> & w);

	/** \brief Processing using a factory worker
		\param img The image to process using the operational workers produced by the factory
	*/
	virtual int ProcFactory(kipl::base::TImage<ImgType,3> &img);
	
	virtual int ExchangeBoundary(kipl::base::TImage<float,3> &img, BaseQueueWorker<ImgType> & w);
	int PrintFilterInfo(const size_t *dims,int Nit, BaseQueueWorker<ImgType> & w);

	bool mirror_top;
	bool mirror_bottom;
	std::vector<kipl::base::TImage<float,2> > top_boundary;
	std::vector<kipl::base::TImage<float,2> > bottom_boundary;
    bool quiet;
    long id_time;
};

template<class ImgType>
QueueFilterManager<ImgType>::QueueFilterManager(BaseQueueWorker<ImgType> & w, ostream & os): worker(w), logger("QueueFilterManager") 
{
	id_time=0L;

	mirror_top=true;
	mirror_bottom=true;
	worker.mirror_bottom=false;
	worker.mirror_top=false;
	quiet=true;
	ostringstream msg;
	msg<<"Manager initialized with a "<<worker.GetWorkerName();
	logger(kipl::logging::Logger::LogVerbose,msg.str());
		
}

template<class ImgType>
int QueueFilterManager<ImgType>::operator() (kipl::base::TImage<ImgType,3> &img, int Niterations)
{
	worker.setDims(img.Dims());

	if (worker.isFactory()) 
		return ProcFactory(img);
	else
		return ProcSingle(img,Niterations,worker);
}

template<class ImgType>
int QueueFilterManager<ImgType>::ProcFactory(kipl::base::TImage<ImgType,3> &img)
{
    BaseQueueWorker<ImgType> *currentWorker=nullptr;
	
	int Niterations;
	logger(kipl::logging::Logger::LogVerbose,"Factory starts");

	ostringstream msg;
	for (int i=0; i<worker.size(); i++) {
		if((Niterations=worker.GetNextWorker(&currentWorker))>-1) {
			msg.str("");
			msg<<"next worker "<<i<<":"<<currentWorker->GetWorkerName();
			logger(kipl::logging::Logger::LogVerbose,msg.str());

			Quiet(currentWorker->isQuiet());
			ProcSingle(img,Niterations,*currentWorker);
		}
	}
	logger(kipl::logging::Logger::LogVerbose,"Factory finishes");
	return 0;
}


template<class ImgType>
int QueueFilterManager<ImgType>::ProcSingle(kipl::base::TImage<ImgType,3> &img, int Niterations, BaseQueueWorker<ImgType> & w)
{
	int slice_index;
	size_t index;
	const size_t *dims=img.Dims();
	
	kipl::base::TImage<ImgType,2> slice;
	kipl::base::TImage<ImgType,2> res_slice(dims);
	
	w.setDims(dims);
	ostringstream msg;
	PrintFilterInfo(dims,Niterations,w);
	if (w.procBlock()) {
		w.proc(img);
	}
	else {
		for (int i=0; i<Niterations; i++) {
			ExchangeBoundary(img,w);
			
			index=0;
			
			logger(kipl::logging::Logger::LogVerbose,"Top processing");
				
			while (!top_boundary.empty()) {
				slice_index=w.insert(top_boundary.back(),res_slice);
				top_boundary.pop_back();
			}
			
			
			logger(kipl::logging::Logger::LogVerbose,"Main processing");

			try {
				for (index=0; index<dims[2]; index++) {
					
					
					slice=kipl::base::ExtractSlice(img,index,kipl::base::ImagePlaneXY);

					slice_index=w.insert(slice,res_slice);

					if ((slice_index>-1) && (slice_index>=0))
						kipl::base::InsertSlice(res_slice, img, slice_index, kipl::base::ImagePlaneXY);
						//img.InsertSlice(res_slice,plane_XY, slice_index);
				} 
			}
			catch (kipl::base::KiplException &e) {
				msg.str("");
				msg<<"Failed during slice processing on slice "<<index<<"("<<dims[2]<<") : \n"<<e.what();
				throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
			}
/*
			do {
				img.ExtractSlice(slice,plane_XY,index);
				index++;
				
				slice_index=w.insert(slice,res_slice);
				if (slice_index>=0)
					img.InsertSlice(res_slice,plane_XY, slice_index);
			} while (index<dims[2]);
	*/		
			
			logger(kipl::logging::Logger::LogVerbose,"Bottom processing ");

			while (!bottom_boundary.empty()) {
				slice_index=w.insert(bottom_boundary.back(),res_slice);
				bottom_boundary.pop_back();
				InsertSlice(res_slice, img, slice_index, kipl::base::ImagePlaneXY);
			}
				
			w.reset();
		}
		
	}
	
	return 1;
}

template <class ImgType>
int QueueFilterManager<ImgType>::ExchangeBoundary(kipl::base::TImage<float,3> &img, BaseQueueWorker<ImgType> & w )
{
	const int nk2=w.kernelsize()/2;
	
	// Stuff the end queues
	kipl::base::TImage<float,2> slice;
		
	const size_t *dims=img.Dims();
	
	top_boundary.clear();
	bottom_boundary.clear();	
	int i;
	
	for (i=nk2; i>0 ; i--) { // Bottom boundary
		slice=ExtractSlice(img, img.Size(2)-i-1, kipl::base::ImagePlaneXY);
		//img.ExtractSlice(slice,plane_XY,img.Size(2)-i-1);	
		bottom_boundary.push_back(slice);
	}
		
	for (i=1; i<=nk2 ; i++) { // Top boundary
		slice=ExtractSlice(img, i, kipl::base::ImagePlaneXY);
		//img.ExtractSlice(slice,plane_XY,i);	
		top_boundary.push_back(slice);
	}	
	
	if (mirror_top && mirror_bottom) return 0;

	return 1;
}

template <class ImgType>
int QueueFilterManager<ImgType>::PrintFilterInfo(const size_t *dims,int Nit, BaseQueueWorker<ImgType> & w)
{
	string workername;
	workername=w.GetWorkerName();
	
	vector<string> parnames;
	vector<double> parvals;
	w.GetWorkerParameters(parnames,parvals);
	ostringstream msg;
	msg<<"FilterInfo: "<<id_time
		<<" dims="<<dims[0]<<", "<<dims[1]<<", "<<dims[2]
		<<" Nit="<<Nit
		<<" boundary="<<w.kernelsize()/2
		<<" "<<workername<<": ";
	
	for (int i=0; i<parvals.size(); i++)
		msg<<parnames[i]<<"="<<parvals[i]<<" "<<flush;
		
	logger(kipl::logging::Logger::LogVerbose,msg.str());
		
	return 1;
}

}}

#endif

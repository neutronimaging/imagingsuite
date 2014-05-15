/***************************************************************************
 *   Copyright (C) 2005 by Anders Kaestner   *
 *   anders.kaestner@env.ethz.ch   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 ***************************************************************************/
#ifndef QUEUEFILTERBASEQUEUEWORKER_H
#define QUEUEFILTERBASEQUEUEWORKER_H

#include <base/timage.h>
#include <iostream>

namespace akipl {namespace queuefilter {

template <class ImgType>
class DummyQueueWorker;
/**
Base class for queue based filter workers

@author Anders Kaestner
*/
template <class ImgType>
class BaseQueueWorker{
protected:
	std::ostream &logstream;
public:
	/** \brief constructor that initializes the base worker
		\param os stream for the log messages
	*/
    BaseQueueWorker(std::ostream &os) : logstream(os){
    	quiet=true; 
    	mirror_top=true;
    	mirror_bottom=true;
    	workername="baseworker";
    	isfactory=false;
    	procblock=false;
    };

//	BaseQueueWorker(const BaseQueueWorker &w) {copy(w);}

//	const BaseQueueWorker & operator= (const BaseQueueWorker &w) {copy(w); return *this;}
    
    /// Reports if current instance is a factory of not
	bool isFactory() {return isfactory;}
	
	/// Reports if the image is to be processed as a block or slicewise
	bool procBlock() {return procblock;}
	
	/// Empty destructor
    virtual ~BaseQueueWorker() {};
    
    virtual int proc(kipl::base::TImage<ImgType,3> &img) { return -1;}
    virtual int insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res)=0;
    virtual int reset()=0;
    virtual int size()=0;
	virtual int kernelsize()=0;
    virtual bool Quiet(bool q) {
		this->quiet=q; 
		return q;
	}
	bool isQuiet() {return quiet;}

    /*!
        \fn QueueFilter::BaseQueueWorker::setDims(int *dims)
     */
	virtual int setDims(const unsigned int *Dims)
    {
        memcpy(dims,Dims,3*sizeof(int));
       	Ndata=dims[0]*dims[1];

       	return Ndata; 
    }

    bool mirror_top;
    bool mirror_bottom;
    const string & GetWorkerName() {return workername;}
    virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)=0;
    virtual int GetCurrentWorker(BaseQueueWorker<ImgType> ** w) { return -1;}
    virtual int GetNextWorker(BaseQueueWorker<ImgType> ** w) { return -1;}
    virtual int saveProgressImages(const string &name) {return -1;}
protected :
	string workername;
    size_t dims[3];
    long Ndata;
    bool quiet;
    bool isfactory;
    bool procblock;
};



/*
template<class ImgType>
int BaseQueueWorker<ImgType>::copy(const BaseQueueWorker &w)
{
	logstream=w.logstream;
	workername=w.workername;
    memcpy(dims,w.dims,3*sizeof(unsigned int));
    Ndata=w.Ndata;
    quiet=w.quiet;
    isfactory=w.isfactory;
    procblock=w.procblock;
    this->fig=w.fig;
    mirror_top=w.mirror_top;
    mirror_bottom=w.mirror_bottom;
}
*/
/** \brief Dummy worker needed to initialize the parallel processing manager
	@author Anders Kaestner
*/
template <class ImgType>
class DummyQueueWorker: public BaseQueueWorker<ImgType>
{
public:
    DummyQueueWorker(ostream &os=cout):BaseQueueWorker<ImgType>(os){}
	
    virtual ~DummyQueueWorker() {}
    virtual int insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res) {this->logstream<<"DummyWorker is called..."<<endl;return 0;}
    virtual int reset() {return 0;}
    virtual int size() {return 0;}
	virtual int kernelsize() {return 0;}
    virtual bool Quiet(bool q) {return 0;}

    /*!
        \fn QueueFilter::BaseQueueWorker::setDims(int *dims)
     */
	virtual int setDims(const int *Dims) {return 0;}
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals) {parnames.clear(); parvals.clear(); return 0;};
};
}}
#endif

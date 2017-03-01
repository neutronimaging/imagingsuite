//<LICENCE>

#ifndef __LINEARFITLERWORKER_H
#define __LINEARFITLERWORKER_H

#include <iostream>
#include <deque>
#include <base/timage.h>

#include "basequeueworker.h"

namespace akipl { namespace queuefilter {

template<class ImgType>
class LinearFilterWorker: public BaseQueueWorker<ImgType>
{
public:
	LinearFilterWorker(float *Kernel, int N, ostream &os=cout);
	int insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res);
	int reset(){
		cnt=-1; 
		res_cnt=-1; 
		queue.clear();
		bottom_queue.clear(); 
		bottom_reached=false;
		return 1;
	}
	~LinearFilterWorker() {
		if (this->kernel) delete [] this->kernel;
		if (this->pLine) delete [] this->pLine;
	}
	int size() {return queue.size();}
	virtual int kernelsize() {return Nkernel;}
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
private:
	int insertSlice(kipl::base::TImage<ImgType,2> &img);
	int filterSlice(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<float,2> &res);
	int filterQueue(kipl::base::TImage<float,2> &img);
	int printQimg();
	deque<kipl::base::TImage<float,2> > queue;
	deque<kipl::base::TImage<float,2> > bottom_queue;
	int cnt;
	int res_cnt;
	int bottom_edge;
	int top_edge;
	bool bottom_reached;
	//int dims[8];
	int dir;
	int Ndata;
	float *kernel;
	float **pLine;
	int Nkernel;
};

template<class ImgType>
LinearFilterWorker<ImgType>::LinearFilterWorker(float *Kernel, int N, ostream &os): BaseQueueWorker<ImgType>(os)
{
	cnt=-1;
	res_cnt=-1;
	this->kernel=new float[N];
	this->pLine=new float*[N];
	memcpy(this->kernel,Kernel,N*sizeof(float));
	Nkernel=N;
	top_edge=N/2;
	queue.clear();
	bottom_reached=false;
	this->mirror_top=true;
	this->mirror_bottom=true;
	this->workername="LinearWorker";
}

template<class ImgType>
int LinearFilterWorker<ImgType>::insert(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<ImgType,2> &res) 
{
	if (res_cnt>=int(this->dims[2]-1))
		return -2;
	
	insertSlice(img);

	if (queue.size()<Nkernel) 
		return res_cnt;
	
	filterQueue(res);

	res_cnt++;

	return res_cnt;
}

template<class ImgType>
int LinearFilterWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear();
	parnames.push_back(string("NKernel"));
	parvals.clear();
	parvals.push_back(Nkernel);
	
	return 1;
}

template<class ImgType>
int LinearFilterWorker<ImgType>::insertSlice(kipl::base::TImage<ImgType,2> &img)
{
	kipl::base::TImage<float,2> fimg;
	cnt++;
	
	filterSlice(img,fimg);

	queue.push_back(fimg);
	if (queue.size()<=Nkernel) 
		return 0;
	else {
		queue.pop_front();
		return 1;
	}

	/*
	// start new 20060104
	if (queue.size()<Nkernel) {
		if (!queue.empty() && mirror_top) {
			queue.push_front(fimg);
			queue.push_back(fimg);
		}
		else
			queue.push_back(fimg);
			
		return 0;
	} 
	
	queue.pop_front();
	queue.push_back(fimg);
	
	// stop new 20060104
	*/
	// removed 20060104
	if (cnt<int(this->dims[2])) {
		queue.push_back(fimg);
		if (queue.size()<Nkernel) 
			return 0;
		else
			return 1;
	}
	else {
		if (this->mirror_bottom) {
			if (!bottom_reached) {
				copy(queue.begin()+Nkernel/2+1,
					queue.end(),
					front_inserter(bottom_queue));
				bottom_reached=true;
			}
			
			if (!bottom_queue.empty()) {
				queue.push_back(bottom_queue.front());
				bottom_queue.pop_front();
				queue.pop_front();
			}
		}
		else {
			filterSlice(img,fimg);
			queue.pop_front();
			queue.push_back(fimg);
		}
	}
/* // Removed 20060802
	if (cnt<int(this->dims[2])) {

		filterSlice(img,fimg);
		if (queue.size()<Nkernel) {
			if (!queue.empty() && this->mirror_top) {
				queue.push_front(fimg);
				queue.push_back(fimg);
			}
			else
				queue.push_back(fimg);
				
			return 0;
		} 
		
		queue.pop_front();
		queue.push_back(fimg);
		
		bottom_edge=Nkernel;
	}
	else {
		if (this->mirror_bottom) {
			if (!bottom_reached) {
				copy(queue.begin()+Nkernel/2+1,
					queue.end(),
					front_inserter(bottom_queue));
				bottom_reached=true;
			}
			
			if (!bottom_queue.empty()) {
				queue.push_back(bottom_queue.front());
				bottom_queue.pop_front();
				queue.pop_front();
			}
		}
		else {
			filterSlice(img,fimg);
			queue.pop_front();
			queue.push_back(fimg);
		}
	}
*/
	return 1;
}

template<class ImgType>
int LinearFilterWorker<ImgType>::filterQueue(kipl::base::TImage<float,2> &img)
{
	int i;
	size_t j;

	for (i=0; i<Nkernel; i++) 
		pLine[i]=queue[i].GetDataPtr();
	
	img.Resize(this->dims);

	float *pImg=img.GetDataPtr();

	for (j=0; j<img.Size(); j++) {
		pImg[j]=0;
		
		for (i=0; i<Nkernel; i++) 
			pImg[j]+=this->kernel[i]*pLine[i][j];
	}

	return 0;
}

template<class ImgType>
int LinearFilterWorker<ImgType>::printQimg()
{
	this->logstream<<"["<<cnt<<"]"<<endl;
	for (int i=0; i<queue.size(); i++)
		this->logstream<<i<<"-["<<queue[i].Size()<<"], "<<flush;
	this->logstream<<endl<<endl;

	return 0;
}

template<class ImgType>
int LinearFilterWorker<ImgType>::filterSlice(kipl::base::TImage<ImgType,2> &img, kipl::base::TImage<float,2> &res)
{

	int i,k,m;

	size_t j;

	kipl::base::TImage<float,2> tmp(img.Dims());
	res.Resize(this->dims);
	deque<float> lineQ;

	ImgType *pImg;
	float *pTmp,*pRes;
	int Nk2=Nkernel/2;
	
	for (j=0; j<this->dims[1];j++){
		pImg=img.GetLinePtr(j);
		pTmp=tmp.GetLinePtr(j);
		// init x-direction
		lineQ.clear();
		lineQ.push_back(pImg[0]);
		i=1;
		while (lineQ.size()<Nkernel) {
			lineQ.push_back(pImg[i]);
			lineQ.push_front(pImg[i]);
			i++;
		}

		for (k=0 ; i<this->dims[0]; i++,k++) {
			pTmp[k]=0.0f;
			for (m=0; m<Nkernel; m++)
				pTmp[k]+=kernel[m]*lineQ[m];
			lineQ.pop_front();
			lineQ.push_back(pImg[i]);
		}
		
		for ( i=img.Size(0)-2; k<this->dims[0]; i--,k++) {
			pTmp[k]=0.0f;
			for (m=0; m<Nkernel; m++)
				pTmp[k]+=kernel[m]*lineQ[m];
			lineQ.pop_front();
			lineQ.push_back(pImg[i]);
		}
		
	}

	
	// Process y direction
	deque<float *> q2d;
	
	q2d.clear();
	q2d.push_back(tmp.GetLinePtr(0));
	
	i=1;
	while (q2d.size()<Nkernel) {
		pImg=tmp.GetLinePtr(i);
		q2d.push_back(pImg);
		q2d.push_front(pImg);
		i++;
	}
	int l;
	for (j=0; i<this->dims[1]; j++,i++) {
		pRes=res.GetLinePtr(j);
		q2d.push_back(tmp.GetLinePtr(i));
		for (l=0; l<this->dims[0]; l++) {
			pRes[l]=0;
			for (k=0; k<Nkernel; k++)
				pRes[l]+=q2d[k][l]*this->kernel[k];
		}
		q2d.pop_front();
	}

	for (i=this->dims[1]-2 ; j<this->dims[1]; j++,i--) {
		pRes=res.GetLinePtr(j);
		q2d.push_back(tmp.GetLinePtr(i));
		for (l=0; l<this->dims[0]; l++) {
			pRes[l]=0;
			for (k=0; k<Nkernel; k++)
				pRes[l]+=q2d[k][l]*this->kernel[k];
		}
		q2d.pop_front();
	}
	

	return 0;
}

}}
#endif

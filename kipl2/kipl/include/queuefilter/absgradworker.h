//<LICENCE>

#ifndef __ABSGRADWORKER_H
#define __ABSGRADWORKER_H

#include <iostream>
#include <deque>
#include <image/image.h>

#include "basequeueworker.h"

using namespace std;
using namespace Image;
namespace QueueFilter {

template<class ImgType>
class AbsGradWorker: public BaseQueueWorker<ImgType>
{
public:
	AbsGradWorker();
	int insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res);
	int reset(){
		cnt=-1; 
		res_cnt=-1; 
		queue.clear();
		bottom_queue.clear(); 
		bottom_reached=false;
		return 1;
	}
	~AbsGradWorker() {
		if (this->pLine) delete [] this->pLine;
	}
	int size() {return queue.size();}
	virtual int kernelsize() {return Nkernel;}
private:
	int insertSlice(CImage<ImgType,2> &img);
	int filterSlice(CImage<ImgType,2> &img, CImage<float,2> &res);
	int filterQueue(CImage<float,2> &img);
	int printQimg();
	deque<CImage<float,2> > queue;
	deque<CImage<float,2> > bottom_queue;
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
	const float binK[3]={0.25f,0.5f,0.25f};
	const float diffK[3]={-0.5f,0.0f,0.5f};
};

template<class ImgType>
AbsGradWorker<ImgType>::AbsGradWorker()
{
	cnt=-1;
	res_cnt=-1;
	this->pLine=new float*[3];
	memcpy(this->kernel,Kernel,N*sizeof(float));
	Nkernel=3;
	top_edge=1;
	queue.clear();
	bottom_reached=false;
	mirror_top=true;
	mirror_bottom=true;
}

template<class ImgType>
int AbsGradWorker<ImgType>::insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res) 
{
	if (res_cnt>=dims[2]-1)
		return -2;
	
	insertSlice(img);

	if (queue.size()<Nkernel) 
		return res_cnt;
	
	filterQueue(res);
	res_cnt++;

	return res_cnt;
}


template<class ImgType>
int AbsGradWorker<ImgType>::insertSlice(CImage<ImgType,2> &img)
{
	CImage<float,2> fimg;
	cnt++;
	
	if (cnt<dims[2]) {
		filterSlice(img,fimg);
		
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
		
		/*
		if (cnt!=top_edge) {
			queue.pop_front();
			queue.push_back(fimg);
		}
		else 
			queue.push_back(fimg);
		*/
		bottom_edge=Nkernel;
	}
	else {
		if (mirror_bottom) {
			if (!bottom_reached) {
				copy(queue.begin()+Nkernel/2+1,queue.end(),front_inserter(bottom_queue));
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
/*	
	if (cnt++<dims[2]) {
		filterSlice(img,fimg);
		if (!cnt) {
			queue[top_edge]=fimg;
			queue[top_edge-1]=fimg;
			
			return 0;
		}
		
		if (cnt<top_edge) {
			queue[top_edge+cnt]=fimg;
			queue[top_edge-cnt-1]=fimg;
			return 0;
		}
		else {
			if (cnt!=top_edge) {
				queue.pop_front();
				queue.push_back(fimg);
			}
			else {
				queue[top_edge+cnt]=fimg;
			}
		}
	
		bottom_edge=Nkernel;
	}
	else 
		queue.push_back(queue[--bottom_edge]);
*/
	return 1;
}

template<class ImgType>
int AbsGradWorker<ImgType>::filterQueue(CImage<float,2> &img)
{
	int i,j,k;

	for (i=0; i<Nkernel; i++) 
		pLine[i]=queue[i].getDataptr();
	
	img.resize(dims);

	float *pImg=img.getDataptr();

	for (j=0; j<img.N(); j++) {
		pImg[j]=0;
		
		for (i=0; i<Nkernel; i++) 
			pImg[j]+=this->kernel[i]*pLine[i][j];
	}

	return 0;
}

template<class ImgType>
int AbsGradWorker<ImgType>::printQimg()
{
	cout<<"["<<cnt<<"]"<<endl;
	for (int i=0; i<queue.size(); i++)
		cout<<i<<"-["<<queue[i].N()<<"], "<<flush;
	cout<<endl<<endl;

	return 0;
}

template<class ImgType>
int AbsGradWorker<ImgType>::filterSlice(CImage<ImgType,2> &img, CImage<float,2> &res)
{

	int i,j,k,m;

	CImage<float,2> tmp(img.getDimsptr());
	res.resize(dims);
	deque<float> lineQ;

	ImgType *pImg;
	float *pTmp,*pRes;
	int Nk2=Nkernel/2;
	
	for (j=0; j<dims[1];j++){
		pImg=img.getLineptr(j);
		pTmp=tmp.getLineptr(j);
		// init x-direction
		lineQ.clear();
		lineQ.push_back(pImg[0]);
		i=1;
		while (lineQ.size()<Nkernel) {
			lineQ.push_back(pImg[i]);
			lineQ.push_front(pImg[i]);
			i++;
		}

		for (k=0 ; i<dims[0]; i++,k++) {
			pTmp[k]=0.0f;
			for (m=0; m<Nkernel; m++)
				pTmp[k]+=kernel[m]*lineQ[m];
			lineQ.pop_front();
			lineQ.push_back(pImg[i]);
		}

		for ( ; k<dims[0]; i--,k++) {
			pTmp[k]=0.0f;
			for (m=0; m<Nkernel; m++)
				pTmp[k]+=kernel[m]*lineQ[m];
			lineQ.pop_front();
			lineQ.push_back(pImg[i]);
		}

	}

	// Process y direction
	deque<float *> q2d;
	
	for (i=Nk2; i>=0; i--)
		q2d.push_back(tmp.getLineptr(i));

	for (i=1; i<Nk2; i++)
		q2d.push_back(tmp.getLineptr(i));
	
	for (j=0; j<dims[1]-Nk2; j++) {
		pRes=res.getLineptr(j);
		q2d.push_back(tmp.getLineptr(j+Nk2));
		for (i=0; i<dims[0]; i++) {
			pRes[i]=0;
			for (k=0; k<Nkernel; k++)
				pRes[i]+=q2d[k][i]*this->kernel[k];
		}
		q2d.pop_front();
	}
	
	for (i=dims[1]-1; i>=dims[1]-Nk2; i--)
		q2d.push_back(tmp.getLineptr(i));
	
	for (; j<dims[1]; j++) {
		pRes=res.getLineptr(j);
	
		for (i=0; i<dims[0]; i++) {
			pRes[i]=0;
			for (k=0; k<Nkernel; k++)
				pRes[i]+=q2d[k][i]*this->kernel[k];
		}
		q2d.pop_front();
	}
	
	return 0;
}

}
#endif

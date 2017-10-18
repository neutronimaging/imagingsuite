//<LICENCE>

#ifndef __RANKFILTERWORKER_H
#define __RANKFILTERWORKER_H

#include <iostream>
#include <deque>
#include <vector>
#include <set>
#include <image/image.h>

#include "baseneighborhoodoperatorworker.h"

using namespace std;
using namespace Image;

namespace QueueFilter {

template<class ImgType>
class RankFilterWorker: public BaseNeighborhoodOperatorWorker<ImgType>
{
public:
	RankFilterWorker(int *FilterDims, int Rank, ostream &os=cout);
	
	virtual int Process(ImgType *resultP);
	virtual ~RankFilterWorker() {}
	
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
	int ChangeRank(int R) {if ((R>=0) && (R<this->filterN)) rank=R; else rank=0; return rank; }
protected:
	int rank;
};

template<class ImgType>
RankFilterWorker<ImgType>::RankFilterWorker(int *FilterDims, int Rank, ostream &os) : BaseNeighborhoodOperatorWorker<ImgType>(FilterDims,os) 
{
	ChangeRank(Rank);
	this->workername="rankfilter worker";
}

template<class ImgType>
int RankFilterWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear(); parvals.clear();
	parnames.push_back(string("rank")); parvals.push_back(rank);
	parnames.push_back(string("dimx")); parvals.push_back(this->filterdims[0]);
	parnames.push_back(string("dimy")); parvals.push_back(this->filterdims[1]);
	parnames.push_back(string("dimz")); parvals.push_back(this->filterdims[2]);
	
	return 1;
}

template<class ImgType>
int RankFilterWorker<ImgType>::Process(ImgType *resultP)
{
	const int xdim=this->filterdims[0]/2;
	const int ydim=this->filterdims[1]/2;
	const int zdim=this->filterdims[2]/2;
	ImgType *pLine;
	int x,y,z,r,k,i,j;

	multiset<ImgType> rankSet;
	typename multiset<ImgType>::iterator rankIt;
	for (i=0, j=-xdim; i<this->dims[0]-xdim ; i++, j++) {
		for (z=0; z<this->filterdims[2]; z++) {
			for (y=0; y<this->filterdims[1]; y++) {
				pLine=this->lineptrQ[z][y];
				for (x=0; x<this->filterdims[1]; x++) 
					rankSet.insert(pLine[abs(j+x)]);
			}
		}
		for (r=0, rankIt=rankSet.begin(); (r<rank) && (rankIt!=rankSet.end()) ; r++, rankIt++);

		resultP[i]=*rankIt;
		rankSet.clear();
	}
	
	for (; i<this->dims[0]; i++, j++) {
		for (z=0; z<this->filterdims[2]; z++) {
			for (y=0; y<this->filterdims[1]; y++) {
				pLine=this->lineptrQ[z][y];
				for (x=0; x<this->filterdims[1]; x++) {
					k=(j+x);
					if (this->dims[0]-k<=0)
						k=((this->dims[0]-1)<<1)-k;
					rankSet.insert(pLine[k]);
				}
			}
		}
		for (r=0, rankIt=rankSet.begin(); (r<rank) && (rankIt!=rankSet.end()) ; r++, rankIt++);

		resultP[i]=*rankIt;
		rankSet.clear();
	}
	
	return 0;
}

}

#endif


/***************************************************************************
 *   Copyright (C) 2005 by Anders Kaestner   *
 *   anders.kaestner@env.ethz.ch   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __BASENEIGHBORHODDOPERATORWORKER_H
#define __BASENEIGHBORHODDOPERATORWORKER_H

#include <iostream>
#include <deque>
#include <vector>
#include <image/image.h>

#include "basequeueworker.h"

using namespace std;
using namespace Image;
namespace QueueFilter {

template<class ImgType>
class BaseNeighborhoodOperatorWorker: public BaseQueueWorker<ImgType>
{
public:
	BaseNeighborhoodOperatorWorker(int *FilterDims, ostream &os=cout);
	virtual int insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res);
	virtual int reset(){
		cnt=-1; 
		queue.clear();
		return 1;
	}
	virtual ~BaseNeighborhoodOperatorWorker() {}
	virtual int Process(ImgType *resultP)=0;
	
	int size() {return queue.size();}
	virtual int kernelsize() {return filterdims[2];}
	virtual int GetWorkerParameters(vector<string> & parnames, vector<double> & parvals);
protected:
	int UpdateLineQ(int N);
	deque<vector<ImgType *> > lineptrQ;
	deque<CImage<ImgType,2> > queue;
	int filterdims[3];
	int filterN;
	int cnt;
};

template<class ImgType>
BaseNeighborhoodOperatorWorker<ImgType>::
	BaseNeighborhoodOperatorWorker(int *FilterDims, ostream &os): BaseQueueWorker<ImgType>(os) 
{
	filterdims[0]=FilterDims[0];
	filterdims[1]=FilterDims[1];
	filterdims[2]=FilterDims[2];
	filterN=filterdims[0]*filterdims[1]*filterdims[2];
	this->workername="baseNeighborhoodWorker";
	reset();
}

template<class ImgType>
int BaseNeighborhoodOperatorWorker<ImgType>::GetWorkerParameters(vector<string> & parnames, vector<double> & parvals)
{
	parnames.clear(); parvals.clear();
	parnames.push_back(string("dimx")); parvals.push_back(filterdims[0]);
	parnames.push_back(string("dimy")); parvals.push_back(filterdims[1]);
	parnames.push_back(string("dimz")); parvals.push_back(filterdims[2]);
	
	return 1;
}

template<class ImgType>
int BaseNeighborhoodOperatorWorker<ImgType>::insert(CImage<ImgType,2> &img, CImage<ImgType,2> &res)
{
	queue.push_back(img);
	
	if (queue.size()==filterdims[2]) {
		lineptrQ.clear();
		
		// Initialize lineQ
		int i=-filterdims[1]/2;
		int j=0;
		while (i<int(this->dims[1])) {
			if (UpdateLineQ(abs(i))==filterdims[1]) 
				Process(res.getLineptr(j++));
			i++;
		}
		while (j<this->dims[1]) {
			i--;
			UpdateLineQ(i);
			Process(res.getLineptr(j++));
		}

		queue.pop_front();
		cnt++;
	}

	return cnt;
}

template <class ImgType>
int BaseNeighborhoodOperatorWorker<ImgType>::UpdateLineQ(int N)
{
	vector<ImgType *> lineVec;
	typename deque<CImage<ImgType,2> >::iterator itQ;
	if (lineptrQ.size()==filterdims[1])
		lineptrQ.pop_front();
		
	for (itQ=queue.begin(); itQ!=queue.end() ; itQ++) 
		lineVec.push_back(itQ->getLineptr(N));
	
	lineptrQ.push_back(lineVec);
	
	return lineptrQ.size();
}


}

#endif


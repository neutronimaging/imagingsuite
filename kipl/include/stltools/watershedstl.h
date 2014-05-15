/***************************************************************************
 *   Copyright (C) 2006 by Anders Kaestner   *
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
#ifndef __WATERSHEDSTL_H
#define __WATERSHEDSTL_H
#include "geodesicstl.h"
#include "stlvecmath.h"
//#include "../misc/sortimg.h"
#include <vector>
#include <map>

namespace kipl { namespace STLmorphology {

template <class T1, class WsType>
int watershed(vector<T1> &vec, vector<WsType> &ws)
{
	/*
	if (!numeric_limits<WsType>::is_signed) {
		cerr<<"Watershed require signed data types for the label vector"<<endl;
		return -1;
	}

	const WsType mask=-2;
	const WsType wshed=0;
	const WsType init=-1;
	const WsType inqueue=-3;

	const WsType max_label=numeric_limits<WsType>::max();
	
	const size_t length=vec.size();
	
	ws.resize(length);

	fill(ws.begin(),ws.end(),init);

	int current_label=0;
	bool flag=false;

	SupportTools::SortImg<T1,long> ws_sort;
	
	// sort pixels
	std::deque<int> fifo;
	
	long i;
	
	ws_sort(vec);

	// Do geodesic SKIZ of level h-1 inside level h	
	long pos,png;
	Morphology::CNeighborhood NG(&length,1);
	int Nng=NG.N();
	T1 h, h_old;

	long total_cnt=ws_sort.size();



	for (ws_sort.resetLevel(); !ws_sort.lastLevel(); ws_sort.nextLevel()) {
		h=ws_sort.getValue();

		for (ws_sort.resetData(); !ws_sort.lastData(); ws_sort.nextData()) {
			pos=ws_sort.getData();
			
			ws[pos]= mask;
			for (i=0; i<Nng; i++) {
				if (((png=NG.neighbor(pos,i))!=-1) && (ws[png]>=wshed)) {
					ws[pos]=inqueue;
					fifo.push_front(pos);
					break;
				}
			}
			
		}

		while (!fifo.empty()) {
			pos=fifo.front();
			fifo.pop_front();
			for (i=0; i<Nng; i++) {
				if (((png=NG.neighbor(pos,i))!=-1)) 
					if ((ws[png]>0)) {
							if ((ws[pos]==inqueue) || ((ws[pos]==wshed) && (flag==true) ))
								ws[pos]=ws[png];
							else {
								if ((ws[pos]>0) && (ws[pos]!=ws[png])) {
									ws[pos]=wshed;
									flag=false;
								}
							}
					}
					else {
						if ((ws[png]==wshed) && (ws[pos]==inqueue)) {
							ws[pos]=wshed;
							flag=true;
						}
						else {
							if (ws[png]==mask) {
								ws[png]=inqueue;
								fifo.push_front(png);
							}
						}
					}
			}
		}
		
		for (ws_sort.resetData(); !ws_sort.lastData(); ws_sort.nextData()) {
			pos=ws_sort.getData();
			if (ws[pos]==mask) {
				if (++current_label==max_label) 
					cerr<<"Watershed: Warning max label used"<<endl;
				fifo.push_front(pos);
				ws[pos]=current_label;
				
				while (!fifo.empty()) {
					pos=fifo.front();
					fifo.pop_front();
					for (i=0; i<Nng; i++) {
						if (((png=NG.neighbor(pos,i))!=-1) && (ws[png]==mask)) {
							ws[png]=current_label;
							fifo.push_front(png);
						}
					}
				}
			}
		}
		
	}
*/
	return 0;
}

}}

#endif

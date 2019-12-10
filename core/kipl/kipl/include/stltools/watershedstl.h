//<LICENCE>

#ifndef __WATERSHEDSTL_H
#define __WATERSHEDSTL_H

#include <vector>
#include <map>

#include "geodesicstl.h"
#include "stlvecmath.h"
//#include "../misc/sortimg.h"

namespace kipl { namespace STLmorphology {

template <class T1, class WsType>
int watershed(std::vector<T1> &vec, std::vector<WsType> &ws)
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

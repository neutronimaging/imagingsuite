//<LICENCE>

#ifndef CLASSGROWING_HPP_
#define CLASSGROWING_HPP_

#include <list>
#include <sstream>

#include "../../morphology/morphology.h"
#include "../../logging/logger.h"

namespace akipl { namespace segmentation {

template <class T, size_t N>
int ClassGrow(kipl::base::TImage<T,N> &img, size_t nClasses, const T voidvalue)
{
    kipl::logging::Logger logger("ClassGrow");
    std::stringstream msg;
	std::list<long long int> voidlist;
	std::list<pair<long long int, T> > replacelist;
	long long int i=0;
	long long int pos=0;
	T *pImg=img.GetDataPtr();
	
	for (i=0; i< img.Size(); i++) {
		if (pImg[i]==voidvalue)
			voidlist.push_back(i);
	}
    msg.str("");
    msg<<"Found "<<voidlist.size()<<" elements in void region";
    logger(kipl::logging::Logger::LogVerbose,msg.str());
    
	int *cnt=new int[nClasses+1]; 
	std::list<long long int>::iterator it;
	kipl::morphology::CNeighborhood neighborhood(img.Dims(), N, N==2 ? kipl::morphology::conn8 : kipl::morphology::conn26);

    int nIterations=0;
    while (!voidlist.empty()) {
		// Analyze neighborhood of each pixel
		for (it=voidlist.begin(); it!=voidlist.end(); i++) {
			memset(cnt,0, sizeof(int)*nClasses);
			for (i=0; i<neighborhood.N(); i++) {
				if ((pos=neighborhood.neighbor(*it,i))!=-1) {
					cnt[static_cast<int>(pImg[pos])]++;
				}				
			}
			
			// Identify majority class
			int major=0;
			for (i=1; i<nClasses; i++) {
				if (cnt[major]<cnt[i])
					major=i;
			}
			if (major!=voidvalue) {
				replacelist.push_back(make_pair(*it,major));
				it=voidlist.erase(it);
			}
		}
	
        msg.str("");
        msg<<"Iteration "<<nIterations<<": Placed "<<replacelist.size()<<" elements in the replace list ("<<voidlist.size()<<" are still in the voidlist)";
        logger(kipl::logging::Logger::LogVerbose,msg.str());
            
        
		// Replace assigned pixels 
		while (!replacelist.empty()) {
			pImg[replacelist.front().first]=replacelist.front().second;
			replacelist.pop_front();
		}
        nIterations++;
	}
	
	return 0;
}

}}

#endif /*CLASSGROWING_HPP_*/

//<LICENCE>

#ifndef __ZEROCROSSINGS_H
#define __ZEROCROSSINGS_H

#include <vector>
#include <iostream>
#include <algorithm>

template <class ForwardIterator>
int FindZeroCrossings(const ForwardIterator &begin,const ForwardIterator &end, std::vector<ForwardIterator> &pos)
{
	pos.clear();
	
	ForwardIterator prev=begin;
	ForwardIterator it=begin;
	it++;
	for ( ; it!=end; it++, prev++) {
		if (((*prev>0) && (*it<=0)) || ((*prev<=0) && (*it>0))) 
			pos.push_back(prev);
	}
	
	return pos.size();
}

template <class ForwardIterator, class T>
int Diff(const ForwardIterator &begin,const ForwardIterator &end, std::vector<T> &diff, bool forward=true)
{
	diff.clear();
	
	ForwardIterator prev=begin;
	ForwardIterator it=begin;
	it++;
	if (!forward)
		diff.push_back(T(0));
	
	for ( ; it!=end; it++, prev++) 
		diff.push_back(*it-*prev);	
	
	if (forward)
		diff.push_back(T(0));
		
	return diff.size();
}

template <class ForwardIterator>
int FindPeaks(const ForwardIterator &begin,const ForwardIterator &end, std::vector<ForwardIterator> &pos)
{
    std::vector<double> diff;
	Diff(begin,end,diff,true);

	FindZeroCrossings(diff.begin(),diff.end(),pos);	
    typename std::vector<ForwardIterator>::iterator it;
	for (it=pos.begin(); it!=pos.end(); it++) {
		*it=*it-diff.begin()+begin;
	}
	
	return pos.size();
}

template <class T>
std::vector<std::pair<ptrdiff_t,ptrdiff_t>> findContinuousBlocks(std::vector<T> &mask, float threshold = 0.5f)
{
    std::vector<std::pair<ptrdiff_t,ptrdiff_t>> blocks;
    auto it = mask.begin();

    do
    {
        while ( (*it<threshold) && (it!=mask.end()) ) ++it;

        if (it==mask.end())
            break;

        ptrdiff_t rise = it - mask.begin();

        while ( (threshold<*(it)) && (it!=mask.end()) ) ++it;

        ptrdiff_t fall = it - mask.begin();
        blocks.push_back(std::make_pair(rise,fall));
    }
    while ( it!=mask.end() );

    return blocks;
}

#endif

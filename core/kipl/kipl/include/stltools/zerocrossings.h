//<LICENCE>

#ifndef __ZEROCROSSINGS_H
#define __ZEROCROSSINGS_H

template <class ForwardIterator>
int FindZeroCrossings(const ForwardIterator &begin,const ForwardIterator &end, vector<ForwardIterator> &pos)
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
int Diff(const ForwardIterator &begin,const ForwardIterator &end, vector<T> &diff, bool forward=true)
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
int FindPeaks(const ForwardIterator &begin,const ForwardIterator &end, vector<ForwardIterator> &pos)
{
	vector<double> diff;
	Diff(begin,end,diff,true);

	FindZeroCrossings(diff.begin(),diff.end(),pos);	
	typename vector<ForwardIterator>::iterator it;
	for (it=pos.begin(); it!=pos.end(); it++) {
		*it=*it-diff.begin()+begin;
	}
	
	return pos.size();
}


#endif

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
#ifndef __GEODESICSTL_H
#define __GEODESICSTL_H
//#include "../visualization/gnuplot_i.h"
#include <vector>
#include <iostream>
#include <algorithm>

namespace kipl { namespace STLmorphology {

template <class T>
int RecByDilation(std::vector<T> & g, std::vector<T> & f)
{
	std::cout<<"RecByDIlation: Must be enhance with loop "<<std::endl;

	if (g.size()!=f.size()) {
		std::cerr<<"RecByDilation: size(g)!=size(f)"<<std::endl;
		return 0;
	}
	
	if (f>g) {
		std::cerr<<"RecByDilation: f must be less or equal to g"<<std::endl;
		return 0;
	}

	T maxf;
	typename std::vector<T>::iterator fpos, gpos,tpos;
	fpos=f.begin();
	gpos=g.begin();
	*fpos=min(*fpos,*gpos);
	T prev=*fpos;
	fpos++; gpos++;
	
	for (; fpos!=f.end(); fpos++, gpos++) {
		maxf=max(prev,*fpos);
		*fpos=min(*gpos,maxf);
		prev=*fpos;
	}

	fpos=f.end(); fpos--;
	gpos=g.end(); gpos--;
	
	*fpos=min(*fpos,*gpos);
	prev=*fpos;
	fpos--; gpos--;
	for ( ; fpos!=f.begin(); fpos--, gpos--) {
		maxf=max(prev,*fpos);
		*fpos=min(*gpos,maxf);
		prev=*fpos;
	}	
	maxf=max(prev,*fpos);
	*fpos=min(*gpos,maxf);
	
	return 1;
}

template <class T>
int RecByErosion(std::vector<T> & g, std::vector<T> & f)
{
	if (g.size()!=f.size()) {
		std::cerr<<"RecByDilation: size(g)!=size(f)"<<std::endl;
		return 0;
	}
	
	if (f<g) {
		std::cerr<<"RecByErosion: f must be greater or equal to g"<<std::endl;
		return 0;
	}
	
	T minf;
	typename std::vector<T>::iterator fpos, gpos;
	std::vector<T> prev=g;
	
	
	while (f!=prev) {
		prev=f;
		fpos=f.begin();
		gpos=g.begin();
		T prev=*fpos;
        *fpos=std::max(*fpos,*gpos);
	
		fpos++; gpos++;
		
		for (; fpos!=f.end(); fpos++, gpos++) {
            minf=std::min(prev,*fpos);
			prev=*fpos;
            *fpos=std::max(*gpos,minf);
			
		}
	
		fpos=f.end(); fpos--;
		gpos=g.end(); gpos--;
	
		prev=*fpos;	
        *fpos=std::max(*fpos,*gpos);
	
		fpos--; gpos--;
		for ( ; fpos!=f.begin(); fpos--, gpos--) {
            minf=std::min(prev,*fpos);
			prev=*fpos;
            *fpos=std::max(*gpos,minf);
		}	
        minf=std::min(prev,*fpos);
        *fpos=std::max(*gpos,minf);
	}
	return 1;

}

template <class T>
int RMax(std::vector<T> &f, std::vector<T> &g)
{
	g.clear();
	typename std::vector<T>::iterator fpos, gpos, tpos;
	for (fpos=f.begin(); fpos!=f.end(); fpos++) 
		g.push_back((*fpos)-1);
		
	RecByDilation(f,g);
	for (fpos=f.begin(),gpos=g.begin(); fpos!=f.end(); fpos++,gpos++) 
		*gpos=*fpos-*gpos;
	
	return 1;
}

template <class T>
int HMax(std::vector<T> &f, std::vector<T> &g,T h)
{
	g.clear();
	typename std::vector<T>::iterator fpos, gpos, tpos;
	for (fpos=f.begin(); fpos!=f.end(); fpos++) 
		g.push_back((*fpos)-h);
		
	RecByDilation(f,g);
	
	return 1;
}

template <class T>
int EMax(std::vector<T> &f, std::vector<T> &g,T h, bool binary=false)
{
	std::vector<T> tmp;
	
	HMax(f,tmp,h);
	RMax(tmp,g);
	if (binary)
		for (int i=0; i<g.size(); i++)
			g[i]=(g[i]!=0);
	
	return 1;
}

template <class T>
int RMin(std::vector<T> &f, std::vector<T> &g)
{
	std::vector<T> tmp;
	
	typename std::vector<T>::iterator fpos, tpos;
	for (fpos=f.begin(); fpos!=f.end(); fpos++) 
		tmp.push_back((*fpos)+1);
	
	RecByErosion(f,tmp);
	

	g.clear();
	for (fpos=f.begin(),tpos=tmp.begin(); fpos!=f.end(); fpos++,tpos++) {
		//	cout<<"f: "<<*fpos<<", tmp: "<<*tpos<<endl;
		g.push_back(*tpos-*fpos);
	}
	
	return 1;
}

template <class T>
int HMin(std::vector<T> &f, std::vector<T> &g,T h)
{
	std::vector<T> tmp;
	tmp=f;
	g.clear();
	typename std::vector<T>::iterator pos;
	for (pos=f.begin(); pos!=f.end(); pos++) 
		g.push_back((*pos)+h);
		
	RecByErosion(f,g);
/*	
	for (pos=g.begin(); pos!=g.end(); pos++) 
		*pos-=h;
	*/
	return 1;
}

template <class T>
int EMin(std::vector<T> &f, std::vector<T> &g,T h, bool binary=false)
{
	std::vector<T> tmp;
	
	HMin(f,tmp,h);
	RMin(tmp,g);
	if (binary)
		for (size_t i=0; i<g.size(); i++)
			g[i]=(g[i]!=0);
	
	return 1;
}

template <class T, class Tm>
int MinImpose(std::vector<T> &f, std::vector<Tm> &fm, std::vector<T> &res)
{
	std::vector<T> tmp(f.size());
	
	res=f;
	
	T minVal=*min_element(f.begin(),f.end());
	T maxVal=*max_element(f.begin(),f.end());
	minVal=2*minVal-maxVal;
	
	
	
	typename std::vector<T>::iterator fIt=f.begin();
	typename std::vector<T>::iterator resIt=res.begin();
	typename std::vector<T>::iterator tmpIt=tmp.begin();
	typename std::vector<Tm>::iterator markIt=fm.begin();
		
	for ( ; fIt!=f.end() ; fIt++,resIt++, tmpIt++, markIt++) {
		
		*tmpIt=*markIt ? minVal : maxVal;
		*resIt=(*fIt)+1 < *tmpIt ? (*fIt)+1 : *tmpIt;
	}

	//RecByErosion(tmp,res);

	return 0;
}

}}

#endif

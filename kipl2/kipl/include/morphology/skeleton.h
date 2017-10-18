//<LICENCE>

#ifndef __SKELETON_H
#define __SKELETON_H

#include <deque>
#include <sstream>

#include "../base/timage.h"
#include "../logging/logger.h"
#include "morphology.h"


using namespace std;
using namespace kipl::base;
using namespace kipl::morphology;

namespace kipl { namespace morphology {
template <class ImgType>
int Skeleton(TImage<ImgType,2> &img, TImage<ImgType,2> &res, deque<int> &skel) 
{
    kipl::logging::Logger logger("morphology::skeleton");
    std::stringstream msg;
	size_t const * const dims=img.Dims();
    res=img;
	TImage<ImgType,2> tmp;

    int dirs[4]={-dims[0],1,dims[0],-1};
    ImgType lut[16]={1,1,1,0,1,1,0,0,1,0,1,0,0,0,0,1}; // Thinning
	// ImgType lut[16]={1,1,1,1,1,1,1,0,1,1,1,0,1,0,0,1}; // Skeleton requires pruning...

    int i,j,it;
    unsigned char n;
	skel.clear();
	ImgType *pImg=img.getDataptr();
	ImgType *pRes,*pTmp;

	for (i=0; i<img.N(); i++)
    {
		if (pImg[i]) {
			skel.push_back(i);
		}
    }

	int prev=0;
	int p;
	deque<int>::iterator pos,epos;
	it=0;

    while (skel.size()!=prev) 
	{
		tmp=res;
		pTmp=tmp.getDataptr();
		pRes=res.getDataptr();
		prev=skel.size();
        msg.str("");
        msg<<"Iteration "<<it++<<", skeleton size"<<skel.size();
		
        for (pos=skel.begin() ; pos !=skel.end() ; pos++) {
			p=*pos;
			
  		    if (pRes[p]) {
                n=(pRes[p+dirs[0]])!=0;
                for (j=1; j<4; j++) 
                    n=(n<<1) | (pRes[dirs[j]+p]!=0);

				pTmp[p]=lut[n];
			
                if (!(pTmp[p])) {
					epos=pos;
					if (pos!=skel.begin()) {
						pos--;
						skel.erase(epos);
					}
					else {
						skel.pop_front();
						pos=skel.begin();
					}
				}
				
            }
        }

		res=tmp;
		
		
    }
//	CDilate<ImgType,2> di(disk8_3x3,3,3);
//	CErode<ImgType,2> er(disk8_3x3,3,3);
//	tmp=di(res);
//	res=er(tmp);
	tmp=res+img;

	return 1;
}

/// \brief Thinning of a black and white image
///
/// Louisa Lam, Seong-Whan Lee, and Ching Y. Wuen, "Thinning Methodologies-A
/// Comprehensive Survey," IEEE TrPAMI, vol. 14, no. 9, pp. 869-885, 1992.  The
/// algorithm is described at the bottom of the first column and the top of the
/// second column on page 879.

/// \param img input image
/// \param res resulting skeleton
/// \param skel array with indices to the skeleton pixels, can be used to skeletonize substructures if non-empty upon call
template <class ImgType>
int thinning(TImage<ImgType,2> &img, TImage<ImgType,2> &res, deque<int>  & skel) 
{
	const unsigned char G1[512]={0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,0,1,1, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 0,0,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 0,0,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,0,0,0,0,0,0, 1,1,1,1,0,0,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,0,1,1,1,0,1,1, 1,1,0,0,1,1,0,0};

	const unsigned char G2[512]={0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,1,0,1, 0,0,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,0,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 0,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,0,0,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,0,1,0,1,0, 1,1,0,0,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,0,1,0, 1,1,1,1,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,0,1,0,1,0, 1,1,0,0,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,1,1,1,1,1,1,1, 1,1,1,1,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,0,0,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,0,1,0, 1,1,1,1,1,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,0,1,0,1,0, 1,1,0,0,1,0,0,0};

	const unsigned char G3[512]={0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};

	const unsigned char G4[512]={0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,1,1,1,0,0, 1,1,0,1,1,1,0,0,
								 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
								 1,1,0,0,1,1,0,0, 1,1,0,0,1,1,0,0};
	
	size_t const * const dims=img.Dims();

	int i,j,p,it,prev=-1;

	TImage<ImgType,2> tmp(dims);
    ImgType *pImg=img.GetDataPtr(), *pTmp=tmp.GetDataPtr();

    if (skel.empty()) {
		for (i=0; i<img.Size(); i++)
			if (pImg[i]) {
				skel.push_back(i);
		}
    }

	unsigned short n;
	int sx=dims[0];
	const int dirs[9]={-sx-1,-sx,-sx+1, -1 , 0, 1, sx-1,sx,sx+1};

	int tempres;	
	res=img;
    res.Clone();

    ImgType *pRes=res.GetDataPtr();

	deque<int>::iterator pos,epos;
	it=0;
	int postmp=0;

	deque<int>  skel_tmp;
	while (skel.size()!=prev) 
	{
		skel_tmp.clear();
		tmp=ImgType(0);
		pTmp=tmp.GetDataPtr();
		pRes=res.GetDataPtr();
	
		prev=skel.size();
			
        for (pos=skel.begin() ; pos !=skel.end() ; pos++) {
			p=*pos;
			postmp=p+dirs[0];
			n=postmp<0 ? 0 : (pRes[postmp])!=0;
			for (j=1; j<9; j++) {
				postmp=p+dirs[j];
				n=(n<<1);
				if ((0<=postmp) && (postmp<img.Size()))
				 	n|=(pRes[postmp]!=0);
			}
            pTmp[p]=static_cast<bool>(pRes[p]) & !(G1[n] & G2[n] & G3[n]);
		}
		size_t cnt=0;

        while (!skel.empty())
		{
			p=skel.front();
			skel.pop_front();

			postmp=p+dirs[0];
			n=postmp<0 ? 0 : (pTmp[postmp])!=0;
			for (j=1; j<9; j++) {
				postmp=p+dirs[j];				
				n=(n<<1);
				if (postmp>=0)
					n|=(pTmp[postmp]!=0);
            }
            tempres=static_cast<bool>(pTmp[p]) & !(G1[n] & G2[n] & G4[n]);
			if (pRes[p]==tempres) {
				skel_tmp.push_back(p);
			}
			pRes[p]=tempres;	
		}
		copy(skel_tmp.begin(),skel_tmp.end(),std::back_inserter(skel));
		//for (pos=skel.begin() ; pos !=skel.end() ; pos++,cnt++) {
		//	p=*pos;
		//	postmp=p+dirs[0];
		//	n=postmp<0 ? 0 : (pTmp[postmp])!=0;
		//	for (j=1; j<9; j++) {
		//		postmp=p+dirs[j];				
		//		n=(n<<1);
		//		if (postmp>=0)
		//			n|=(pTmp[postmp]!=0);
		//	}
		//	tempres=pTmp[p] & !(G1[n] & G2[n] & G4[n]);
		//	if (pRes[p]!=tempres) {
		//		epos=pos;
		//		if (pos!=skel.begin()) {
		//			pos--;
		//			skel.erase(epos);
		//		}
		//		else {
		//			skel.pop_front();
		//			pos=skel.begin();
		//		}
		//	}
		//	pRes[p]=tempres;
  //      }

    }

	return 1;

}

template <class ImgType>
int SkeletonAnalysis(TImage<ImgType,2> &img, deque<int> &skel, deque<int> &lines, deque<int> & crossings, deque<int>  & endpoints)
{
	lines.clear();
	crossings.clear();
	endpoints.clear();
	ImgType *pImg=img.GetDataPtr();
	size_t const * const dims=img.Dims();
	int sx=dims[0];
	const int dirs[8]={1,sx+1,sx,sx-1,-1,-sx-1,-sx,-sx+1};
	deque<int>::iterator pos;
	int p,i;
	unsigned char sum,q0,qN,qN1;
	
	for (pos=skel.begin(); pos!=skel.end(); pos++) {
		p=*pos;
		if ((p>sx) && (p<(img.Size()-sx))) {
			q0=pImg[p+dirs[0]]!=0;
			qN=q0;
			sum=0;
			for (i=1; i<8; i++) {
				qN1=pImg[p+dirs[i]]!=0;
				sum+=qN1 ^ qN;
				qN=qN1;
			}
			sum+=qN ^ q0;
	
			if (sum<3)
				endpoints.push_back(p);
			if (sum==4)
				lines.push_back(p);
			if (sum>4)
				crossings.push_back(p);
		}
	}
	return 1;
}
}}

#endif



#include <vector>
#include <deque>

#include "../../include/base/timage.h"
#include "../../include/morphology/label.h"

using namespace std;
namespace kipl { namespace morphology {
kipl::base::TImage<int,2> ExtractLabelledRegions(kipl::base::TImage<int,2> &img, vector<int> &lbl, bool keeplabel)
{
    kipl::base::TImage<int,2> tmp(img.dims());
	
	deque<int> stack;
	int *pImg=img.GetDataPtr();
	int *pTmp=tmp.GetDataPtr();
	
	// int sx=img.Size(0);
	
	// int NG[]={1,-1,-sx-1,-sx,-sx+1,sx-1,sx,sx+1};
	
	// size_t nLbl=lbl.size();
	
	size_t N=img.Size();
	
	// vector<int>::iterator lblIT;
	
	for (size_t i=0; i<N; i++) 
	{
		// for (lblIT = lbl.begin(); lblIT != lbl.end(); lblIT++) 
		for (const auto &lblitem : lbl) 
		{
			if (pImg[i]==lblitem) 
			{
				if (keeplabel)
					pTmp[i]=pImg[i];
				else
					pTmp[i]=1;
				break;
			}		
		}
	}
	return tmp;	
}
	
	
	kipl::base::TImage<int,2> ExtractLabelledRegions(kipl::base::TImage<int,2> &img, int n)
	{
		vector<int> lbl;
		
		lbl.push_back(n);
		
		return ExtractLabelledRegions(img,lbl,false);
	}
	
    LabelledItem::LabelledItem(int id, int nDim) {
			ResetAll();
			this->item_id=id;
			this->NDim=nDim;
	}
	
    int LabelledItem::AddPosition(int x, int y, int z)
	{
		this->sum[0]+=x;
		this->sum[1]+=y;
		this->sum[2]+=z;
		this->newinfo=true;
		this->N++;

		CheckBBox(x,y,z);

		return 0;
	}
	
	
    int LabelledItem::AddPosition(int *pos)
	{
		for (int i=0; i<NDim; i++)
			this->sum[i]+=pos[i];

		this->newinfo=true;
		this->N++;
		
		CheckBBox(pos);

		return 0;
	}
	
    int LabelledItem::CheckBBox(int x,int y,int z)
	{
        //cout<<"."<<flush;
		if (x<this->bbox[0]) this->bbox[0]=x;
		if (x>this->bbox[3]) this->bbox[3]=x;
		if (y<this->bbox[1]) this->bbox[1]=y;
		if (y>this->bbox[4]) this->bbox[4]=y;
		if (z<this->bbox[2]) this->bbox[2]=z;
		if (z>this->bbox[5]) this->bbox[5]=z;

		return 0;
	}

    int LabelledItem::getCOG(double &x, double &y, double &z) const
	{
		if (this->newinfo)
			this->Compute();
		
		x=cog[0];
		y=cog[1];
		z=cog[2];
		return 0;
	}
	
    int LabelledItem::getCOG(double *cogtmp) const
	{
		if (this->newinfo)
			Compute();
		
		memcpy(cogtmp,cog,3*sizeof(double));
		
		return 0;
	}
	
    int LabelledItem::ResetAll()
	{
		int i;
		for (i=0; i<3 ; i++) {
			this->sum[i]=0.0;
			this->sum2[i]=0.0;
			this->cog[i]=0.0;
			this->bbox[i]=10000;
			this->bbox[i+3]=0;
		}

		
		this->N=0;
		this->item_id=0;
		this->newinfo=true;
		this->PrettyPrint=false;
		this->NDim=0;
		return 0;
	}
	
	
    int LabelledItem::Copy(const LabelledItem & item)
	{
		this->NDim=item.NDim;
		int i;
		for (i=0; i<NDim ; i++) {
			this->sum[i]=item.sum[i];
			this->sum2[i]=item.sum2[i];
			this->cog[i]=item.cog[i];
		}

		for (i=0; i<6; i++) {
			this->bbox[i]=item.bbox[i];
		}

		this->N=item.N;
		this->item_id=item.item_id;
		this->newinfo=item.newinfo;
		
		return this->item_id;
	}
	
    int LabelledItem::Compute() const
	{
		for (int i=0; i<NDim ; i++) 
			this->cog[i]=this->sum[i]/N;
	
		newinfo=false;
		
		return N;
	}
	
    ostream & operator<<(ostream & os, const LabelledItem & item)
	{
		if (item.PrettyPrint) {
			os<<"Item "<<item.getItemID()<<":";
			os<<"N="<<item.getVolume()<<", ";
			os<<"cog=[";
			
			double cog[3];
			item.getCOG(cog);
			int i;
			for ( i=0; i< item.getNDim()-1; i++) 
				os<<cog[i]<<", ";
			os<<cog[i]<<"]";
		}
		else {
			os<<item.getItemID()<<" ";
			os<<item.getVolume()<<" ";
			
			double cog[3];
			item.getCOG(cog);
			int i;
			for ( i=0; i< item.getNDim(); i++) 
				os<<cog[i]<<" ";
		}
		
		
		return os;
	}
}} // End namespace morphology

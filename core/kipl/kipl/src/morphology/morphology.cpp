//<LICENCE>

#include <string>
#include <limits>

#include "../../include/morphology/morphology.h"
#include "../../include/base/timage.h"
#include "../../include/base/KiplException.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace kipl { namespace morphology {


CNeighborhood::CNeighborhood(const std::vector<size_t> &Dims, size_t const NDim, MorphConnect const conn)
{
	if (	(NDim==2) && 
			(conn!=kipl::morphology::conn4) && 
			(conn!=kipl::morphology::conn8))
		throw kipl::base::KiplException("Dimension does not match connectivity", __FILE__, __LINE__);
	
	if (	(NDim==3) && 
			(conn!=kipl::morphology::conn6) &&
			(conn!=kipl::morphology::conn18) &&
			(conn!=kipl::morphology::conn26))
			throw kipl::base::KiplException("Dimension does not match connectivity", __FILE__, __LINE__);
	
    dims = Dims;
	ndim=NDim;
	sx=dims[0];
	nimg=dims[0];

	for (size_t i=1; i<NDim; i++) {
		nimg*=dims[i];
	}
	
	ptrdiff_t NGm2[]={0,-1, -sx, -sx-1,-sx+1};
	bool NGpm_a2[]={true, false,true, false,true};
	bool NGpm_b2[]={true,true,true,true,false};
	
	ptrdiff_t NGp2[]={0,1,sx,sx+1,sx-1};
	ptrdiff_t NG2[8]={    1,   -1,  -sx,   sx,-sx-1,-sx+1, sx-1, sx+1};
	bool NG_l[]={ true,false, true, true,false, true,false, true};
	bool NG_r[]={false, true, true, true, true,false, true,false};
	
	isEdge=true;
	currentPos=-numeric_limits<ptrdiff_t >::max();
	if (NDim==1) {
		memcpy(NG,NG2,2*sizeof(ptrdiff_t));
		memcpy(NG_left,NG_l,2*sizeof(bool));
		memcpy(NG_right,NG_r,2*sizeof(bool));
		memcpy(NGp,NGp2,2*sizeof(ptrdiff_t));
		memcpy(NGm,NGm2,2*sizeof(ptrdiff_t));
		memcpy(NGpm_a,NGpm_a2,2*sizeof(bool));
		memcpy(NGpm_b,NGpm_b2,2*sizeof(bool));
		cNG=2;
		return ;
	}	

	sxy=dims[1]*dims[0];


	// Select connectivity
	if (NDim==2) {
		memcpy(NG,NG2,8*sizeof(ptrdiff_t ));
		memcpy(NG_left,NG_l,8*sizeof(bool));
		memcpy(NG_right,NG_r,8*sizeof(bool));
		memcpy(NGp,NGp2,5*sizeof(ptrdiff_t));
		memcpy(NGm,NGm2,5*sizeof(ptrdiff_t));
		memcpy(NGpm_a,NGpm_a2,5*sizeof(bool));
		memcpy(NGpm_b,NGpm_b2,5*sizeof(bool));
		first_line=dims[0]-1;
		last_line=nimg-dims[0];
	}
	else {
		// 3D neigborhoods
		ptrdiff_t NGm3[]={0, -1,-sx,-sxy,-sx-1,-sx+1,-sxy-sx-1,-sxy-sx,-sxy-sx+1,-sxy-1,-sxy+1,-sxy+sx-1,-sxy+sx,-sxy+sx+1};
		bool NGpm3_b2[]={true,true,true,true, true,false,true,true,false,true,false,true,true,false};
		bool NGpm3_a2[]={true,false,true,true,false, true,false,true,true,false,true,false,true,true};
		
		ptrdiff_t NGp3[]={0,1,sx,sxy,sx+1,sx-1,sxy-sx+1,sxy-sx,sxy-sx-1,sxy+1,sxy-1,sxy+sx+1,sxy+sx,sxy+sx-1};
		bool NGpm3_d2[]={true,true,true,true,true,true,true,true,true,true,true,true,true,true};
		bool NGpm3_c2[]={true,true,true,false,true,true,false,false,false,false,false,false,false,false};
		
		// New 6-, 18-, and 26- connectivity configuration
		ptrdiff_t NG3[27]=  {        1,       -1,    -sx  ,     sx  ,-sxy     ,sxy     , // 6-connectivity
						-sx    -1,    -sx+1,     sx-1,     sx+1,-sxy-sx  ,sxy+sx  ,
						-sxy   -1,-sxy   +1,-sxy+sx  , sxy-sx  , sxy   -1,sxy   +1, // 18-connectivity
						 sxy+sx-1, sxy-sx+1, sxy-sx-1,-sxy+sx+1,
						-sxy+sx-1,-sxy-sx+1, sxy+sx+1,-sxy-sx-1};// 26-connectivity
					
		
		bool NG3_l[27]={ true,false,true,true,true,true,
						 false,true,false,true,true,true,
						 false,true,true,true,false,true,
						 false,true,false,true,
						 false,true,true,false};
				
		bool NG3_r[27]={ false,true,true,true,true,true,
						 true,false,true,false,true,true,
						 true,false,true,true,true,false,
						 true,false,true,false,
						 true,false,false,true};
				
		bool NG3_f[27]={ true,true,false,true,true,true,
						 false,false,true,true,false,true,
						 true,true,true,false,true,true,
						 true,false,false,true,
						 true,false,true,false};
				
		bool NG3_b[27]={ true,true,true,false,true,true,
						 true,true,false,false,true,false,
						 true,true,false,true,true,true,
						 false,true,true,false,
						 false,true,false,true};

		memcpy(NG,NG3,27*sizeof(ptrdiff_t));
		memcpy(NG_left,NG3_l, 27*sizeof(bool));
		memcpy(NG_right,NG3_r, 27*sizeof(bool));
		memcpy(NG_front,NG3_f, 27*sizeof(bool));
		memcpy(NG_back,NG3_b, 27*sizeof(bool));
		
		
		memcpy(NGp,NGp3,14*sizeof(ptrdiff_t));
		memcpy(NGm,NGm3,14*sizeof(ptrdiff_t));
		memcpy(NGpm_a,NGpm3_a2,14*sizeof(bool));
		memcpy(NGpm_b,NGpm3_b2,14*sizeof(bool));
		memcpy(NGpm_c,NGpm3_c2,14*sizeof(bool));
		memcpy(NGpm_d,NGpm3_d2,14*sizeof(bool));
		
		first_slice=sxy-1;
		last_slice=nimg-sxy;
		
		first_line=dims[0]-1;
		last_line=sxy-dims[0];
		
	}


	switch(conn) {
	case conn4: 
		cNGpm=3; 
		cNG=4;
		break;
	case conn8:
		cNGpm=5;
		cNG=8;
		break;
	case conn6:
		cNGpm=4;
		cNG=6;
		break;
	case conn18:
		cerr<<"CNeighborhood Warning: half neighborhoods are not modified"<<endl;
		cNGpm=14;
		cNG=18;
		break;

	case conn26:
		cNGpm=14;
		cNG=26;
		break;
	default:
		throw kipl::base::KiplException("Unknown connectivity", __FILE__, __LINE__);
	}
}

ptrdiff_t CNeighborhood::index3x3x3(size_t index)
{
	if (index==-1)
		return -1;

	if (index>=cNG) {
		cerr<<"CNeighborhood::index3x3x3 - Index exceeds connectivity"<<endl;
		return -1;
	}

	if (ndim!=3) {
		cerr<<"Please only use index3x3x3 with 3D data"<<endl;
		return -1;
	}

	ptrdiff_t idx3x3x3[26]={14,12,10,16, 4,22,
						9,11,15,17, 1,25,
						3, 5, 7,19,21,23,
					   24,20,18, 8, 
					    6, 2,26, 0}; 
	return idx3x3x3[index];
}

ptrdiff_t CNeighborhood::forward(size_t pos, size_t index)
{
	if (ndim==2)
		return forward2D(pos,index);
		
	if (ndim==3)
		return neighbor3D(pos,index,cNGpm,NGp,NGpm_b,NGpm_a,NGpm_d,NGpm_c);
		
	return -1;
}

ptrdiff_t CNeighborhood::backward(size_t pos, size_t index)
{
	if (ndim==2)
		return backward2D(pos,index);
		
	if (ndim==3)
		return neighbor3D(pos,index,cNGpm,NGm,NGpm_a,NGpm_b,NGpm_c,NGpm_d);
		
	return -1;
}

ptrdiff_t CNeighborhood::neighbor(size_t pos, size_t index)
{
	switch(ndim) {
		case 1:
			return neighbor1D(pos,index);
		case 2:
			return neighbor2D(pos,index);
		case 3:
			return neighbor3D(pos,index,cNG,NG,NG_left,NG_right,NG_front,NG_back);
		default:
			return -1;
		}

	return -1;
}


ptrdiff_t CNeighborhood::forward2D(size_t pos, size_t index)
{
	
	if ((pos<0) || (nimg<=pos))
		return -1;
		
	if (cNGpm<index)
		return -1;
		
	ptrdiff_t p=pos+NGp[index];
	
	if ((pos==currentPos) && (!isEdge))
		return p;
	
	if (pos!=currentPos) {
		mpos=pos%dims[0];	
		currentPos=pos;
	}
	
	if ((pos>first_line) && (pos<last_line)) {
		isEdge=true;
		
		if (mpos==0) {
			if (NGpm_b[index])
				return p;
			else
				return -1;
		}
		
		if (mpos==first_line) {
			if (NGpm_a[index])
				return p;
			else
				return -1;
		}
		
		isEdge=false;
		
		return p;
	}
	
	if (pos<=first_line) {
		isEdge=true;
		
		if (p>=0) {
			if (mpos==0) {
				if (NGpm_b[index])
					return p;
				else
					return -1;
			}
			
			if (mpos==first_line) {
				if (NGpm_a[index])
					return p;
				else
					return -1;
			}
			return p;
		}
		else
			return -1;
	}
			
	if (pos>=last_line) {
		isEdge=true;
		if (p<nimg) {
			if (mpos==0) {
				if (NGpm_b[index])
					return p;
				else
					return -1;
			}
			
			if (mpos==first_line) {
				if (NGpm_a[index])
					return p;
				else
					return -1;
			}
			return p;
		}
		else
			return -1;
	}
	
	isEdge=false;
	
	return p;	
	
}


ptrdiff_t CNeighborhood::backward2D(size_t pos, size_t index)
{
    if (pos>=nimg)
		return -1;
		
	if (index>cNGpm)
		return -1;
	
	ptrdiff_t p=pos+NGm[index];
	
	if ((pos==currentPos) && (!isEdge))
		return p;
	
	if (pos!=currentPos) {
		mpos=pos%dims[0];	
		currentPos=pos;
	}
		
	if ((pos>first_line) && (pos<last_line)) {
		isEdge=true;
			
		if (mpos==0) {
			if (NGpm_a[index])
				return p;
			else
				return -1;
		}
		
		if (mpos==first_line) {
			if (NGpm_b[index])
				return p;
			else
				return -1;
		}
		
		isEdge=false;
		
		return p;
	}
	
	if (pos<=first_line) {
		isEdge=true;
		if (p>=0) {
			if (mpos==0) {
				if (NGpm_a[index])
					return p;
				else
					return -1;
			}
			
			if (mpos==first_line) {
				if (NGpm_b[index])
					return p;
				else
					return -1;
			}
			return p;
		}
		else
			return -1;
	}
			
	if (pos>=last_line) {
		isEdge=true;
		if (p<nimg) {
			if (mpos==0) {
				if (NGpm_a[index])
					return p;
				else
					return -1;
			}
			
			if (mpos==first_line) {
				if (NGpm_b[index])
					return p;
				else
					return -1;
			}
			return p;
		}
		else
			return -1;
	}
	
	isEdge=false;
	
	return p;
}

ptrdiff_t CNeighborhood::neighbor1D(size_t pos, size_t index)
{
	ptrdiff_t p=pos+NG[index & 1];

	if ((p<0) || (p>=sx)) 
		return -1;	
	
	return p;	
}


ptrdiff_t CNeighborhood::neighbor2D(size_t pos, size_t index)
{
	if ((pos==currentPos) && !isEdge) 
		return pos+NG[index];	

	if ((pos<0) || (pos>=nimg))
		return -1;
	if (index>cNG)
		return -1;

	ptrdiff_t p=pos+NG[index];	
	if (pos!=currentPos) {
		mpos=pos%dims[0];	
		currentPos=pos;
		if ((!mpos) || (mpos==first_line))
			isEdge=true;
		else
			isEdge=false;
	}
	if ((pos>first_line) && (pos<last_line)) {
		isEdge=true;
		if (mpos==0) {
			if (NG_left[index])
				return p;
			else
				return -1;
		}
		
		if (mpos==first_line) {
			if (NG_right[index])
				return p;
			else
				return -1;
		}
		
		return p;
	}
	if (pos<=first_line) {
		isEdge=true;
		if (p>=0) {
			if (mpos==0) {
				if (NG_left[index])
					return p;
				else
					return -1;
			}
		
			if (mpos==first_line) {
				if (NG_right[index])
					return p;
				else
					return -1;
			}
				
			return p;
		}
		else
			return -1;
	}
	if (pos>=last_line) {
		isEdge=true;
		if (p<nimg) {
			if (mpos==0) {
				if (NG_left[index])
					return p;
				else
					return -1;
			}
		
			if (mpos==first_line) {
				if (NG_right[index])
					return p;
				else
					return -1;
			}
				
			return p;
		}
		else
			return -1;
	}
	
	isEdge=false;
	
	return p;	
}

ptrdiff_t CNeighborhood::neighbor3D(size_t pos, size_t index,int cnt,ptrdiff_t *ng, bool *left, bool *right, bool *front, bool *back)
{
	if ((index>=cnt) && (index<0))
		return -1;

	ptrdiff_t p=pos+ng[index];
	
	if ((pos==currentPos) && (!isEdge)) 
		return p;
	

	if ((p<0) || (p>=nimg)) 
		return -1;
	
	if (pos!=currentPos) {
		currentPos=pos;
		smpos=pos%dims[0];	
		vmpos=pos%sxy;
	}
	
	isEdge=true;
	
	if ((vmpos>first_line) && (vmpos<last_line)) {
		if (smpos==0) {
			if (left[index]) 
				return p;
			else
				return -1;
		}
		
		if (smpos==first_line) {
			if (right[index])
				return p;
			else
				return -1;
		}

		if ((pos>=sxy) && (pos<last_slice))
			isEdge=false;
		
		return p;
	}
	
	if (vmpos<=first_line) {
		if ((front[index])) {
			if (smpos==0) {
				if (left[index])
					return p;
				else
					return -1;
			}
		
			if (smpos==first_line) {
				if (right[index])
					return p;
				else
					return -1;
			}
				
			return p;
		}
		else
			return -1;
	}
			
	if (vmpos>=last_line) {
		if ((back[index])) {
			if (smpos==0) {
				if (left[index])
					return p;
				else
					return -1;
			}
		
			if (smpos==first_line) {
				if (right[index])
					return p;
				else
					return -1;
			}
				
			return p;
		}
		else
			return -1;
	}

	isEdge=false;
	
	return p;
}


CStructureElement::CStructureElement(void)
{
}

CStructureElement::~CStructureElement(void)
{
}

}}

std::string enum2string(kipl::morphology::MorphConnect mc)
{
    switch (mc) {
        case kipl::morphology::conn4  : return "conn4";  break;
        case kipl::morphology::conn8  : return "conn8";  break;
        case kipl::morphology::conn6  : return "conn6";  break;
        case kipl::morphology::conn18 : return "conn18";  break;
        case kipl::morphology::conn26 : return "conn26";  break;
        case kipl::morphology::euclid : return "euclid";  break;
        default : throw kipl::base::KiplException("Failed to convert Morphconnect to string", __FILE__, __LINE__);
    }
    return "bad value";
}

std::ostream & operator<<(std::ostream &s, kipl::morphology::MorphConnect mc)
{
    s<<enum2string(mc);

    return s;
}

void string2enum(std::string str, kipl::morphology::MorphConnect &mc)
{
    if (str=="conn4") mc=kipl::morphology::conn4;
    else if (str=="conn8") mc=kipl::morphology::conn8;
    else if (str=="conn6") mc=kipl::morphology::conn6;
    else if (str=="conn18") mc=kipl::morphology::conn18;
    else if (str=="conn26") mc=kipl::morphology::conn26;
    else if (str=="euclid") mc=kipl::morphology::euclid;
    else throw kipl::base::KiplException("Failed to convert string to Morphconnect", __FILE__, __LINE__);

}


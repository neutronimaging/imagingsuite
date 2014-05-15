#include "../../include/filters/filterbase.h"

namespace kipl { namespace filters {

FilterBase::~FilterBase() {		
	if (nKernelIndex != NULL)
		delete [] nKernelIndex;
}

size_t FilterBase::PrepareIndex(size_t const * const imgDims, 
								size_t const * const nKernelDims,  	
								size_t const nDims)
{
	size_t len=imgDims[0];
	size_t klen=nKernelDims[0]-1;
	for (size_t i=1; i<nDims; i++) {
		klen+=(nKernelDims[i]-1)*len;
		len*=imgDims[i];
	}
	len-=klen;

	memset(this->nKernelIndex,0, sizeof(int)*nKernel);
	int kProd=1;
	int imgProd=1;
	for (size_t dim=0; dim<nDims; dim++) {
		kProd*=nKernelDims[dim];
		const int cnCenter=static_cast<int>(nKernelDims[dim]>>1);
		for (int i=0; i<static_cast<int>(nKernel); i++) {
			nKernelIndex[i] += ((i % kProd)*nKernelDims[dim]/kProd - cnCenter)*imgProd;
		}
		imgProd*=imgDims[dim];
	}

	return len;
}


std::ostream & operator<<(std::ostream &str, FilterBase::EdgeProcessingStyle eps)
{
	switch(eps) {
		case FilterBase::EdgeZero   : str<<"EdgeZero";   break;
		case FilterBase::EdgeSame   : str<<"EdgeSame";   break;
		case FilterBase::EdgeMirror : str<<"EdgeMirror"; break;
		case FilterBase::EdgeValid  : str<<"EdgeValid";  break;
	}
	return str;
}
}}

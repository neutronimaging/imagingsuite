

#include "../../include/filters/filterbase.h"

namespace kipl { namespace filters {

FilterBase::~FilterBase() {		
}

size_t FilterBase::PrepareIndex(const std::vector<size_t> &imgDims,
                                const std::vector<size_t> &nKernelDims)
{

	size_t len=imgDims[0];
	size_t klen=nKernelDims[0]-1;
    for (size_t i=1; i<nKernelDims.size(); i++)
    {
		klen+=(nKernelDims[i]-1)*len;
		len*=imgDims[i];
	}
	len-=klen;

    nKernelIndex = std::vector<ptrdiff_t>(nKernel,0);

    size_t kProd=1;
    size_t imgProd=1;
    for (size_t dim=0; dim<nKernelDims.size(); dim++)
    {
		kProd*=nKernelDims[dim];
        const int cnCenter=static_cast<int>(nKernelDims[dim]/2);
        for (int i=0; i<static_cast<int>(nKernel); i++)
        {
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

std::ostream KIPLSHARED_EXPORT & operator<<(std::ostream &str, kipl::filters::KernelType kt)
{
    switch (kt) {
        case kipl::filters::KernelType::Full : str<<"Full"; break;
        case kipl::filters::KernelType::Separable : str<<"Separable"; break;
    }

    return str;
}

std::ostream & operator<<(std::ostream &str, kipl::filters::FilterBase::EdgeProcessingStyle eps)
{
    switch(eps) {
        case kipl::filters::FilterBase::EdgeZero   : str<<"EdgeZero";   break;
        case kipl::filters::FilterBase::EdgeSame   : str<<"EdgeSame";   break;
        case kipl::filters::FilterBase::EdgeMirror : str<<"EdgeMirror"; break;
        case kipl::filters::FilterBase::EdgeValid  : str<<"EdgeValid";  break;
    }
    return str;
}

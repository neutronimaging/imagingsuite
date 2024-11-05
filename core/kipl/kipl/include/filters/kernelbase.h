

#ifndef __KERNELBASE_H_
#define __KERNELBASE_H_

namespace kipl { namespace filters {

/// \brief A class to define and handle convolution kernels.
///
/// It was implemented with the intention to improve the user experience, but i was not able to complete this concept.
template <typename T, size_t nDims>
class KernelBase {
public:
    KernelBase() : m_Kernel(nullptr), m_nKernelLength(0)
	{
		memset(m_nDims,0,sizeof(size_t)*nDims);
		memset(m_nCenters,0,sizeof(size_t)*nDims);
	}

	~KernelBase() {RemoveAllocation();}
	size_t const * const Dims() { return m_nDims; } 
	size_t const * const Centers() { return m_nCenters;}
	T const * const Kernel() { return m_Kernel; }
	size_t Length() { return m_nKernelLength; }
protected:
	size_t m_nDims[nDims];
	size_t m_nCenters[nDims];
	size_t m_nKernelLength;
	T *m_Kernel;
	
	void CreateAllocation(size_t const * const dims, size_t const * const centers )
	{
		m_nKernelLength=dims[0];
		for (size_t i=1; i<nDims; i++)
			m_nKernelLength*=dims[i];
		memcpy(m_nDims,dims,sizeof(size_t)*nDims);
		memcpy(m_nCenters, centers, sizeof(size_t)*nDims);

		m_Kernel=reinterpret_cast<T*>(_aligned_malloc(m_nKernelLength*sizeof(T),16));
	}

	void RemoveAllocation()
	{
        if (m_Kernel!=nullptr)
			_aligned_free(m_Kernel);
	}
};
}}
#endif

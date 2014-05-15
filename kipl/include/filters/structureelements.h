#ifndef __STRUCTUREELEMENTS_H_
#define __STRUCTUREELEMENTS_H_

#include "kernelbase.h"
template <typename T, size_t nDims>
class StructureElementBase : public KernelBase<T,nDims> 
{
private:
	StructureElementBase() {}
};

template <typename T, size_t nDims>
class BoxSE : public StructureElementBase<T,nDims>
{
public: 
	/// \brief Symetric box 
	/// \param size Side length in all dimensions
	BoxSE(size_t const size);

	/// \brief Symetric box 
	/// \param size Side length in each dimension
	BoxSE(size_t const * const size);
	BoxSE(size_t const * const size, size_t const * const centers);
};

#endif
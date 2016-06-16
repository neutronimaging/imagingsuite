#ifndef __STRUCTUREELEMENTS_H_
#define __STRUCTUREELEMENTS_H_

#include "kernelbase.h"

namespace kipl { namespace morphology {


/// \brief Base class for morphological structure elements
template <typename T, size_t nDims>
class StructureElementBase : public KernelBase<T,nDims> 
{
private:
	StructureElementBase() {}
};

/// \brief The class implements a box structure elements
template <typename T, size_t nDims>
class BoxSE : public StructureElementBase<T,nDims>
{
public: 
    /// \brief Symmetric box
	/// \param size Side length in all dimensions
	BoxSE(size_t const size);

    /// \brief Symmetric box
	/// \param size Side length in each dimension
	BoxSE(size_t const * const size);

    /// \brief Symmetric box
    /// \param size Side length in each dimension
    /// \param centers Defines the position of the centers in the structure element
    BoxSE(size_t const * const size, size_t const * const centers);
};

}

#endif

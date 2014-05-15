//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//

#ifndef ALIGNED_MALLOC_H_
#define ALIGNED_MALLOC_H_
#include <cstdlib>
/// \brief Memory alocation to a specified byte boundary
/// \param size Length of the buffer to be allocated
/// \param align_size Alignement length for the buffer. align_size has to be a power of two !!
/// \returns A pointer to the allocated buffer
void *aligned_malloc(std::size_t size, std::size_t align_size);
/// \brief Dealocates the aligned memory buffer
/// \param ptr Pointer to the buffer to be dealocated
void aligned_free(void *ptr);

bool is_aligned(void *ptr, std::size_t align_size)
{
	return (reinterpret_cast<size_t>(ptr) & (align_size-1))!=0;
}

#endif /*ALIGNED_MALLOC_H_*/

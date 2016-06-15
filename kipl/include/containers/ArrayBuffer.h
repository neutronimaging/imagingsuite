#ifndef __ARRAYBUFFER_H
#define __ARRAYBUFFER_H

#include "../base/core/sharedbuffer.h"

namespace kipl { namespace containers {
/// \brief A fixed sized array buffer with push-back insertion
/// \note can this be replaced by an STL container?
template <class T>
class ArrayBuffer
{
public:
	ArrayBuffer(size_t N) : data(N), cnt(0), mSize(N) {};

	/// \brief Push an item behind the last inserted
	/// \param val The item to put in the buffer
	/// \returns the number of elements stored in the buffer
	size_t push_back(T val) { data.GetDataPtr()[cnt++]=val; return cnt;}

	/// \brief The number of items currently stored in the buffer
	/// \returns the number of items.
	size_t size() {return cnt;}
	
	/// \brief Sets the buffer insertion point to zero. Neither data nor memory allocation will be cleaned.
	void reset() { cnt=0; }

	void resize(size_t N) {data=kipl::base::core::buffer<T>(N); mSize=N; cnt=0;}

	/// \returns A pointer to the data array.
	T* dataptr() {return data.GetDataPtr();}

	/// \brief Copies the data of the input buffer to the current buffer.
	/// \param b the array to copy
	void copy(ArrayBuffer<T> *b) {
		memcpy(data.GetDataPtr(),b->dataptr(),b->size()*sizeof(T));
		cnt=b->size();
	}

	/// \returns if the buffer is empty.
 /// \retval true if the buffer is empty
 /// \retval false otherwise
	bool empty() {return cnt==0;}
protected:
	kipl::base::core::buffer<T> data;
	size_t cnt;
	size_t mSize;
};

}}
#endif

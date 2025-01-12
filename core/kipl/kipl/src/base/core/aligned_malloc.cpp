//<LICENCE>

#include <cstdlib>

using namespace std;

// align_size has to be a power of two !! 
void *aligned_malloc(size_t size, size_t align_size) 
{

  char *ptr,*ptr2,*aligned_ptr;
  int align_mask = align_size - 1;

  ptr=static_cast<char *>(malloc(size + align_size + sizeof(int)));
  if (ptr==nullptr) return(nullptr);

  ptr2 = ptr + sizeof(int);
  aligned_ptr = ptr2 + (align_size - (reinterpret_cast<size_t>(ptr2) & align_mask));


  ptr2 = aligned_ptr - sizeof(int);
  *(reinterpret_cast<int *>(ptr2))=static_cast<int>(aligned_ptr - ptr);

  return(aligned_ptr);
}

void aligned_free(void *ptr) 
{
  int *iptr=static_cast<int *>(ptr);
  int *ptr2=iptr - 1;
  iptr -= *ptr2;
  free(iptr);
}

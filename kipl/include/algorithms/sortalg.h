//<LICENCE>

#ifndef __sortalg_h
#define __sortalg_h

/// \brief Sort an array using the heap sort algorithm the operation is inplace
/// \param ra The array to sort
/// \param n number of elements
template <class T>
void heapsort(T * ra, int n)
{
	int i,ir,j,l;
	T rra;
	
	if (n < 2)  return;

	ra--;

	l=(n >> 1)+1;
	ir=n;
	for (;;) {
		if (l > 1) {
			rra=*(ra + --l);
		} else {
			rra=*(ra+ir);
			*(ra+ir)=*(ra+1);
			if (--ir == 1) {
				*(ra+1)=rra;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if (j < ir && *(ra+j) < *(ra+j+1)) j++;
			if (rra < *(ra+j)) {
				*(ra+i)=*(ra+j);
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		*(ra+i)=rra;
	}
	ra++;
}

/*
template <class T>
void heapsort(T * ra, int n)
{
	int i,ir,j,l;
	T rra;
	
	if (n < 2)  return;

	ra--;

	l=(n >> 1)+1;
	ir=n;
	for (;;) {
		if (l > 1) {
			rra=ra[--l];
		} else {
			rra=ra[ir];
			ra[ir]=ra[1];
			if (--ir == 1) {
				ra[1]=rra;
				break;
			}
		}
		i=l;
		j=l+l;
		while (j <= ir) {
			if (j < ir && ra[j] < ra[j+1]) j++;
			if (rra < ra[j]) {
				ra[i]=ra[j];
				i=j;
				j <<= 1;
			} else j=ir+1;
		}
		ra[i]=rra;
	}
	ra++;
}
*/
#endif

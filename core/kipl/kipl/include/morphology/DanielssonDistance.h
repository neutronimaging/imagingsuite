//<LICENCE>

#ifndef DANIELSSONDISTANCE_H_
#define DANIELSSONDISTANCE_H_

#include "../base/timage.h"

namespace kipl { namespace morphology {

template <size_t N>
class DistanceTripple {
public:
	DistanceTripple();
	DistanceTripple(const DistanceTripple<N> &d);
	DistanceTripple(short _x, short _y, short _z);

	DistanceTripple<N> & operator=(const DistanceTripple<N> &d);
	DistanceTripple<N> & operator=(short v);

	DistanceTripple<N> operator+(const DistanceTripple<N> &d);

	float Distance();

	float Distance2();

	float Distance2(short _x,short _y, short _z);

	float Distance2(DistanceTripple<N> &t);

	short x;
	short y;
	short z;
};

template <typename T, size_t N>
class DanielssonDistance {
public:
	DanielssonDistance();
	virtual ~DanielssonDistance();

	kipl::base::TImage<float,N> operator()(kipl::base::TImage<T,N> &mask);

protected:
	void InitializeIndexing(size_t const * const dims);

	size_t m_nNeighborhoodSize;
	ptrdiff_t m_nIndex[15];
	DistanceTripple<N> m_Distances[15];
	DistanceTripple<N> test;

};
}}

#include "core/DanielssonDistance.hpp"

#endif /* DANIELSSONDISTANCE_H_ */

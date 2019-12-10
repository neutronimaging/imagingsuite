//<LICENCE>

#ifndef DANIELSSONDISTANCE_HPP_
#define DANIELSSONDISTANCE_HPP_

#include "../DanielssonDistance.h"


namespace kipl { namespace morphology {

template <size_t N>
DistanceTripple<N>::DistanceTripple() : x(0),y(0),z(0) {}

template <size_t N>
DistanceTripple<N>::DistanceTripple(const DistanceTripple<N> &d) : x(d.x),y(d.y),z(d.z) {}

template <size_t N>
DistanceTripple<N>::DistanceTripple(short _x, short _y, short _z) : x(_x),y(_y),z(_z) {}

template <size_t N>
DistanceTripple<N> & DistanceTripple<N>::operator=(const DistanceTripple<N> &d) {x=d.x; y=d.y; z=d.z; return *this;}

template <size_t N>
DistanceTripple<N> & DistanceTripple<N>::operator=(short v)
{
	x=v; y=v; z=v; return *this;
}

template <size_t N>
DistanceTripple<N> DistanceTripple<N>::operator+(const DistanceTripple<N> &d)
{
	return DistanceTripple<N>(x+d.x,y+d.y,z+d.z);
}

template <size_t N>
float DistanceTripple<N>::Distance()
{
	switch (N) {
	case 1 : return abs(x); break;
	case 2 : return sqrt(float(x)*float(x)+float(y)*float(y)); break;
	case 3 : return sqrt(float(x)*float(x)+float(y)*float(y)+float(z)*float(z)); break;
	}
	return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2()
{
	switch (N) {
	case 1 : return float(x)*float(x); break;
	case 2 : return float(x)*float(x)+float(y)*float(y); break;
	case 3 : return float(x)*float(x)+float(y)*float(y)+float(z)*float(z); break;
	}
	return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2(short _x,short _y, short _z)
{
	switch (N) {
	case 1 : return float(_x+x)*float(_x+x); break;
	case 2 : return float(_x+x)*float(_x+x)	+float(_y+y)*float(_y+y); break;
	case 3 : return float(_x+x)*float(_x+x)	+float(_y+y)*float(_y+y)+float(_z+z)*float(_z+z); break;
	}
	return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2(DistanceTripple<N> &t)
{
	switch (N) {
	case 1 : return float(t.x+x)*float(t.x+x); break;
	case 2 : return float(t.x+x)*float(t.x+x)+float(t.y+y)*float(t.y+y); break;
	case 3 : return float(t.x+x)*float(t.x+x)+float(t.y+y)*float(t.y+y)+float(t.z+z)*float(t.z+z); break;
	}
	return 0.0f;
}

template <typename T, size_t N>
DanielssonDistance<T,N>::DanielssonDistance()
{

	switch (N) {
		case 1: m_nNeighborhoodSize=2; break;
		case 2: m_nNeighborhoodSize=5; break;
		case 3: m_nNeighborhoodSize=14; break;
	}

	test=DistanceTripple<N>(0,0,0);

	m_Distances[0]=DistanceTripple<N>(0,0,0);
	m_Distances[1]=DistanceTripple<N>(1,0,0);

	m_Distances[2]=DistanceTripple<N>(1,1,0);
	m_Distances[3]=DistanceTripple<N>(0,1,0);
	m_Distances[4]=DistanceTripple<N>(1,1,0);

	m_Distances[5]=DistanceTripple<N>(1,1,1);
	m_Distances[6]=DistanceTripple<N>(0,1,1);
	m_Distances[7]=DistanceTripple<N>(1,1,1);
	m_Distances[8]=DistanceTripple<N>(1,0,1);
	m_Distances[9]=DistanceTripple<N>(0,0,1);
	m_Distances[10]=DistanceTripple<N>(1,0,1);
	m_Distances[11]=DistanceTripple<N>(1,1,1);
	m_Distances[12]=DistanceTripple<N>(0,1,1);
	m_Distances[13]=DistanceTripple<N>(1,1,1);

}

template <typename T, size_t N>
DanielssonDistance<T,N>::~DanielssonDistance()
{
}

template <typename T, size_t N>
kipl::base::TImage<float,N> DanielssonDistance<T,N>::operator()(kipl::base::TImage<T,N> &mask)
{
	kipl::base::TImage<float,N> distance(mask.Dims());
	kipl::base::TImage<DistanceTripple<N>,N> dist(mask.Dims());
	InitializeIndexing(mask.Dims());

	DistanceTripple<N> maxdist(std::numeric_limits<short>::max(),std::numeric_limits<short>::max(), std::numeric_limits<short>::max());
	DistanceTripple<N> zerodist(0,0,0);
	DistanceTripple<N> firstdist(1,0,0);

	for (size_t i=0; i<dist.Size(); i++) {

		dist[i] = mask[i] ? maxdist : zerodist;
	}

	int end[3]={mask.Size(0)-1,mask.Size(1)-1,mask.Size(2)-1};

	T * pMask=NULL;
	for (int z=1; z<end[2]; z++) {
		for (int y=1; y<end[1]; y++) {
			pMask=mask.GetLinePtr(y,z);
			DistanceTripple<N> * pDist= dist.GetLinePtr(y,z);

			for (int x=1; x<end[0]; x++) {
				if (pMask[x]) {

					float mindist=std::numeric_limits<float>::max();
					int index=0;
					for (size_t i=1; i<m_nNeighborhoodSize; i++) {

						int pos=x-m_nIndex[i];
						float val=pDist[pos].Distance2();
						if ((pMask[pos]) && (val<mindist)) {
							mindist=val;
							index=i;
						}
					}
					if (index!=0)
						pDist[x]=pDist[x-m_nIndex[index]]+m_Distances[index];
					else
						pDist[x]=firstdist;
				}
			}
		}
	}

//	for (int z=end[2]-1; 1<=z; z--) {
//		for (int y=end[1]-1; 1<=y; y--) {
//			pMask=mask.GetLinePtr(y,z);
//			DistanceTripple<N> * pDist= dist.GetLinePtr(y,z);
//
//			for (int x=end[0]-1; 1<=x; x--) {
//				if (pMask[x]) {
//					float mindist=pDist[x].Distance2();
//
//					int index=0;
//					for (size_t i=1; i<m_nNeighborhoodSize; i++) {
//						int pos=x+m_nIndex[i];
//						float val=pDist[pos].Distance2(m_Distances[i]);
//						if (val<mindist) {
//							mindist=val;
//							index=i;
//						}
//					}
//					if (index!=0) {
//						pDist[x]=pDist[x+m_nIndex[index]]+m_Distances[index];
//					}
//				}
//			}
//		}
//	}


	for (size_t i=0; i<distance.Size(); i++) {
		distance[i]=dist[i].Distance();
	}

	return distance;
}

template <typename T, size_t N>
void DanielssonDistance<T,N>::InitializeIndexing(size_t const * const dims)
{
	ptrdiff_t sx=dims[0];
	ptrdiff_t sxy=dims[0]*dims[1];

	m_nIndex[0]=0;
	m_nIndex[1]=1;

	m_nIndex[2]=sx-1;
	m_nIndex[3]=sx;
	m_nIndex[4]=sx+1;

	m_nIndex[5]=sxy-sx-1;
	m_nIndex[6]=sxy-sx;
	m_nIndex[7]=sxy-sx+1;
	m_nIndex[8]=sxy-1;
	m_nIndex[9]=sxy;
	m_nIndex[10]=sxy+1;
	m_nIndex[11]=sxy+sx-1;
	m_nIndex[12]=sxy+sx;
	m_nIndex[13]=sxy+sx+1;

}

}}

namespace kipl { namespace morphology { namespace old {

template <size_t N>
DistanceTripple<N>::DistanceTripple() : x(0),y(0),z(0) {}

template <size_t N>
DistanceTripple<N>::DistanceTripple(const DistanceTripple<N> &d) : x(d.x),y(d.y),z(d.z) {}

template <size_t N>
DistanceTripple<N>::DistanceTripple(short _x, short _y, short _z) : x(_x),y(_y),z(_z) {}

template <size_t N>
DistanceTripple<N> & DistanceTripple<N>::operator=(const DistanceTripple<N> &d) {x=d.x; y=d.y; z=d.z; return *this;}

template <size_t N>
DistanceTripple<N> & DistanceTripple<N>::operator=(short v)
{
    x=v; y=v; z=v; return *this;
}

template <size_t N>
DistanceTripple<N> DistanceTripple<N>::operator+(const DistanceTripple<N> &d)
{
    return DistanceTripple<N>(x+d.x,y+d.y,z+d.z);
}

template <size_t N>
float DistanceTripple<N>::Distance()
{
    switch (N) {
    case 1 : return abs(x); break;
    case 2 : return sqrt(float(x)*float(x)+float(y)*float(y)); break;
    case 3 : return sqrt(float(x)*float(x)+float(y)*float(y)+float(z)*float(z)); break;
    }
    return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2()
{
    switch (N) {
    case 1 : return float(x)*float(x); break;
    case 2 : return float(x)*float(x)+float(y)*float(y); break;
    case 3 : return float(x)*float(x)+float(y)*float(y)+float(z)*float(z); break;
    }
    return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2(short _x,short _y, short _z)
{
    switch (N) {
    case 1 : return float(_x+x)*float(_x+x); break;
    case 2 : return float(_x+x)*float(_x+x)	+float(_y+y)*float(_y+y); break;
    case 3 : return float(_x+x)*float(_x+x)	+float(_y+y)*float(_y+y)+float(_z+z)*float(_z+z); break;
    }
    return 0.0f;
}

template <size_t N>
float DistanceTripple<N>::Distance2(DistanceTripple<N> &t)
{
    switch (N) {
    case 1 : return float(t.x+x)*float(t.x+x); break;
    case 2 : return float(t.x+x)*float(t.x+x)+float(t.y+y)*float(t.y+y); break;
    case 3 : return float(t.x+x)*float(t.x+x)+float(t.y+y)*float(t.y+y)+float(t.z+z)*float(t.z+z); break;
    }
    return 0.0f;
}

template <typename T, size_t N>
DanielssonDistance<T,N>::DanielssonDistance()
{

    switch (N) {
        case 1: m_nNeighborhoodSize=2; break;
        case 2: m_nNeighborhoodSize=5; break;
        case 3: m_nNeighborhoodSize=14; break;
    }

    test=DistanceTripple<N>(0,0,0);

    m_Distances[0]=DistanceTripple<N>(0,0,0);
    m_Distances[1]=DistanceTripple<N>(1,0,0);

    m_Distances[2]=DistanceTripple<N>(1,1,0);
    m_Distances[3]=DistanceTripple<N>(0,1,0);
    m_Distances[4]=DistanceTripple<N>(1,1,0);

    m_Distances[5]=DistanceTripple<N>(1,1,1);
    m_Distances[6]=DistanceTripple<N>(0,1,1);
    m_Distances[7]=DistanceTripple<N>(1,1,1);
    m_Distances[8]=DistanceTripple<N>(1,0,1);
    m_Distances[9]=DistanceTripple<N>(0,0,1);
    m_Distances[10]=DistanceTripple<N>(1,0,1);
    m_Distances[11]=DistanceTripple<N>(1,1,1);
    m_Distances[12]=DistanceTripple<N>(0,1,1);
    m_Distances[13]=DistanceTripple<N>(1,1,1);

}

template <typename T, size_t N>
DanielssonDistance<T,N>::~DanielssonDistance()
{
}

template <typename T, size_t N>
kipl::base::TImage<float,N> DanielssonDistance<T,N>::operator()(kipl::base::TImage<T,N> &mask)
{
    kipl::base::TImage<float,N> distance(mask.Dims());
    kipl::base::TImage<DistanceTripple<N>,N> dist(mask.Dims());
    InitializeIndexing(mask.Dims());

    DistanceTripple<N> maxdist(std::numeric_limits<short>::max(),std::numeric_limits<short>::max(), std::numeric_limits<short>::max());
    DistanceTripple<N> zerodist(0,0,0);
    DistanceTripple<N> firstdist(1,0,0);

    for (size_t i=0; i<dist.Size(); i++) {

        dist[i] = mask[i] ? maxdist : zerodist;
    }

    int end[3]={mask.Size(0)-1,mask.Size(1)-1,mask.Size(2)-1};

    T * pMask=NULL;
    for (int z=1; z<end[2]; z++) {
        for (int y=1; y<end[1]; y++) {
            pMask=mask.GetLinePtr(y,z);
            DistanceTripple<N> * pDist= dist.GetLinePtr(y,z);

            for (int x=1; x<end[0]; x++) {
                if (pMask[x]) {

                    float mindist=std::numeric_limits<float>::max();
                    int index=0;
                    for (size_t i=1; i<m_nNeighborhoodSize; i++) {

                        int pos=x-m_nIndex[i];
                        float val=pDist[pos].Distance2();
                        if ((pMask[pos]) && (val<mindist)) {
                            mindist=val;
                            index=i;
                        }
                    }
                    if (index!=0)
                        pDist[x]=pDist[x-m_nIndex[index]]+m_Distances[index];
                    else
                        pDist[x]=firstdist;
                }
            }
        }
    }

//	for (int z=end[2]-1; 1<=z; z--) {
//		for (int y=end[1]-1; 1<=y; y--) {
//			pMask=mask.GetLinePtr(y,z);
//			DistanceTripple<N> * pDist= dist.GetLinePtr(y,z);
//
//			for (int x=end[0]-1; 1<=x; x--) {
//				if (pMask[x]) {
//					float mindist=pDist[x].Distance2();
//
//					int index=0;
//					for (size_t i=1; i<m_nNeighborhoodSize; i++) {
//						int pos=x+m_nIndex[i];
//						float val=pDist[pos].Distance2(m_Distances[i]);
//						if (val<mindist) {
//							mindist=val;
//							index=i;
//						}
//					}
//					if (index!=0) {
//						pDist[x]=pDist[x+m_nIndex[index]]+m_Distances[index];
//					}
//				}
//			}
//		}
//	}


    for (size_t i=0; i<distance.Size(); i++) {
        distance[i]=dist[i].Distance();
    }

    return distance;
}

template <typename T, size_t N>
void DanielssonDistance<T,N>::InitializeIndexing(size_t const * const dims)
{
    ptrdiff_t sx=dims[0];
    ptrdiff_t sxy=dims[0]*dims[1];

    m_nIndex[0]=0;
    m_nIndex[1]=1;

    m_nIndex[2]=sx-1;
    m_nIndex[3]=sx;
    m_nIndex[4]=sx+1;

    m_nIndex[5]=sxy-sx-1;
    m_nIndex[6]=sxy-sx;
    m_nIndex[7]=sxy-sx+1;
    m_nIndex[8]=sxy-1;
    m_nIndex[9]=sxy;
    m_nIndex[10]=sxy+1;
    m_nIndex[11]=sxy+sx-1;
    m_nIndex[12]=sxy+sx;
    m_nIndex[13]=sxy+sx+1;

}

}}}
#endif /*  */

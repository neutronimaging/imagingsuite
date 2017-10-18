//<LICENCE>

#ifndef __DRAWING_HPP
#define __DRAWING_HPP

namespace kipl { namespace drawing {

template <typename T>
void Sphere<T>::Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod)
{
	int sx=img.Size(0);
	int sy=img.Size(1);
	int sz=img.Size(2);
	for (int i=-m_nRadius; i<=m_nRadius; i++) {
		int zi=i+z;

		if ((0<=(zi)) && (zi<sz)) {
			float ri2=i*i;
			for (int j=-m_nRadius; j<=m_nRadius; j++) {
				int yj=y+j;
				if ((0<=yj) && (yj<sy)) {
					float rj2=ri2+j*j;
					T *pLine=img.GetLinePtr(yj,zi);
					int begin=(x-m_nRadius)<0 ? -x : -m_nRadius;
					int end=(x+m_nRadius)<=sx ? sx-x: m_nRadius;

					for (int k=begin; k<=end; k++) {
						float sum=rj2+k*k;
						if (sum<=m_fRadius2) {
							pLine[k+x]=value;
						}
					}
				}
			}
		}
	}

}

template <typename T>
void Cube<T>::Draw(kipl::base::TImage<T,3> &img, int x, int y, int z, T value, ePaintMethod paintmethod)
{
	int half=m_nSide>>1;

	T *pLine=NULL;
	for (int zz=0; zz<m_nSide; zz++) {
		for (int yy=0; yy<m_nSide; yy++) {
			pLine=img.GetLinePtr(y+yy-half,z+zz-half)-x+half;
			for (int xx=0; xx<m_nSide; xx++) {
				pLine[xx]=value;
			}
		}
	}

}

}}
#endif

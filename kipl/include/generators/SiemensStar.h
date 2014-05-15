#ifndef __SIEMENSSTAR_H
#define __SIEMENSSTAR_H

#include "../base/timage.h"
#include "../math/mathconstants.h"

namespace kipl { namespace generators {

template<typename T>
void SiemensStar(size_t N, size_t Spokes, kipl::base::TImage<T,2> *img, bool centered=true)
{
	size_t dims[2]={N,N};

	img->Resize(dims);
	T *pImg=img->GetDataPtr();

	const double N2=static_cast<float>(N/2);
	double R=N2*N2*0.95f;
	double Rcenter=Spokes/dPi; Rcenter*=Rcenter;
	double x,y;

	size_t i=0;
	double bias= centered ? -0.5 : 0.0;
	for (y=-N2; y<N2; y++) {

		for (x=-N2; x<N2; x++, i++) {

			double r=x*x+y*y;

			if ((r<R) && (Rcenter<r)) {
				pImg[i]=abs(fmod(floor(Spokes*atan2(x,y)/fPi),2.0))+bias;
			}
			else {

				pImg[i]=r<=Rcenter ? 1+static_cast<T>(bias) : bias;
			}

		}
	}

}


}}


#endif

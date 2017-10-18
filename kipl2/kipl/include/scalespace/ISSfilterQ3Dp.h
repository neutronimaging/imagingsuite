//<LICENCE>

#ifndef __ISSFILTERQ3DP_H
#define __ISSFILTERQ3DP_H
#include <iostream>
#include <list>

#include <base/timage.h>
#include "../logging/logger.h"
#include "filterenums.h"

namespace akipl { namespace scalespace {

template <typename T>
class ISSfilterQ3Dp {
	kipl::logging::Logger logger;
public:
	
  ISSfilterQ3Dp() :logger("ISSfilterQ3Dp"), eInitialImage(InitialImageOriginal),error(NULL) {m_dEpsilon=1e-7;}
  ~ISSfilterQ3Dp() {if (error!=NULL) delete [] error;}
  int Process(kipl::base::TImage<T,3> &img,
		  double dTau,
		  double dLambda,
		  double dAlpha,
		  int nN,
		  bool saveiterations=false,
		  std::string itpath="");

   T const * const GetErrorArray() {return error;}
   void SetNumThreads(int n) {m_nThreads=n;}
   eInitialImageType eInitialImage;
private:
	enum Direction {
		dirX=0,
		dirY=1,
		dirZ=2
	};
	
  kipl::base::TImage<T,3> m_f;
  kipl::base::TImage<T,3> m_v;
  int m_nThreads;
  double m_dTau; 
  double m_dLambda;
  double m_dAlpha;
  double m_dEpsilon;
  T *error;
  T _SolveIteration(kipl::base::TImage<T,3> &img);
  T _SolveIterationSSE(kipl::base::TImage<T,3> &img);
  int _P(kipl::base::TImage<T,3> &img, kipl::base::TImage<T,3> &res);
  int _LeadRegularize(kipl::base::TImage<T,3> *img, Direction dir);
  int _LagRegularize(kipl::base::TImage<T,3> *img, Direction dir);
  int _FirstDerivate(   std::list<kipl::base::TImage<T,2> > &img_queue,
						std::list<kipl::base::TImage<T,2> > &dx_queue,
						std::list<kipl::base::TImage<T,2> > &dy_queue,
						std::list<kipl::base::TImage<T,2> > &dz_queue); // Lag difference

  int _FirstDerivateSSE(std::list<kipl::base::TImage<T,2> > &img_queue,
  									std::list<kipl::base::TImage<T,2> > &dx_queue,
  									std::list<kipl::base::TImage<T,2> > &dy_queue,
  									std::list<kipl::base::TImage<T,2> > &dz_queue); // Lag difference


  int _FirstDerivateSSE2(std::list<kipl::base::TImage<T,2> > &img_queue,
  									std::list<kipl::base::TImage<T,2> > &dx_queue,
  									std::list<kipl::base::TImage<T,2> > &dy_queue,
  									std::list<kipl::base::TImage<T,2> > &dz_queue); // Lag difference
  int _Curvature(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result); // Lead difference

  int _CurvatureSSE(std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result); // Lead difference


};

}}


#include "core/ISSfilterQ3Dp.hpp"

#endif

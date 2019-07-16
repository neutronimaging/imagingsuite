//<LICENSE>
#ifndef ISSFILTERQ3D_H
#define ISSFILTERQ3D_H
#include <iostream>
#include <list>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>
#include <interactors/interactionbase.h>

#include "filterenums.h"

namespace advancedfilters {

template <typename T>
class ISSfilterQ3D {
	kipl::logging::Logger logger;
public:
  ISSfilterQ3D(kipl::interactors::InteractionBase *interactor=nullptr);
  ~ISSfilterQ3D();
  int Process(kipl::base::TImage<T,3> &img,
		  double dTau,
		  double dLambda,
		  double dAlpha,
		  int nN,
		  bool saveiterations=false,
		  std::string itpath="");

   T const * const GetErrorArray() {return error;}
   T const * const GetEntropyArray() { return entropy;}

   void SetNumThreads(int n) {m_nThreads=n;}
   eInitialImageType eInitialImage;
   eRegularizationType m_eRegularization;
private:
   kipl::interactors::InteractionBase *m_Interactor;
	enum Direction {
		dirX=0,
		dirY=1,
		dirZ=2
	};
	
  kipl::base::TImage<T,3> m_f;
  kipl::base::TImage<T,3> m_v;
  
  double m_dTau; 
  double m_dLambda;
  double m_dAlpha;
  double m_dEpsilon;


  int m_nThreads;
  T *error;
  T *entropy;
  size_t hist[512];
  size_t axis[512];

  kipl::base::TImage<T,2> GetPaddedSlice(kipl::base::TImage<T,3> &vol, size_t nIdx, size_t nMargin);
  void InsertPaddedSlice(kipl::base::TImage<T,2> & slice, kipl::base::TImage<T,3> &vol, size_t nIdx, size_t nMargin);

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


  int _Curvature(   std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result); // Lead difference

  int _CurvatureSSE(std::list<kipl::base::TImage<T,2> > &dx_queue,
					std::list<kipl::base::TImage<T,2> > &dy_queue,
					std::list<kipl::base::TImage<T,2> > &dz_queue,
					kipl::base::TImage<T,2> &result); // Lead difference

  double _ComputeEntropy(kipl::base::TImage<T,3> &img);

  /// \param val a fraction value 0.0-1.0 to tell the progress of the back-projection.
  /// \param msg a message string to add information to the progress bar.
  /// \returns The abort status of interactor object. True means abort back-projection and false continue.
  bool updateStatus(float val, std::string msg);

};

}


#include "ISSfilterQ3D.hpp"

#endif

//
// C++ Interface: ISSfilter
//
// Description: 
//
//
// Author: Anders Kaestner <anders.kaestner@solnet.ch>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __ISSFILTER_H
#define __ISSFILTER_H
#include <iostream>
#include <base/timage.h>

namespace akipl { namespace scalespace {

template <typename ImgType, size_t NDims=2>
class ISSfilter {
	std::ostream & logstream;
public:
	
  ISSfilter(std::ostream &os=std::cout) :logstream(os),error(NULL) {m_dEpsilon=1e-7;}
  ~ISSfilter() {if (error!=NULL) delete [] error;}
  int Process(kipl::base::TImage<ImgType,NDims> &img, double dTau, double dLambda, double dAlpha, int nN);

   ImgType const * const GetErrorArray() {return error;}
private:
	enum Direction {
		dirX=0,
		dirY=1,
		dirZ=2
	};
	
  kipl::base::TImage<ImgType,NDims> m_f;
  kipl::base::TImage<ImgType,NDims> m_v;
  
  double m_dTau; 
  double m_dLambda;
  double m_dAlpha;
  double m_dEpsilon;
  ImgType *error;
  ImgType _SolveIteration(kipl::base::TImage<ImgType,NDims> &img);
  int _LeadDiff(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<ImgType,NDims> &diff, Direction dir);
  int _LagDiff(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<ImgType,NDims> &diff, Direction dir);
  int _P(kipl::base::TImage<ImgType,NDims> &img, kipl::base::TImage<ImgType,NDims> &res);
  int _LeadRegularize(kipl::base::TImage<ImgType,NDims> *img, Direction dir);
  int _LagRegularize(kipl::base::TImage<ImgType,NDims> *img, Direction dir);
};

}}

#include "core/ISSfilter.hpp"

#endif

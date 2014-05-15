/***************************************************************************
 *   Copyright (C) 2004 by Anders Kaestner                                 *
 *   anders.kaestner@env.ethz.ch                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef __JAMA_INVERSES_H
#define __JAMA_INVERSES_H

#include <tnt_vec.h>
#include <tnt_cmat.h>
#include <tnt_math_utils.h>
#include <jama_svd.h>

using namespace TNT;

namespace JAMA
{
// This class requires to modify the JAMA_SVD to declare the private members to protected
	template<class Real>
	class TSVD: public SVD<Real>
	{
	
		public:
			TSVD(const Array2D<Real> &Arg);
		
			Vector<Real> solve (const Vector<Real> &b, int tval=-1);
			Vector<Real> solve (const Vector<Real> &b, double lambda);
	};


	template<class Real>
	TSVD<Real>::TSVD(const Array2D<Real> &Arg): SVD<Real>(Arg)
	{}
	
	template<class Real>
	Vector<Real> TSVD<Real>::solve (const Vector<Real> &b, int tval) 
	{
		if (b.size()!=this->s.dim()) {
			cerr<<"TSVD::solve() : size miss match"<<endl;
			return b;
		}	
		
		int t=0;
		if ((tval<0))
			t=this->rank();
		else
			t=tval;
	
		
		int i, j;
		Vector<Real> w(this->s.dim(),(Real)0);
		Vector<Real> tmp(this->s.dim());

		Real coef;
		for (i=0; i<t; i++) {
			coef=(Real) 0;
			for (j=0; j<this->s.dim(); j++) {
				coef+=(this->U[j][i]*b[j]);
			}
			coef=coef/this->s[i];
			for (j=0; j<this->s.dim(); j++) {
				tmp[j]=(this->V[j][i]*coef);
			}
			w=w+tmp;
		
		}	
		
		return w;
		
		
	}
	
		template<class Real>
	Vector<Real> TSVD<Real>::solve (const Vector<Real> &b, double lambda) 
	{
		if (b.size()!=this->s.size()) {
			cerr<<"TSVD::solve() : size miss match"<<endl;
			return b;
		}	
		
		
		int i, j;
		Vector<Real> w(this->s.size(),(Real)0);
		Vector<Real> tmp(this->s.size());

		Real coef;
		double l2=lambda*lambda;
		double s2;
		for (i=0; i<this->s.size(); i++) {
			coef=(Real) 0;
			for (j=0; j<this->s.size(); j++) {
				coef+=(this->U[j][i]*b[j]);
			}
			s2=this->s[i]*this->s[i];
			coef=(s2/(s2+l2))*coef/this->s[i];
			for (j=0; j<this->s.size(); j++) {
				tmp[j]=(this->V[j][i]*coef);
			}
			w=w+tmp;
		
		}	
		
		return w;
		
		
	}

}

#endif 

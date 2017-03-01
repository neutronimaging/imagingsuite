//<LICENCE>

#ifndef __WAVELETS_H
#define __WAVELETS_H

#include <iostream>
#include <list>

#include "../base/timage.h"
#include "../logging/logger.h"
#include "../base/kiplenums.h"

namespace kipl { namespace wavelets {

template <typename T>
	class WaveletKernel
	{
		std::string m_sName;
	public:
		WaveletKernel(std::string name);
		std::string name() const {return m_sName;}
		int begin() const {return m_nBegin;}
		int end() const {return m_nEnd;}
		int size() const {return m_nEnd-m_nBegin+1;}

		T const * const transH() const {return m_tH-m_nBegin;}
		T const * const transG() const {return m_tG-m_nBegin;}

		T const * const synthH() const {return m_sH-m_nBegin;}
		T const * const synthG() const {return m_sG-m_nBegin;}
		std::list<std::string> NameList();

	private:
		void BuildKernel(std::string name);
		void CopyKernel(double *k, int start, int end);
		int m_nBegin;
		int m_nEnd;

		// Transform kernels
		T m_tH[512];
		T m_tG[512];

		// Synth kernels
		T m_sH[512];
		T m_sG[512];

	};

	template <typename T>
	class WaveletQuad {
	public:
		WaveletQuad() : N(0) {}
		WaveletQuad(size_t const * const dims) : N(0),a(dims), h(dims), v(dims), d(dims) {a=T(0); h=T(0); v=T(0); d=T(0);}
		void Resize(size_t const * const dims) { a.Resize(dims); h.Resize(dims); v.Resize(dims); d.Resize(dims); }
		void SaveQuad(std::string fname);
		int N;
		kipl::base::TImage<T,2> a;
		kipl::base::TImage<T,2> h;
		kipl::base::TImage<T,2> v;
		kipl::base::TImage<T,2> d;
	};

	template <typename T>
	class WaveletTransform
	{
	public:
		WaveletTransform(std::string name);

		void transform(kipl::base::TImage<T,2> img, int N);
		kipl::base::TImage<T,2> synthesize();
		void SaveSpectrum(std::string fname);

		list<WaveletQuad<T> > data;

		kipl::base::ePadType padtype;

		const WaveletKernel<T> & Kernel() const {return kernel;}
	protected:
		size_t n_dims[2];

		WaveletQuad<T> transform(WaveletQuad<T> &q);
		void transform(kipl::base::TImage<T,2> &src,
				kipl::base::TImage<T,2> &dst,
				T const * const H,
				T const * const V,
				int begin, int end);

		void synthesize(WaveletQuad<T> &q, kipl::base::TImage<T,2> &img);
	public:
		void pad_mirror(T * src, int nsrc, T * dst, int insert_pos);
		void pad_periodic(T * src, int nsrc, T * dst, int insert_pos);
		void pad_zero(T * src, int nsrc, T * dst, int insert_pos);
		void pad_sp0(T * src, int nsrc, T * dst, int insert_pos);
		void pad_sp1(T * src, int nsrc, T * dst, int insert_pos);
	protected:
		void get_col(kipl::base::TImage<T,2> &src, T* col, int xpos, int ypos, T len);
		void set_col(T* col, kipl::base::TImage<T,2> &dst, int xpos, int ypos, T len, int step=1);
		void dyadup(T* src, T* dst, int N);
		void upsconv(kipl::base::TImage<T> & src, kipl::base::TImage<T> & dst, T const * const kh, T const * const kv);
		void upsconvRC(kipl::base::TImage<T> & src, kipl::base::TImage<T> & dst, T const * const kh, T const * const kv);
		void upsconvCR(kipl::base::TImage<T> & src, kipl::base::TImage<T> & dst, T const * const kh, T const * const kv);
		WaveletKernel<T> kernel;

	};

}}

template <typename T>
std::ostream & operator<<(std::ostream & s, const kipl::wavelets::WaveletKernel<T> &wk);

template <typename T>
std::ostream & operator<<(std::ostream & s, const kipl::wavelets::WaveletTransform<T> &wt);

#include "core/wavelets.hpp"

#endif

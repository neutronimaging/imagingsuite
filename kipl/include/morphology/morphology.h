//
// This file is part of the i KIPL image processing library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//
#ifndef __MORPHOLOGY_H
#define __MORPHOLOGY_H

//#include "filter.h"
#include <cstddef>
#include <string>

namespace kipl { namespace morphology {
	/// Enum used by the TopHat transform
	typedef enum {
		TH_black=0,
		TH_white
	} TopHatType;

	/// Enum used to select connectivity of morphological algorithms
	typedef enum {
		conn4=0, 
		conn8,
		conn6,
		conn18,
		conn26,
		euclid
	} MorphConnect;

	
	/// \brief Support class to compute the indices for a neighborhood at a given connectivity
	/// The class does edge checking
	class CNeighborhood {
	public: 
		/// \brief Constructor to configure the neighborhood
		/// \param dims dimension sizes of the image
		/// \param NDim Number of dimensions
		/// \param conn Selects connectivity of the neighborhood
		CNeighborhood(size_t const * const dims, size_t const NDim, kipl::morphology::MorphConnect const conn=conn4);
		
		/// \brief Forward looking neighborhood 
		/// \param pos one-dim index to a pixel
		/// \param index neighborhood index
		///	\retval >=0 index to a neighborhood pixel of pixel at pos
		///	\retval -1 Neighborhood pixel does not exist for the pos/index combination
		ptrdiff_t forward(size_t pos, size_t index);
		
		/// \brief Backward looking neighborhood 
		///	\param pos one-dim index to a pixel
		///	\param index neighborhood index
		///
		///	\retval >=0 index to a neighborhood pixel of pixel at pos
		///	\retval -1 Neighborhood pixel does not exist for the pos/index combination
		ptrdiff_t backward(size_t pos, size_t index);

		/// \brief Full neighborhood 
		///	\param pos one-dim index to a pixel
		///	\param index neighborhood index
		///
		///	\retval >=0 index to a neighborhood pixel of pixel at pos
		///	\retval -1 Neighborhood pixel does not exist for the pos/index combination
		ptrdiff_t neighbor(size_t pos, size_t index);
		
		/// Number of forward or backward pixels
		size_t Nfb() { return cNGpm; }
		
		/// Number of neighborhood pixels
		size_t N() { return cNG; }
		ptrdiff_t index3x3x3(size_t index);

		/// destructor
		virtual ~CNeighborhood() {}
	protected:
		/// \brief Forward looking neighborhood for 2D images
		/// \param pos pixel index
		/// \param index neighborhood index
		///
		/// \returns The neighbor pixel index
		/// \retval non-negative valid pixel index
		/// \retval -1 out of bounds
		ptrdiff_t forward2D(size_t pos,size_t index);
		/// \brief Backward looking neighborhood for 2D images
		/// \param pos pixel index
		/// \param index neighborhood index
		/// 
		/// \returns The neighbor pixel index
		/// \retval non-negative valid pixel index
		/// \retval -1 out of bounds
		ptrdiff_t backward2D(size_t pos, size_t index);

		/// \brief Neighborhood for 1D data
		/// \param pos pixel index
		/// \param index neighborhood index
		/// 
		/// \returns The neighbor pixel index
		/// \retval non-negative valid pixel index
		/// \retval -1 out of bounds
		ptrdiff_t neighbor1D(size_t pos, size_t index);

		/// \brief Neighborhood for 2D images
		/// \param pos pixel index
		/// \param index neighborhood index
		/// 
		/// \returns The neighbor pixel index
		/// \retval non-negative valid pixel index
		/// \retval -1 out of bounds
		ptrdiff_t neighbor2D(size_t pos, size_t index);
		/// \brief Forward looking neighborhood for 2D images
		/// \param pos pixel index
		/// \param index neighborhood index
		/// \param cnt size of the neighborhood
		/// \param ng array with neighborhood indices
		/// \param left Controls the edge processing on the left side
		/// \param right Controls the edge processing on the right side
		/// \param front Controls the edge processing on the front
		/// \param back Controls the edge processing on the back
		/// 		
		/// \returns The neighbor pixel index
		/// \retval non-negative valid pixel index
		/// \retval -1 out of bounds
		ptrdiff_t neighbor3D(size_t pos, size_t index,int cnt,ptrdiff_t *ng, bool *left, bool *right, bool *front, bool *back);
		
		private: 
		ptrdiff_t NG[27];
		bool NG_left[27];
		bool NG_right[27];
		bool NG_front[27];
		bool NG_back[27];
		ptrdiff_t NGp[27];
		bool NGpm_a[27];
		bool NGpm_b[27];
		bool NGpm_c[27];
		bool NGpm_d[27];
		ptrdiff_t NGm[27];
		size_t cNG;
		size_t cNGpm;
		size_t dims[8];
		int ndim;
		size_t nimg;
		size_t first_line;
		size_t first_slice;
		size_t last_line;
		size_t last_slice;
		bool isEdge;
		size_t currentPos;
		ptrdiff_t mpos;
		ptrdiff_t smpos;
		ptrdiff_t vmpos;
		
		ptrdiff_t sx;
		ptrdiff_t sxy;
	};
	
	
/*
	///\brief Base class to provide basic morphological functionality
	/// 
	/// This class is only defined to operate on CImage<char>
	template<class ImgType,int NDim>
	class CMorphology : protected Filter::CBaseFilter<ImgType,NDim>
	{
	public:
		/// \brief Set the 2D opertation kernel of the operator
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		int SetKernel(const char *k,int x,int y);
		
		/// \brief Set the opertation kernel of the operator
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		int SetKernel(const char *k,unsigned int *Dims);
		
		/// \brief Basic constructor 
		CMorphology();
		
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		CMorphology(const char *k,int x,int y);
		
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		CMorphology(const char *k,unsigned int *Dims);

		/// \brief Destructor that deallocates all used memory
		virtual ~CMorphology();

	protected:
		/// Computes hit or miss for a given index location
		/// \param img Source image
		/// \param x Index

		char HitMiss(CImage<ImgType,NDim> &img, int x);
		/// The operation kernel
		char * kernel;

	};

	/// Class to perforn erosion
	template<class ImgType,int NDim>
	class CErode : public CMorphology<ImgType,NDim>
	{
	public:
		/// \brief Basic constructor
		CErode() {}
		
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		CErode(const char *k,int x,int y) : CMorphology<ImgType,NDim>(k,x,y) {}
		
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		CErode(const char *k,unsigned int *Dims) : CMorphology<ImgType,NDim>(k,Dims) {} 
		/// \brief The erosion operator
		///	\param img Source image
		CImage<ImgType,NDim> operator()(CImage<ImgType,NDim> &img);
		
		virtual ~CErode() {}
	};


	/// Class to perform dilation
	template<class ImgType, int NDim>
	class CDilate : public CMorphology<ImgType,NDim>
	{
	public:
		/// \brief Basic constructor
		CDilate() {}

		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		CDilate(const char *k,int x,int y) : CMorphology<ImgType,NDim>(k,x,y) {} 

		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		CDilate(const char *k,unsigned int *Dims) : CMorphology<ImgType,NDim>(k,Dims) {} 
		/// The dilation operator
		CImage<ImgType,NDim> operator()(CImage<ImgType,NDim> &img);
		
		virtual ~CDilate() {}
	};

	/// Class to perforn erosion
	template <class ImgType,int NDim>
	class CGrayErode : public CMorphology<ImgType,NDim>
	{
	public:
		/// \brief Basic constructor
		CGrayErode() {}

		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		CGrayErode(const char *k,int x,int y) : CMorphology<ImgType,NDim>(k,x,y) {} 
		
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		CGrayErode(const char *k,unsigned int *Dims) : CMorphology<ImgType,NDim>(k,Dims) {} 
		/// The erosion operator
		CImage<ImgType,NDim> operator()(CImage<ImgType,NDim> &img);
		virtual ~CGrayErode() {}
	};


	/// Class to perform dilation
	template <class ImgType,int NDim>
	class CGrayDilate : public CMorphology<ImgType,NDim>
	{
	public:
		/// \brief Basic constructor
		CGrayDilate() {}
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param x Size of the x dimension
		///	\param y Size of the y dimension
		CGrayDilate(const char *k,int x,int y) : CMorphology<ImgType,NDim>(k,x,y) {} 
		/// \brief Creates a 2D filter and initializes it with kernel
		///	\param k Pointer to the kernel array
		///	\param Dims Pointer to the array with dimension sizes
		CGrayDilate(const char *k,unsigned int *Dims) : CMorphology<ImgType,NDim>(k,Dims) {} 
		/// The dilation operator
		CImage<ImgType,NDim> operator()(CImage<ImgType,NDim> &img);
		virtual ~CGrayDilate() {} 
	};
*/
	/// Predefined four-neigbourhood disk
	const char disk4_3x3[]={0,1,0,1,1,1,0,1,0};
	/// Predefined 6-neigbourhood disk
	const char ball6_3x3x3[]={0,0,0,0,1,0,0,0,0,
							  0,1,0,1,1,1,0,1,0,
							  0,0,0,0,1,0,0,0,0};
	/// Predefined eight-neigbourhood disk
	const char disk8_3x3[]={1,1,1,1,1,1,1,1,1};
	/// Predefined 27-neigbourhood disk
	const char ball27_3x3x3[]={1,1,1,1,1,1,1,1,1,
							   1,1,1,1,1,1,1,1,1,
							   1,1,1,1,1,1,1,1,1};
	/// Predefined octagon disk 5x5
	const char disk_5x5[]={0,1,1,1,0, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 0,1,1,1,0};
	/// Predefined octagon disk 7x7
	const char disk_7x7[]={0,0,1,1,1,0,0, 0,1,1,1,1,1,0, 1,1,1,1,1,1,1, 1,1,1,1,1,1,1, 0,1,1,1,1,1,0, 0,0,1,1,1,0,0};
	const char disk_9x9[]={0,0,0,1,1,1,0,0,0,
						   0,0,1,1,1,1,1,0,0, 
						   0,1,1,1,1,1,1,1,0, 
						   1,1,1,1,1,1,1,1,1, 
						   1,1,1,1,1,1,1,1,1, 
						   1,1,1,1,1,1,1,1,1,
						   0,1,1,1,1,1,1,1,0,
						   0,0,1,1,1,1,1,0,0,
						   0,0,0,1,1,1,0,0,0};
	/// Predefined forward neighborhood 4conn
	const char ng_f_4[]={1,1,1,0};
	/// Predefined backward neighborhood 4conn
	const char ng_b_4[]={0,1,1,1};
	/// Predefined forward and backward neighborhood 8conn
	const char ng_8[]={1,1,1,1};

	/// \brief Base class to implement a structure element
	/// \todo Implement the needed infrastructure to use structure element objects
	class CStructureElement
	{
	public:
		/// Default construtor of the structure element
		CStructureElement(void);
		/// Destructor of the structure element
		virtual ~CStructureElement(void);
	};

}}

std::string enum2string(kipl::morphology::MorphConnect mc);
std::ostream & operator<<(std::ostream &s, kipl::morphology::MorphConnect mc);
void string2enum(std::string str, kipl::morphology::MorphConnect &mc);

#endif



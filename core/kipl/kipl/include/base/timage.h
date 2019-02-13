//<LICENCE>

#ifndef TIMAGE_H_
#define TIMAGE_H_
#include <stdexcept>
#include <complex>

#include "core/sharedbuffer.h"
#include "imageinfo.h"

namespace kipl {
/// \brief The namespace contains classes for basic image handling
namespace base {

/// \brief Container class that represents an image
/// The class manages the internal buffer with refenece counting to save time and memory.
template<typename T, size_t N=2>
class TImage {
  /// \brief The unit test class used to verify the function of the image class is allowed to access private members.
public:
  /// \brief Default c'tor creates an empty image
	TImage();
	/// \brief Copy c'tor
	/// The constructor does only do a shallow copy
	/// \param img Image to be copied
	TImage(const TImage<T,N> &img);
	/// \brief Constructor to specify the image size
	/// \param dims Array containing the dimensions of the image. The first index in the dimension array refers to the fast index increment in the image.
	TImage(size_t const * const dims);
	/// \brief D'tor for the image class. 
	~TImage();
	
	/// \brief Assignment operator. Performs a shallow copy of the source image.
	/// \param img Image to be copied
	/// \test The method is tested with unit test
	const TImage & operator=(const TImage<T,N> &img);
	
	/// \brief Assigns a scalar value to all pixels in the image.
	/// \param value The scalar to assign.
	/// \test The method is tested with unit test
	const TImage & operator=(const T value);
	
	/// \returns The length of the data buffer
	/// \test The method is tested with unit test
	size_t Size() const { return m_buffer.Size(); }
	
	/// \param n Dimension index (starts with 0)
	/// \returns The length of dimension n
	/// \test The method is tested with unit test
	size_t Size(size_t n) const { 
		if (N<=n) { 
			ostringstream msg;
			msg<<"An invalid dimension "<<n<<" was requested in TImage<"<<N<<">::Size(n)";
			throw KiplException(msg.str(),__FILE__,__LINE__);
		}
		
		return m_Dims[n];	
	}
	
	void FreeImage() { size_t d[8]={1,1,1,1,1,1,1,1}; Resize(d); }

	size_t Resize(size_t const * const dims);
	/// \brief Element access operator
	/// \param index Sequential index of a pixel in the image
	/// \test The operator is tested by a unit test
	T & operator[](const size_t index) {return m_buffer[index];}
	T operator[](const size_t index) const {return m_buffer[index];}

    /// \brief Coordinate access of a pixel
    /// \param x x-coordinate
    /// \param y y-coordinate
    /// \param z z-coordinate
    T & operator()(size_t x=0, size_t y=0, size_t z=0);

    /// \brief Coordinate access of a pixel
    /// \param x x-coordinate
    /// \param y y-coordinate
    /// \param z z-coordinate
    T & operator()(int x=0, int y=0, int z=0);
	
	/// \brief Gives the number of references to the current memory block
	/// \test The method is tested with unit test
	int References() {return m_buffer.References();}
	// Arithmetic operators
	TImage<T,N> & operator+=(TImage<T,N> &img); //< Operator to add two images
	TImage<T,N> & operator-=(TImage<T,N> &img);
	TImage<T,N> & operator*=(TImage<T,N> &img);
	TImage<T,N> & operator/=(TImage<T,N> &img);
	TImage<T,N> & operator+=(const T x);
	TImage<T,N> & operator-=(const T x);
	TImage<T,N> & operator*=(const T x);
	TImage<T,N> & operator/=(const T x);

    TImage<T,N> operator+(const T x) const;
    TImage<T,N> operator-(const T x) const;
    TImage<T,N> operator*(const T x) const;
    TImage<T,N> operator/(const T x) const;
	
    /// \brief Returns the pointer to the data
	/// \test The method is tested with unit test
	T * GetDataPtr() { return m_buffer.GetDataPtr();}
	
	/// \brief Returns the pointer to the data for image constants
     const T * GetDataPtr() const { return m_buffer.GetDataPtr();}
	
	/// \brief Returns the pointer to the specified line in the image
	/// \param y Line index in the second image dimension
	/// \param z Line index in the third image dimension (this will be ignored for 2D images)
	/// \test The method is tested with unit test
	T * GetLinePtr(size_t y, size_t z=0) { 
		switch (N) { 
		case 1 : return m_buffer.GetDataPtr();
		case 2 : return m_buffer.GetDataPtr()+y*m_Dims[0];
		case 3 : return m_buffer.GetDataPtr()+y*m_Dims[0]+z*m_Dims[0]*m_Dims[1];
		}
		return m_buffer.GetDataPtr();
	}
	
	/// \brief Returns the pointer to the specified line in the image constant
	/// \param y Line index in the second image dimension
	/// \param z Line index in the third image dimension (this will be ignored for 2D images)	
     const T * GetLinePtr(size_t y, size_t z=0) const {
		switch (N) { 
		case 1 : return m_buffer.GetDataPtr();
		case 2 : return m_buffer.GetDataPtr()+y*m_Dims[0];
		case 3 : return m_buffer.GetDataPtr()+y*m_Dims[0]+z*m_Dims[0]*m_Dims[1];
		}
		return m_buffer.GetDataPtr();
	}
	
	/// \brief Returns a reference to the dimension array
    const size_t * Dims() const {return m_Dims;}

    /// \brief Makes a current image a deep copy of the data.
	void Clone();

    /// \brief Makes a deep copy of the data in the provided image.
    /// \param img Source image
    void Clone(const kipl::base::TImage<T,N> &img);

	ImageInfo info;
private:
  /// \brief Computes to number of elements to allocate for the image
  /// \param dims Array containing the dimensions of the image
	size_t _ComputeNElements(size_t const * const dims);
	
	size_t m_NData;
	
	/// \brief Buffer constaining the image data
	kipl::base::core::buffer<T> m_buffer;

	/// \brief Dimension array
	size_t m_Dims[N];
};

/// \brief Computes the pixelwise sum between two images
/// \param imgA The first term
/// \param imgB The second term
/// \returns The pixewise sum
template<typename T, size_t N>
TImage<T,N> operator+(TImage<T,N> &imgA, TImage<T,N> &imgB);

/// \brief Computes the pixelwise difference between two images
/// \param imgA The first term
/// \param imgB The second term
/// \returns The pixewise difference
template<typename T, size_t N>
TImage<T,N> operator-(TImage<T,N> &imgA, TImage<T,N> &imgB);

/// \brief Computes the pixelwise product between two images
/// \param imgA The first factor
/// \param imgB The second factor
/// \returns The pixewise sum
template<typename T, size_t N>
TImage<T,N> operator*(TImage<T,N> &imgA, TImage<T,N> &imgB);

/// \brief Computes the pixelwise division between two images
/// \param imgA The numerator
/// \param imgB The denominator
/// \returns The pixewise sum
template<typename T, size_t N>
TImage<T,N> operator/(TImage<T,N> &imgA, TImage<T,N> &imgB);

/// \brief Send information about the image to a stream
/// \param s target stream
/// \param img the image
/// \returns A reference to the stream
template<typename T, size_t N>
std::ostream & operator<<(std::ostream &s, const TImage<T,N> &img);

/// \brief Compares if two images have the same size
/// \param img1 the first image
/// \param img2 the second image
/// \returns true if the images have the same dimensions
template<typename T, size_t N>
bool CheckEqualSize(TImage<T,N> &img1, TImage<T,N> &img2);
}}

// Implementation of the methods
#include "core/timage.hpp"

#endif /*TIMAGE_H_*/

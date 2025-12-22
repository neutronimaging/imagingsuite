//<LICENCE>

#ifndef TSUBIMAGE_HPP_
#define TSUBIMAGE_HPP_

#include "../timage.h"
#include "../KiplException.h"
#include "../tsubimage.h"

#include <algorithm>
#include <span>
#include <ranges>
#include <format>

namespace kipl { namespace base {

// Helper functions for mirror boundary handling

// Reflect an index into [0, n-1] using "mirror" boundary (…3 2 1 | 0 1 2 3 2 1…)
inline std::size_t reflect_index(std::ptrdiff_t i, std::size_t n) {
    if (n == 0) return 0;
    if (n == 1) return 0;
    // bring i into [-(n-1), +infty)
    if (i < 0) {
        // mirror to positive by periodic extension of length 2*(n-1)
        // i = -i - 1; // reflect over -0.5
		i = -i;
    }
    const std::ptrdiff_t period = 2 * (static_cast<std::ptrdiff_t>(n) - 1);
    i = i % period;
    if (i < 0) i += period;
    // fold the upper half back
    if (i >= static_cast<std::ptrdiff_t>(n)) {
        i = period - i;
    }
    return static_cast<std::size_t>(i);
}

// 1D: read with mirror boundary via span
template<typename T>
inline const T& mirror_at(std::span<const T> s, std::ptrdiff_t i) {
    return s[reflect_index(i, s.size())];
}

// 1D: fill a destination span with mirrored padding from source span
template<typename T>
inline void copy_with_mirror_padding(std::span<const T> src,
                                     std::span<T>       dst,
                                     std::ptrdiff_t     start_in_src)
{
    for (std::ptrdiff_t k = 0; k < static_cast<std::ptrdiff_t>(dst.size()); ++k) {
        dst[k] = mirror_at(src, start_in_src + k);
    }
}

// 2D helper: mirror-access a row span and column index
template<typename T>
inline const T& mirror_at_2d(std::span<const std::span<const T>> rows,
                             std::ptrdiff_t y, std::ptrdiff_t x)
{
    auto ry = reflect_index(y, rows.size());
    auto rx = reflect_index(x, rows[ry].size());
    return rows[ry][rx];
}

// ...existing code...
template<typename T>
void copy_rect_with_mirror(std::span<const std::span<const T>> src_rows,
                           std::span<std::span<T>>             dst_rows,
                           std::ptrdiff_t x0, std::ptrdiff_t y0,
                           std::ptrdiff_t W,  std::ptrdiff_t H,
                           std::ptrdiff_t Mx, std::ptrdiff_t My)
{
    // dst size should be (H + 2*My) x (W + 2*Mx)
    for (std::ptrdiff_t dy = -My; dy < H + My; ++dy) {
        auto &out_row = dst_rows[dy + My];
        for (std::ptrdiff_t dx = -Mx; dx < W + Mx; ++dx) {
            out_row[dx + Mx] = mirror_at_2d(src_rows, y0 + dy, x0 + dx);
        }
    }
}

// Subimage class implementation

template <typename T, size_t NDims>
TSubImage<T,NDims>::TSubImage() 
{}

template <typename T, size_t NDims>
TSubImage<T,NDims>::TSubImage(std::vector<size_t> const & position, std::vector<size_t> const & lengths, size_t _margin) :
	m_start(position),
	m_estart(position),
	m_length(lengths), 
	m_elength(lengths),
	m_margin(_margin)
{
	if (m_start.size()!=NDims)
		throw kipl::base::KiplException("TSubImage: start size does not match NDims",__FILE__,__LINE__);
	if (m_length.size()!=NDims)
		throw kipl::base::KiplException("TSubImage: length size does not match NDims",__FILE__,__LINE__);

	for (auto & l : m_length) 
	{
		if (l==0)
			throw kipl::base::KiplException("TSubImage: length contains zero",__FILE__,__LINE__);	
	}

	for (auto & l : m_elength) 
	{
		l+=2*m_margin;
	}

	for (auto & s : m_estart) 
	{
		if (m_margin<s)
			s-=m_margin;
		else
			s= 0L;
	}
}

template <typename T, size_t NDims>
TSubImage<T,NDims>::TSubImage(std::vector<size_t> const & position, size_t const lengths, size_t _margin) :
	m_start(position),
	m_estart(position),
	m_length(NDims,lengths),
	m_elength(NDims,lengths),
	m_margin(_margin)
{
	if (m_start.size()!=NDims)
		throw kipl::base::KiplException("TSubImage: start size does not match NDims",__FILE__,__LINE__);


	for (auto & l : m_length) 
	{
		if (l==0)
			throw kipl::base::KiplException("TSubImage: length contains zero",__FILE__,__LINE__);	
	}

	for (auto & l : m_elength) 
	{
		l+=2*m_margin;
	}

	for (auto & s : m_estart) 
	{
		if (m_margin<s)
			s-=m_margin;
		else
			s= 0L;
	}
}

template <typename T, size_t NDims>
kipl::base::TImage<T,NDims> TSubImage<T,NDims>::extract(const kipl::base::TImage<T,NDims> & src) const
{
	kipl::base::TImage<T,NDims> subImg(m_elength);
	this->extract(src, subImg);

	return subImg;
}

template <typename T, size_t NDims>
void TSubImage<T,NDims>::extract(const kipl::base::TImage<T,NDims> & src, kipl::base::TImage<T,NDims> & dst) const
{
	if (m_start.empty())
		throw kipl::base::KiplException("TSubImage is not initialized. Only static methods allowed.",__FILE__,__LINE__);

	// if (src.Size(0)<=m_start[0]+m_length[0]+m_margin)
	// 	throw kipl::base::KiplException("TSubImage::extract: X-dim out of range",__FILE__,__LINE__);
	// if (src.Size(1)<=m_start[1]+m_length[1]+m_margin)
	// 	throw kipl::base::KiplException("TSubImage::extract: Y-dim out of range",__FILE__,__LINE__);
	// if (NDims==3) {
	// 	if (src.Size(2)<=m_start[2]+m_length[2]+m_margin)
	// 		throw kipl::base::KiplException("TSubImage::extract: Z-dim out of range",__FILE__,__LINE__);
	// }

	for (size_t i=0; i<NDims; ++i) {
		if (dst.Size(i)!=m_elength[i])
			throw kipl::base::KiplException("TSubImage::extract: destination image size does not match",__FILE__,__LINE__);
	}

	if constexpr (NDims == 2) {
		if (m_start[0]<m_margin || m_start[1]<m_margin || src.Size(0)<m_start[0]+m_length[0]+m_margin || src.Size(1)<m_start[1]+m_length[1]+m_margin)
		{ // Handle border cases
			// Build spans of source rows
			std::vector<std::span<const T>> src_rows;
			src_rows.reserve(src.Size(1));
			for (size_t y = 0; y < src.Size(1); ++y) {
				src_rows.emplace_back(std::span<const T>(src.GetLinePtr(y, 0), src.Size(0)));
			}

			// Build spans of destination rows (dst expected size: (H+2*My) x (W+2*Mx))
			std::vector<std::span<T>> dst_rows;
			dst_rows.reserve(dst.Size(1));
			for (size_t y = 0; y < dst.Size(1); ++y) {
				dst_rows.emplace_back(std::span<T>(dst.GetLinePtr(y, 0), dst.Size(0)));
			}

			copy_rect_with_mirror<T>(std::span<const std::span<const T>>(src_rows),
									std::span<std::span<T>>(dst_rows),
									m_start[0], m_start[1], m_length[0], m_length[1], m_margin, m_margin);

		} else { // Fast copy when no border issues
			for (size_t y=0; y<m_elength[1]; y++) {
				auto pSrc = src.GetLinePtr(y+m_estart[1],0)+m_estart[0];
				auto pDst = dst.GetLinePtr(y,0);
				std::copy_n(pSrc, m_elength[0], pDst);
			}
		}
    } else if constexpr (NDims == 3) {
        // Mirror boundary handling for 3D extraction
        for (size_t z = 0; z < m_elength[2]; ++z) {
            std::ptrdiff_t src_z = static_cast<std::ptrdiff_t>(z) + static_cast<std::ptrdiff_t>(m_estart[2]);
            src_z = reflect_index(src_z, src.Size(2));
            for (size_t y = 0; y < m_elength[1]; ++y) {
                std::ptrdiff_t src_y = static_cast<std::ptrdiff_t>(y) + static_cast<std::ptrdiff_t>(m_estart[1]);
                src_y = reflect_index(src_y, src.Size(1));
                auto pDst = dst.GetLinePtr(y, z);
                for (size_t x = 0; x < m_elength[0]; ++x) {
                    std::ptrdiff_t src_x = static_cast<std::ptrdiff_t>(x) + static_cast<std::ptrdiff_t>(m_estart[0]);
                    src_x = reflect_index(src_x, src.Size(0));
                    pDst[x] = src(src_x, src_y, src_z);
                }
            }
        }
    } else {
        static_assert(NDims == 2 || NDims == 3, "Only 2D and 3D supported");
    }
}

template <typename T, size_t NDims>
void TSubImage<T,NDims>::insert(const kipl::base::TImage<T,NDims> & subImg, kipl::base::TImage<T,NDims> & dest, bool includeMargin) const
{
	if (dest.Size(0)<m_start[0]+m_length[0])
		throw kipl::base::KiplException(std::format("TSubImage::insert: X-dim out of range ({})",m_start[0]+m_length[0]),__FILE__,__LINE__);
	if (dest.Size(1)<m_start[1]+m_length[1])
		throw kipl::base::KiplException(std::format("TSubImage::insert: Y-dim out of range ({})",m_start[1]+m_length[1]),__FILE__,__LINE__);
	if (NDims==3) {
		if (dest.Size(2)<m_start[2]+m_length[2])
			throw kipl::base::KiplException(std::format("TSubImage::insert: Z-dim out of range ({})",m_start[2]+m_length[2]),__FILE__,__LINE__);
	}

	if (subImg.Size(0)!=m_length[0]+2*m_margin || subImg.Size(1)!=m_length[1]+2*m_margin || (NDims==3 && subImg.Size(2)!=m_length[2]+2*m_margin) )
		throw kipl::base::KiplException("TSubImage::insert: subimage size does not match",__FILE__,__LINE__);

	size_t margin = includeMargin ? 0: m_margin;
	
	std::vector<size_t> insertLengths = m_length;
	for (auto & l : insertLengths) 
		l+=2*(m_margin-margin);
	if constexpr (NDims == 2) {
		for (size_t y=0; y<insertLengths[1]; y++) 
		{
			auto pSrc = subImg.GetLinePtr(y+margin)+margin;
			auto pDst = dest.GetLinePtr(y+m_start[1]-(m_margin-margin),0)+m_start[0]-(m_margin-margin);
			std::copy_n(pSrc, insertLengths[0], pDst);
		}
	} else if constexpr (NDims == 3) {
		for (size_t z=0; z<insertLengths[2]; ++z) 
		{
			for (size_t y=0; y<insertLengths[1]; y++) 
			{
				auto pSrc = subImg.GetLinePtr(y+margin,z+margin)+margin;
				auto pDst = dest.GetLinePtr(y+m_start[1],z+m_start[2])+m_start[0];
				std::copy_n(pSrc, insertLengths[0], pDst);
			}
		}
	} else {
		static_assert(NDims == 2 || NDims == 3, "Only 2D and 3D supported");
	}	
}

template <typename T, size_t NDims>
const std::vector<size_t> & TSubImage<T,NDims>::start() const
{
	return m_start;
}

template <typename T, size_t NDims>
const std::vector<size_t> & TSubImage<T,NDims>::estart() const
{
	return m_estart;
}

template <typename T, size_t NDims>
const std::vector<size_t> & TSubImage<T,NDims>::length() const
{
	return m_length;
}

template <typename T, size_t NDims>
const std::vector<size_t> & TSubImage<T,NDims>::elength() const
{
	return m_elength;
}

template <typename T, size_t NDims>
size_t TSubImage<T,NDims>::margin() const
{
	return m_margin;
}

template <typename T, size_t NDims>
TImage<T,NDims> TSubImage<T,NDims>::Get(TImage<T,NDims> const src, const std::vector<size_t> & nStart, const std::vector<size_t> & nLength)
{
    if (nStart.size()<NDims)
        throw kipl::base::KiplException("Too few start postions in Get subimage",__FILE__,__LINE__);
    if (nLength.size()<NDims)
        throw kipl::base::KiplException("Too few length postions in Get subimage",__FILE__,__LINE__);

    auto dims=src.dims();
	
	size_t startZ  = 0;
	size_t endZ    = 1;
	size_t startY  = 0;	
	size_t lengthY = 1;
	size_t startX  = nStart[0];	
	size_t sizeXY  = dims[0];
	size_t lengthX = nLength[0];
	
	switch (NDims) {
	case 2: 
		startY  = nStart[1];	
		lengthY = nLength[1];
				
		sizeXY  = dims[0]*dims[1];
		break;
	case 3:
		startZ  = nStart[2];	
		endZ    = startZ+nLength[2];
		startY  = nStart[1];	
		lengthY = nLength[1];
						
		sizeXY  = dims[0]*dims[1];
		break;
	}
		
	T * pSrc =const_cast<T*>( src.GetDataPtr());
	
	TImage<T,NDims> img(nLength);
	T* pImg = img.GetDataPtr();
	
	for (size_t z=startZ; z<endZ; z++) {
		pSrc+=z*sizeXY+startY*dims[0]+startX;
		for (size_t y=0; y<lengthY; y++) {
			pImg=img.GetLinePtr(y);
			memcpy(pImg,pSrc,lengthX*sizeof(T));
			pSrc+=dims[0];
		}
	}
			
	return img;
}

template <typename T, size_t NDims>
TImage<T,NDims> TSubImage<T,NDims>::Get(TImage<T,NDims> const src, const std::vector<size_t> &roi, bool includeCoord)
{
    if (roi.size()<2*NDims)
        throw kipl::base::KiplException("Too few postions in Get subimage",__FILE__,__LINE__);

    std::vector<size_t> dims(NDims);
    size_t plusOne=includeCoord ? 1 : 0;
    switch (NDims){
    case 1 :


        dims[0]=roi[1]-roi[0]+plusOne; break;
    case 2 :
        dims[0]=roi[2]-roi[0]+plusOne;
        dims[1]=roi[3]-roi[1]+plusOne;
        break;
    case 3 :
        dims[0]=roi[3]-roi[0]+plusOne;
        dims[1]=roi[4]-roi[1]+plusOne;
        dims[1]=roi[5]-roi[2]+plusOne;
        break;
    default:
        throw kipl::base::KiplException("nDim greater than 3 is not supported.",__FILE__,__LINE__);
        break;
    }

    kipl::base::TImage<T,NDims> img(dims);

    switch (NDims){
    case 1 :
        memcpy(img.GetDataPtr(),src.GetDataPtr()+roi[0],dims[0]*sizeof(T)); break;
    case 2 :
        for (size_t i=0; i<img.Size(1); i++ )
            memcpy(img.GetLinePtr(i),src.GetLinePtr(i+roi[1])+roi[0],sizeof(T)*img.Size(0));
        break;
    case 3 :
        for (size_t i=0; i<img.Size(2); i++ )
            for (size_t j=0; j<img.Size(1); j++ )
                memcpy(img.GetLinePtr(j,i),src.GetLinePtr(i+roi[1],j+roi[2])+roi[0],sizeof(T)*img.Size(0));
        break;
    default:
        throw kipl::base::KiplException("nDim greater than 3 is not supported.",__FILE__,__LINE__);
        break;
    }

    return img;
}

template <typename T, size_t NDims>
void TSubImage<T,NDims>::Put(const TImage<T,NDims> src, TImage<T, NDims> dest, size_t const *const nStart)
{
	size_t startZ  = 0;
	size_t endZ    = 1;
	size_t startY  = 0;	
	size_t lengthY = 1;
	size_t startX  = nStart[0];	
	size_t sizeXY  = dest.Size(0);
	size_t lengthX = src.Size(0);
	
	switch (NDims) {
	case 2: 
		startY  = nStart[1];	
		lengthY = src.Size(1);
				
		sizeXY  = dest.Size(0)*dest.Size(1);
		break;
	case 3:
		startZ  = nStart[2];	
		endZ    = startZ+src.Size(2);
		startY  = nStart[1];	
		lengthY = src.Size(1);
						
		sizeXY  = dest.Size(0)*dest.Size(1);
		break;
	}
		
	T * pSrc  = const_cast<T*>( src.GetDataPtr());
	
	for (size_t z=startZ; z<endZ; z++) {
		T * pDest = dest.GetLinePtr(0,z);
		pDest+=z*sizeXY+startY*dest.Size(0)+startX;
		
		for (size_t y=0; y<lengthY; y++) {
			memcpy(pDest, pSrc, lengthX*sizeof(T));
			pSrc+=src.Size(0);
			pDest+=dest.Size(0);
		}
	}
	
}

// ImagePatchExtractor class implementation
template <typename T, size_t NDims>	
ImagePatchExtractor<T, NDims>::ImagePatchExtractor(std::vector<size_t> const & imageDims, std::vector<size_t> const & subImageDims, size_t margin) :
	m_imageDims(imageDims),
	m_subImageDims(subImageDims),
	m_margin(margin)
{
	if constexpr (NDims != 2) {
		throw kipl::base::KiplException("ImagePatchExtractor: only 2D supported", __FILE__, __LINE__);
	}

	if (m_imageDims.size() != subImageDims.size())
		throw kipl::base::KiplException("ImagePatchExtractor: imageDims size != subImageDims size", __FILE__, __LINE__);
	
	for (size_t i=0; i<m_imageDims.size(); ++i) {
		if (m_subImageDims[i] == 0)
			throw kipl::base::KiplException("ImagePatchExtractor: subImageDims contains zero", __FILE__, __LINE__);
		if (m_imageDims[i] == 0)
			throw kipl::base::KiplException("ImagePatchExtractor: imageDims contains zero", __FILE__, __LINE__);
		if (m_imageDims[i] < m_subImageDims[i])
			throw kipl::base::KiplException("ImagePatchExtractor: subImageDims larger than imageDims", __FILE__, __LINE__);
	}
	

	// Calculate number of patches in each dimension
	size_t nDims = m_subImageDims.size();
	m_gridDims.resize(nDims);
	m_gridRemainders.resize(nDims);
	for (size_t d = 0; d < nDims; ++d) {

		m_gridDims[d]       = m_imageDims[d] / m_subImageDims[d]; // Ceiling division
		m_gridRemainders[d] = m_imageDims[d] % m_subImageDims[d];
	}

	// Calculate total number of patches
	m_totalPatches = 1;
	for (auto n : m_gridDims) {
		m_totalPatches *= n;
	}
}

template <typename T, size_t NDims>
size_t ImagePatchExtractor<T, NDims>::size() const
{
	return m_totalPatches;
}

template <typename T, size_t NDims>
size_t ImagePatchExtractor<T, NDims>::size(size_t axis) const
{
	if (axis >= m_gridDims.size())
		throw kipl::base::KiplException("ImagePatchExtractor::size: axis out of range", __FILE__, __LINE__);
	return m_gridDims[axis];
}

template <typename T, size_t NDims>
std::vector< kipl::base::TSubImage<T,NDims> > ImagePatchExtractor<T, NDims>::getAllSubImages() const
{
	std::vector< kipl::base::TSubImage<T,NDims> > subImages;

	subImages.reserve(m_totalPatches);

	// Iterate over all patch indices
	std::vector<size_t> currentIndices(NDims, 0);
	for (size_t count = 0; count < m_totalPatches; ++count) {
		// Calculate start position for current patch
		size_t x = count % m_gridDims[0];
		size_t y = count / m_gridDims[0];

		auto [startPos, adjustedSubImageDims] = calculatePatchInfo(x, y);

		subImages.emplace_back(startPos, adjustedSubImageDims, m_margin);
	}

	return subImages;
}

template <typename T, size_t NDims>
std::tuple<std::vector<size_t>, std::vector<size_t> > ImagePatchExtractor<T, NDims>::calculatePatchInfo(size_t x, size_t y) const
{
	std::vector<size_t> startPos({x*m_subImageDims[0],y*m_subImageDims[1]});
	std::vector<size_t> adjustedSubImageDims = m_subImageDims;

	std::vector<size_t> pos = {x,y};

	for (size_t i=0; i<NDims; ++i) 
	{		
		if (m_gridRemainders[i]<m_gridDims[i]) 
		{
			if (pos[i]<m_gridRemainders[i] && m_gridDims[i]>1) 
			{
				adjustedSubImageDims[i]+=1;
				startPos[i]+=pos[i];
			}
			else
				startPos[i]+=m_gridRemainders[i];
		}
		else
		{
			size_t bias  = m_gridRemainders[i]/m_gridDims[i];
			size_t extra = m_gridRemainders[i]%m_gridDims[i];	
			if (pos[i]<extra) 
			{
				adjustedSubImageDims[i]+=bias+1;
				startPos[i]+=pos[i]*(bias+1);
			}
			else
			{
				adjustedSubImageDims[i]+=bias;
				startPos[i]+=(extra*(bias+1))+((pos[i]-extra)*bias);
			}
		}
		
	}
	
	return {startPos, adjustedSubImageDims};
}

template <typename T, size_t NDims>
kipl::base::TSubImage<T,NDims> ImagePatchExtractor<T, NDims>::operator[](size_t idx) const
{
	return this->at(idx);
}

template <typename T, size_t NDims>
kipl::base::TSubImage<T,NDims> ImagePatchExtractor<T, NDims>::at(size_t idx) const
{
	if (idx >= m_totalPatches)
		throw kipl::base::KiplException("ImagePatchExtractor::at: index out of range", __FILE__, __LINE__);

	size_t x = idx % m_gridDims[0];
	size_t y = idx / m_gridDims[0];

	auto [startPos, adjustedSubImageDims] = calculatePatchInfo(x, y);
	
	return kipl::base::TSubImage<T,NDims>(startPos, adjustedSubImageDims, m_margin);
}

template <typename T, size_t NDims>
kipl::base::TSubImage<T,NDims> ImagePatchExtractor<T, NDims>::operator()(size_t x,size_t y) const
{
	if constexpr (NDims != 2)
		throw kipl::base::KiplException("ImagePatchExtractor::operator(): only for 2D images", __FILE__, __LINE__);

	if (x >= m_gridDims[0] || y >= m_gridDims[1])
		throw kipl::base::KiplException("ImagePatchExtractor::operator(): index out of range", __FILE__, __LINE__);

	// Calculate start position for the patch
	auto [startPos, adjustedSubImageDims] = calculatePatchInfo(x, y);

	return kipl::base::TSubImage<T,NDims>(startPos, adjustedSubImageDims, m_margin);
}


template <typename T>
TImage<T,2> ExtractSlice(TImage<T,3> volume, size_t idx)
{
    if (volume.Size(2) <= idx)
        throw kipl::base::KiplException("ExtractSlice: Index out of range", __FILE__, __LINE__);

    TImage<T,2> slice(volume.dims());

    // copy whole slice
    std::copy_n(volume.GetLinePtr(0, idx), slice.Size(), slice.GetDataPtr());

    return slice;
}

template <typename T>
void InsertSlice(TImage<T,3> volume, TImage<T,2> slice, size_t idx)
{
    if (volume.Size(2) <= idx)
        throw kipl::base::KiplException("InsertSlice: Index out of range", __FILE__, __LINE__);
    if ((volume.Size(0) != slice.Size(0)) || (volume.Size(1) != slice.Size(1)))
        throw kipl::base::KiplException("InsertSlice: Slice dims does not match volume xy-plane", __FILE__, __LINE__);

    // copy whole slice
    std::copy_n(slice.GetDataPtr(), slice.Size(), volume.GetLinePtr(0, idx));
}
}}

#endif /*TEXTRACTOR_HPP_*/

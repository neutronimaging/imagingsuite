//<LICENCE>

#ifndef TSUBIMAGE_H_
#define TSUBIMAGE_H_

#include <vector>
#include <tuple>

#include "timage.h"

namespace kipl { namespace base {

/// \brief The class offers different ways to extract sub images and put them back again.
template <typename T, size_t NDims>
class TSubImage {
public:
    TSubImage();
    TSubImage(std::vector<size_t> const & position, std::vector<size_t> const & lengths, size_t _margin);
    TSubImage(std::vector<size_t> const & position, size_t const lengths, size_t _margin);

    /// \brief Extracts the subimage from the source image
    /// \param src The image to extract the subimage from
    /// \return A subimage including margins as defined in the constructor
    kipl::base::TImage<T,NDims> extract(const kipl::base::TImage<T,NDims> & src) const;

    /// \brief Extracts the subimage from the source image into the provided destination image. This is more efficient as no new image is created.
    /// \param src The image to extract the subimage from
    /// \param dst The image to store the extracted subimage into
    void extract(const kipl::base::TImage<T,NDims> & src, kipl::base::TImage<T,NDims> & dst) const;

    /// \brief Insterts the subimage into the destination image
    /// \param subImg The subimage to insert
    /// \param dest The image to insert the subimage into
    /// \param includeMargin If true, the margin defined in the constructor is included in the insertion
    void insert(const kipl::base::TImage<T,NDims> & subImg, kipl::base::TImage<T,NDims> & dest, bool includeMargin=false) const;

    // Static methods

    /// \brief Gets a sub image using start position and width for each axis
    /// \param src The source image
    /// \param nStart array with starting positions for each image axis
    /// \param nLength array with subimage width for each image axis
    /// \returns the extracted image
    static TImage<T,NDims> Get(TImage<T,NDims> const src, const std::vector<size_t> & nStart, const std::vector<size_t> & nLength);

    /// \brief Gets a sub image using roi coordinates
    /// \param src The source image
    /// \param roi array containing coordinate tuples for the diagonal corners of the image
    /// \param includeCoord include the end coordinate (e.g. x1-x0+1) on true or (x1-x0) on false
    /// \returns the extracted image
    /// \test Unit test is available
    static TImage<T,NDims> Get(TImage<T,NDims> const src, const std::vector<size_t> & roi, bool includeCoord=true);

    /// \brief Puts a sub image back into a larger image
    /// \param src The sub image to put
    /// \param dest the larger image that will receive the sub image
    /// \param nStart tuple with the coordinate of the insertion corner
	static void Put(const TImage<T,NDims> src, TImage<T, NDims> dest, size_t const *const nStart);

    const std::vector<size_t> & start() const;
    const std::vector<size_t> & estart() const;
    const std::vector<size_t> & length() const;
    const std::vector<size_t> & elength() const;
    size_t margin() const;

private:
    std::vector<size_t> m_start;
    std::vector<size_t> m_estart;
    std::vector<size_t> m_length;
    std::vector<size_t> m_elength;

    size_t m_margin;
};

/// \brief Class to scan an image and produce subimage items
template <typename T, size_t NDims>
class ImagePatchExtractor {
public:
    ImagePatchExtractor(std::vector<size_t> const & imageDims, std::vector<size_t> const & subImageDims, size_t margin, bool useRemainders=true);
 
    size_t size() const;
    size_t size(size_t axis) const;
    std::vector< kipl::base::TSubImage<T,NDims> > getAllSubImages() const;

    kipl::base::TSubImage<T,NDims> operator[](size_t idx) const;
    kipl::base::TSubImage<T,NDims> at(size_t idx) const;
    kipl::base::TSubImage<T,NDims> operator()(size_t x,size_t y) const;

private:
    std::tuple<std::vector<size_t>, std::vector<size_t> > calculatePatchInfo(size_t x, size_t y) const;

    std::vector<size_t> m_imageDims;
    std::vector<size_t> m_subImageDims;
    size_t m_margin;

    std::vector<size_t> m_gridDims;
    std::vector<size_t> m_gridRemainders;
    size_t m_totalPatches;
};

/// \brief Extracts a 2D slice in the XY plane from a 3D image
/// \param volume the source image
/// \param idx the slice index along the Z-axis
/// \returns The extracted slice
template <typename T>
TImage<T,2> ExtractSlice(TImage<T,3> volume, size_t idx);

/// \brief Inserts a 2D slice into the XY plane of a 3D image
/// \param volume the source image
/// \param slice the image to insert in the slice plane
/// \param idx the slice index along the Z-axis
template <typename T>
void InsertSlice(TImage<T,3> volume, TImage<T,2> slice, size_t idx);
}}

#include "core/tsubimage.hpp"

#endif /*TEXTRACTOR_H_*/

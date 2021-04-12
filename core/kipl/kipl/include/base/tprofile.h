//<LICENCE>

#ifndef TPROFILE_H
#define TPROFILE_H

#include <vector>

namespace kipl { namespace base {

/// \brief Extracts a vertical projection from the image, ie the rows are summed
/// \param pData pointer to the image
/// \param dims array with image dimensions
/// \param pProfile array to contain the projection
/// \param bMeanProjection switch to select if the average projection is to be computed instead of the sum
/// \test TkiplbasetestTest::testProfiles() in tkiplbasetest.pro
template <typename T, typename S>
void VerticalProjection2D(const T *pData, const std::vector<size_t> &dims, S *pProfile, bool bMeanProjection=false);


/// \brief Extracts a vertical projection from the image, ie the rows are summed
/// \param pData pointer to the image
/// \param dims array with image dimensions
/// \param pProfile array to contain the projection
/// \param bMeanProjection switch to select if the average projection is to be computed instead of the sum
/// \test TkiplbasetestTest::testProfiles() in tkiplbasetest.pro
template <typename T>
std::vector<T> projection2D(const T *pData, const std::vector<size_t> &dims, int axis=0, bool bMeanProjection=false);

/// \brief Extracts a horizontal projection from the image, the columns are summed
/// \param pData pointer to the image
/// \param dims array with image dimensions
/// \param pProfile array to contain the projection
/// \param bMeanProjection switch to select if the average projection is to be computed instead of the sum
/// \test TkiplbasetestTest::testProfiles() in tkiplbasetest.pro
template <typename T, typename S>
void HorizontalProjection2D(const T *pData, const std::vector<size_t> &dims, S *pProfile, bool bMeanProjection=false);


}}

#include "core/tprofile.hpp"
#endif // TPROFILE_H



#ifndef __WSTHRES_H
#define __WSTHRES_H

#include <vector>

namespace akipl { namespace segmentation {

/// Computes thresholds using the watershed algorithm
/// The location of the watershed are used as threshold positions
///
/// \param hist vector containing the histogram
/// \param height relative height of the modes to be included. h$\in$[0,1] h=height*max(hist)
/// \param medianlength Length of median filter used to smooth the histogram
/// \param thvec Output vector containing the location of all watershed lines.
///
/// \returns If any thresholds have been found
int Threshold_Watershed(std::vector<size_t> &hist, double height, size_t medianlength, vector<size_t> &thvec);

}}

#endif

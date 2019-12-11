//<LICENSE>

#include <piercingpointestimator.h>

#include <algorithm>

#include <tnt_array1d.h>
#include <tnt_array2d.h>
#include <jama_lu.h>
#include <jama_qr.h>

#include <base/tprofile.h>
#include <base/tsubimage.h>
#include <base/textractor.h>
#include <math/tnt_utils.h>
#include <math/statistics.h>
#include <filters/filter.h>

#include <io/io_serializecontainers.h>
#include <io/io_tiff.h>


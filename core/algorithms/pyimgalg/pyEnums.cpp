//<LICENSE>
#include <algorithm>
#include <morphology/morphextrema.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <math/median.h>
#include <math/mathfunctions.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>

#include <sortspotclean.h>
#include <ImagingException.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindEnums(py::module &m)
{

py::enum_<kipl::base::eConnectivity>(m,"eConnectivity")
            .value("Connectivity4",              kipl::base::eConnectivity::conn4)
            .value("Connectivity8",              kipl::base::eConnectivity::conn8)
            .value("Connectivity6",              kipl::base::eConnectivity::conn6)
            .value("Connectivity18",             kipl::base::eConnectivity::conn18)
            .value("Connectivity26",             kipl::base::eConnectivity::conn26)
            .value("ConnectivityEuclid",         kipl::base::eConnectivity::euclid)
            .value("ConnectivityNone",           kipl::base::eConnectivity::none)
            .export_values();

}
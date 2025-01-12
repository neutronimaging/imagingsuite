//<LICENSE>

#include <sstream>
#include <iostream>

#include <buildfilelist.h>
#include <fileset.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindFileSet(py::module &m)
{
    py::class_<FileSet> fsClass(m, "FileSet");

    fsClass.def(py::init<>());
    fsClass.def(py::init<const FileSet &>());
//     //FileSet(const FileSet & cfg);
//     //const FileSet & operator=(const FileSet & cfg);

    fsClass.def("getId", &FileSet::getId);

// //    std::string WriteXML(int indent=4);
// //    int ParseXML(std::string xml);
// //    int ParseXML(xmlTextReaderPtr reader);
    fsClass.def("makeFileName", &FileSet::makeFileName);

    fsClass.def_readwrite("fileMask",     &FileSet::m_sFilemask);
    fsClass.def_readwrite("variableName", &FileSet::m_sVariableName);
    fsClass.def_readwrite("first",        &FileSet::m_nFirst);   //< Index number of the first file in the data set
    fsClass.def_readwrite("last",         &FileSet::m_nLast);    //< Index of the last file in the data set
    fsClass.def_readwrite("repeat",       &FileSet::m_nRepeat);  //< Number of repeated images for the same position
    fsClass.def_readwrite("stride",       &FileSet::m_nStride);  //< Index step to obtain the next observation in the series
    fsClass.def_readwrite("step",         &FileSet::m_nStep);    //< Sample every m_nStep images given the previous
    fsClass.def_readwrite("binning",      &FileSet::m_fBinning);

    fsClass.def_readwrite("flip",         &FileSet::m_Flip);
    fsClass.def_readwrite("rotate",       &FileSet::m_Rotate);
    fsClass.def_readwrite("useROI",       &FileSet::m_bUseROI);
    fsClass.def_readwrite("ROI",          &FileSet::m_ROI);

    fsClass.def_readwrite("skipList",     &FileSet::m_nSkipList); // list of indices of files to skip

    m.def("buildFileList", py::overload_cast<FileSet &>(&BuildFileList));
    // m.def("buildFileList", py::overload_cast<std::vector<FileSet> &>(&BuildFileList));
    // m.def("buildFileList", py::overload_cast<std::vector<FileSet> &, std::vector<int> &>(&BuildFileList));
    // m.def("buildProjectionFileList", py::overload_cast<std::vector<FileSet> &, int, int, double>(&BuildProjectionFileList));
    // m.def("buildProjectionFileList", py::overload_cast<std::vector<FileSet> &, std::vector<int> &, int, int, double>(&BuildProjectionFileList));
}

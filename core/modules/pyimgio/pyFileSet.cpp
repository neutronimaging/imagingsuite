//<LICENSE>

#include <sstream>
#include <iostream>

#include <buildfilelist.h>
#include <datasetbase.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void bindFileSet(py::module &m)
{
    py::class_<FileSet> fsClass(m, "FileSet");

    fsClass.def(py::init());

    //FileSet(const FileSet & cfg);
    //const FileSet & operator=(const FileSet & cfg);

    fsClass.def("getId", &FileSet::getId);

//    std::string WriteXML(int indent=4);
//    int ParseXML(std::string xml);
//    int ParseXML(xmlTextReaderPtr reader);
    fsClass.def("makeFileName", &FileSet::makeFileName);

    fsClass.def("fileMask",     &FileSet::m_sFilemask);
    fsClass.def("variableName", &FileSet::m_sVariableName);
    fsClass.def("first",        &FileSet::m_nFirst);   //< Index number of the first file in the data set
    fsClass.def("last",         &FileSet::m_nLast);    //< Index of the last file in the data set
    fsClass.def("repeat",       &FileSet::m_nRepeat);  //< Number of repeated images for the same position
    fsClass.def("stride",       &FileSet::m_nStride);  //< Index step to obtain the next observation in the series
    fsClass.def("step",         &FileSet::m_nStep);    //< Sample every m_nStep images given the previous
    fsClass.def("binning",      &FileSet::m_fBinning);

    fsClass.def("flip",         &FileSet::m_Flip);
    fsClass.def("rotate",       &FileSet::m_Rotate);
    fsClass.def("useROI",       &FileSet::m_bUseROI);
    fsClass.def("ROI",          &FileSet::m_ROI);

    fsClass.def("skipList",     &FileSet::m_nSkipList); // list of indices of files to skip

//    m.def("BuildFileList", &readers::GetFileExtensionType);


//std::list<std::string> BuildFileList(FileSet &il);
//std::list<std::string> BuildFileList(std::list<FileSet> &il);
//std::list<std::string> BuildFileList(std::list<FileSet> &il, std::list<int> &skiplist);
//std::map<float,std::string> BuildProjectionFileList(std::list<FileSet> &il, int sequence, int goldenStartIdx, double arc);
//std::map<float,std::string> BuildProjectionFileList(std::list<FileSet> &il, std::list<int> &skiplist, int sequence, int goldenStartIdx, double arc);
}

//<LICENSE>

#ifndef DATASETBASE_H
#define DATASETBASE_H

#include <string>
#include <list>
#include <iostream>
#include <string>
#include <libxml/xmlreader.h>

#include <base/kiplenums.h>
#include <base/timage.h>

#include "readerconfig_global.h"


class READERCONFIGSHARED_EXPORT FileSet
{
    static int cnt;
    int id;
public:
    FileSet();
    ~FileSet();
    FileSet(const FileSet & cfg);
    const FileSet & operator=(const FileSet & cfg);

    int getId();
    std::string WriteXML(int indent=4);
    int ParseXML(std::string xml);
    int ParseXML(xmlTextReaderPtr reader);
    std::string makeFileName(int idx) const ;

    std::string m_sFilemask;
    std::string m_sVariableName;
    int m_nFirst;   //< Index number of the first file in the data set
    int m_nLast;    //< Index of the last file in the data set
    int m_nRepeat;  //< Number of repeated images for the same position
    int m_nStride;  //< Index step to obtain the next observation in the series
    int m_nStep;    //< Sample every m_nStep images given the previous
    float m_fBinning;

    kipl::base::eImageFlip m_Flip;
    kipl::base::eImageRotate m_Rotate;

    bool m_bUseROI;
    size_t m_ROI[4];

    std::list<int> m_nSkipList; // list of indices of files to skip
};

std::ostream READERCONFIGSHARED_EXPORT & operator<<(std::ostream &s, FileSet &il);
#endif // DATASETBASE_H

//<LICENSE>

#ifndef DATASETBASE_H
#define DATASETBASE_H

#include <string>
#include <list>
#include <iostream>
#include <string>

#include <base/kiplenums.h>
#include <base/timage.h>

#include "readerconfig_global.h"


class READERCONFIGSHARED_EXPORT ImageLoader
{
    static int cnt;
    int id;
public:
    ImageLoader();
    ~ImageLoader();
    int getId();
    std::string WriteXML(int indent=4);
    int ParseXML(std::string xml);

    std::string m_sFilemask;
    std::string m_sVariableName;
    int m_nFirst;   //< Index number of the first file in the data set
    int m_nLast;    //< Index of the last file in the data set
    int m_nRepeat;  //< Number of repeated images for the same position
    int m_nStride;  //< Index step to obtain the next observation in the series
    int m_nStep;    //< Sample every m_nStep images given the previous

    kipl::base::eImageFlip m_Flip;
    kipl::base::eImageRotate m_Rotate;

    bool m_bUseROI;
    size_t m_ROI[4];

    std::list<int> m_nSkipList; // list of indices of files to skip
};

std::ostream READERCONFIGSHARED_EXPORT & operator<<(std::ostream &s, ImageLoader &il);
#endif // DATASETBASE_H

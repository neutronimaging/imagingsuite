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
public:
    ImageLoader();
    ~ImageLoader();
    std::string WriteXML(int indent=4);
    int ParseXML(std::string xml);

    kipl::base::TImage<float,2> Load(int idx, kipl::base::eImageRotate rot, kipl::base::eImageFlip flip, size_t *crop=nullptr);
    kipl::base::TImage<float,3> Load(kipl::base::eImageRotate rot,kipl::base::eImageFlip flip, size_t *crop=nullptr);
    std::string m_sFilemask;
    int m_nFirst;
    int m_nLast;
    int m_nStep;
    int m_nStride;
    std::string m_sVariableName;

    std::list<int> m_nSkipList;
};

std::ostream & operator<<(std::ostream &s, ImageLoader &il);
#endif // DATASETBASE_H

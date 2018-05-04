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
#include <averageimage.h>


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

    // \brief Load a single 2D image given the parameters of the described data set
    // \param idx sequence index of the image to load
    // \param rot Rotation of the image if requested
    // \param flip Mirror the image as requested
    // \param crop
    kipl::base::TImage<float,2> Load(int idx,
                                     kipl::base::eImageRotate rot=kipl::base::ImageRotateNone,
                                     kipl::base::eImageFlip flip=kipl::base::ImageFlipNone,
                                     size_t *crop=nullptr);

    // \brief Load the volume given by the parameters of the described data set
    // \param rot Rotation of the image if requested
    // \param flip Mirror the image as requested
    // \param crop
    kipl::base::TImage<float,3> Load(kipl::base::eImageRotate rot=kipl::base::ImageRotateNone,
                                     kipl::base::eImageFlip flip=kipl::base::ImageFlipNone,
                                     size_t *crop=nullptr);

    std::string m_sFilemask;
    std::string m_sVariableName;
    int m_nFirst;   //< Index number of the first file in the data set
    int m_nLast;    //< Index of the last file in the data set
    int m_nRepeat;  //< Number of repeated images for the same position
    int m_nStride;  //< Index step to obtain the next observation in the series
    int m_nStep;    //< Sample every m_nStep images given the previous


    std::list<int> m_nSkipList; // list of indices of files to skip
};

std::ostream READERCONFIGSHARED_EXPORT & operator<<(std::ostream &s, ImageLoader &il);
#endif // DATASETBASE_H

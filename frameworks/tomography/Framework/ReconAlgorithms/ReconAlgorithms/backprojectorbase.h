#ifndef BACKPROJECTORBASE_H
#define BACKPROJECTORBASE_H

#include <string>
#include <list>

#include <logging/logger.h>
#include <base/timage.h>

class BackProjectorBase
{
protected:
    kipl::logging::Logger logger;
    std::string m_sName;

public:
    BackProjectorBase(std::string name = "BackProjectorBase");
    virtual ~BackProjectorBase();

    virtual int backproject(kipl::base::TImage<float,2> &proj, float center, std::list<float> & angles, kipl::base::TImage<float,2> &slice) = 0;
    virtual int backproject(kipl::base::TImage<float,3> &proj, float center, std::list<float> & angles, kipl::base::TImage<float,3> &slices) = 0;

    /// \brief Set the pixel size in mm
    /// \param size The new pixel size
    void setPixelSize(float size) {m_PixelSize=size;}

protected:
    int buildMask(float center, size_t *dims);

    float m_PixelSize; ///< \brief Size of the pixels in the projections

    list<pair<int, int> > m_Mask;
};

#endif // BACKPROJECTORBASE_H

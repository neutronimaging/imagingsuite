//<LICENCE>

#ifndef SINE2D_H_
#define SINE2D_H_

#include "../kipl_global.h"

#include "../base/timage.h"

namespace kipl { namespace generators { 

/// \brief The class creates different test patterns based on 2D sine signals.
class KIPLSHARED_EXPORT Sine2D
{
private:
	Sine2D() {}
public:
    /// \brief Concentric 2D sine oscillations
    /// \param dims the dimensions of the generated image
    /// \param fScale Scales the spatial frequency of the ripple pattern. fScale=1 produce a single sine oscillation the number of oscillations are proportional to fScale.
    /// \returns a test image with sine pattern
	static kipl::base::TImage<float,2> SineRings(size_t *dims, float fScale);

    /// \brief Concentric 2D sine oscillations with frequency increasing with the distance from the center.
    /// \todo The function currently behaves as SineRings. This must be fixed
    /// \param dims the dimensions of the generated image
    /// \param fScale Scales the spatial frequency of the ripple pattern. fScale=1 produce a single sine oscillation the number of oscillations are proportional to fScale.
    /// \returns A test image with Jaehne ring pattern
    static kipl::base::TImage<float,2> JaehneRings(size_t N, float fScale);
};

}}

#endif /*SINE2D_H_*/

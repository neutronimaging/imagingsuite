#ifndef SPOTGENERATOR_H
#define SPOTGENERATOR_H

#include "../base/timage.h"
#include "NoiseImage.h"

namespace kipl { namespace generators {
/// \brief A class to generate spot patterns with different amplitude and width
///
/// The class is still under construction
class SpotGenerator
{
public:
    /// \brief Selects the type of background for the spots.
    enum BackgroundTypes {
        BackgroundFlat,         ///< The spot pattern is drawn on constant intensity background.
        BackgroundGradientX,    ///< The spot pattern is drawn on a gradient that changes in the x-direction
        BackgroundGradientY,    ///< The spot pattern is drawn on a gradient that changes in the y-direction
        BackgroundGradientXY    ///< The spot pattern is drawn on a gradient that changes in both x and y directions
    };

    /// \brief Defines the shape of the spots
    enum SpotShapes {
        SpotSquare,     ///< The spot has the shape of a square
        SpotRound,      ///< The spot is round
        SpotGauss       ///< The spot has the shape of a Gaussian
    };

    /// \brief Selects spot insertion method
    enum InsertMethods {
        InsertReplace,  ///< Replace the current pixel vaules by the spot
        InsertAdd,      ///< Add the spot intensity to the pixel value
        InsertMultiply  ///< Multiply the spot intensity by the pixel value
    };

    SpotGenerator();
    ~SpotGenerator();

    /// \brief Creates a test image with row and columns of various intensity and width
    /// \param dims The dimensions of the image
    /// \param width list of spot widths
    /// \param nWidths number of widths in the list
    /// \param amplitudes list of spot amplitudes
    /// \param nAmplitudes number of amplitudes in the list
    kipl::base::TImage<float,2> SpotLines(size_t *dims,
                              float *width, int nWidths,
                              float *amplitudes, int nAmplitudes,
                              SpotShapes ss=SpotSquare,
                              InsertMethods ins=InsertReplace);


    void SetBackground(BackgroundTypes bg=BackgroundFlat,
                       kipl::generators::NoiseGenerator &ng);
private:
    kipl::base::TImage<float,2> m_Img;
    BackgroundTypes m_Background;
    kipl::generators::NoiseGenerator n_NoiseGenerator;

    void InsertByReplace(kipl::base::TImage<float,2> &spot, int x, int y);
    void InsertByAdd(kipl::base::TImage<float,2> &spot, int x, int y);
    void InsertByMultiply(kipl::base::TImage<float,2> &spot, int x, int y);
};

#endif // SPOTGENERATOR_H

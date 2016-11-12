#ifndef PIXELITERATOR_H
#define PIXELITERATOR_H

#include "../kipl_global.h"

#include "../base/kiplenums.h"

namespace kipl {
namespace base {

class KIPLSHARED_EXPORT PixelIterator
{
public:
    /// \brief Initializes the iterator
    PixelIterator(size_t *dims, kipl::base::eConnectivity conn=kipl::base::conn8);

    /// \brief Copy constructor
    /// \param pi the iterator to copy
    PixelIterator(const PixelIterator &pi);

    /// \brief Assignment
    /// \param pi the iterator instance to copy
    PixelIterator & operator=(const PixelIterator &pi);

    /// \brief Increments the current position by one step
    ///
    /// Exceeding an edge changes to the first postion of the next row.
    PixelIterator & operator++();

    /// \brief Increments the current position by one step
    ///
    /// Exceeding an edge changes to the last postion of the next row.
    PixelIterator & operator--();

    /// \brief Sets the iterator position to the beginning
    ptrdiff_t begin();

    /// \brief Returns the end position (first position outside the image)
    ptrdiff_t end();

    /// \brief Assigns a new position to the iterator
    ptrdiff_t operator=(ptrdiff_t pos);

    bool operator==(ptrdiff_t pos);

    bool operator!=(ptrdiff_t pos);


    /// \brief Moves the pixel to an x,y position and gets the indexed neighborhood pixel
    /// \param x,y the coordinate of the position
    ptrdiff_t setPosition(int x, int y);

    /// \brief Moves the pixel to a position and gets the indexed neighborhood pixel
    /// \param pos the data array index for the new position
    ptrdiff_t setPosition(ptrdiff_t pos);

    /// \brief returns the positions of the neighborhood pixels
    /// \param idx index of the neighbor pixel
    ptrdiff_t neighborhood(int idx);

    /// \brief Returns the size of the neighborhood with the current connectivity
    int neighborhoodSize();

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    ptrdiff_t forwardNeigborhood(int idx);

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    ptrdiff_t backwardNeigborhood(int idx);

    /// \brief Returns the size of the forward eighborhood with the current connectivity
    int forwardSize();
    /// \brief Returns the size of the backward neighborhood with the current connectivity
    int backwardSize();

    /// \brief Returns the pixel index of the current pixel position
    ptrdiff_t getCurrentPosition() ;

    /// \brief Returns the connectivity used to investigate the neighborhood.
    kipl::base::eConnectivity getConnectivity();

    /// \brief determines of current position is on an edge
    kipl::base::eEdgeStatus getEdgeStatus();

protected:
    void setupNeighborhoods();
    kipl::base::eEdgeStatus updateEdgeStatus();

    int m_dims[2];
    ptrdiff_t m_imageSize;
    kipl::base::eConnectivity m_connectivity;
    ptrdiff_t m_neighborhoodIndex[8];
    ptrdiff_t m_edgeX[8];
    ptrdiff_t m_edgeY[8];
    ptrdiff_t m_corner0[8];
    ptrdiff_t m_corner1[8];
    ptrdiff_t m_forward[8];
    ptrdiff_t m_backward[8];

    struct neighborhoodsizes {
        neighborhoodsizes() :
            full(0),
            edgeX0(0),
            edgeX1(0),
            edgeY0(0),
            edgeY1(0),
            cornerX0Y0(0),
            cornerX1Y0(0),
            cornerX0Y1(0),
            cornerX1Y1(0)
            {}
        int full;
        int edgeX0;
        int edgeX1;
        int edgeY0;
        int edgeY1;
        int cornerX0Y0;
        int cornerX1Y0;
        int cornerX0Y1;
        int cornerX1Y1;
    };

    neighborhoodsizes m_neighborhoodSize;
    neighborhoodsizes m_fwbwSize;


    ptrdiff_t m_currentPosition;
    kipl::base::eEdgeStatus m_edgeStatus;

    int m_currentX;
    int m_currentY;

    int m_sx;
    int m_rowStart;
    int m_rowEnd;
};

}
}
#endif // PIXELITERATOR_H

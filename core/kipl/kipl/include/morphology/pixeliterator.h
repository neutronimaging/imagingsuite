

#ifndef PIXELITERATOR_H
#define PIXELITERATOR_H

#include "../kipl_global.h"

#include <stddef.h>
#include <vector>
#include "../base/kiplenums.h"
#include "../logging/logger.h"

namespace kipl {
namespace base {

class KIPLSHARED_EXPORT PixelIterator
{
    kipl::logging::Logger logger;
public:
    /// \brief Initializes the iterator
    PixelIterator(const std::vector<size_t> & dims, kipl::base::eConnectivity conn=kipl::base::conn8);

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

    /// \brief Moves the pixel to an x,y position and gets the indexed neighborhood pixel
    /// \param x,y,z the coordinate of the position
    ptrdiff_t setPosition(int x, int y, int z);

    /// \brief Moves the pixel to a position and gets the indexed neighborhood pixel
    /// \param pos the data array index for the new position
    ptrdiff_t setPosition(ptrdiff_t pos);

    /// \brief returns the positions of the neighborhood pixels
    /// \param idx index of the neighbor pixel
    const std::vector<ptrdiff_t> & neighborhood();
    /// \brief returns the positions of the neighborhood pixels
    /// \param idx index of the neighbor pixel
    ptrdiff_t neighborhood(size_t idx);

    /// \brief Returns the size of the neighborhood with the current connectivity
    size_t neighborhoodSize();

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    const std::vector<ptrdiff_t> & forwardNeighborhood();

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    const std::vector<ptrdiff_t> & backwardNeighborhood();

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    ptrdiff_t forwardNeighborhood(size_t idx);

    /// \brief returns the forward neighborhood of the pixel
    /// \param idx index of the neighbor pixel
    ptrdiff_t backwardNeighborhood(size_t idx);

    /// \brief Returns the size of the forward or backward neighborhood with the current connectivity
    size_t forwardSize();

    size_t backwardSize();

    /// \brief Returns the pixel index of the current pixel position
    ptrdiff_t currentPosition() ;

    /// \brief Returns the x coordinate of the current pixel position
    int currentX() ;

    /// \brief Returns the y coordinate of the current pixel position
    int currentY() ;

    /// \brief Returns the z coordinate of the current pixel position
    int currentZ();

    /// \brief Returns the connectivity used to investigate the neighborhood.
    kipl::base::eConnectivity connectivity();

    /// \brief determines of current position is on an edge
    kipl::base::eEdgeStatus edgeStatus();

    int supportedDims();

    std::string reportStatus();
protected:
    void setupNeighborhoods();
    void setupConn4();
    void setupConn8();
    void setupConn6();
    void setupConn18();
    void setupConn26();

    kipl::base::eEdgeStatus updateEdgeStatus();

    std::vector<ptrdiff_t> m_dims;
    ptrdiff_t m_imageSize;
    int m_ndims;
    ptrdiff_t m_sx;
    ptrdiff_t m_sxy;

    kipl::base::eConnectivity m_connectivity;

    // Full neighborhoods
    std::vector<ptrdiff_t> m_neighborhoodIndex;
    std::vector<ptrdiff_t> m_edgeX0;
    std::vector<ptrdiff_t> m_edgeX1;
    std::vector<ptrdiff_t> m_edgeY0;
    std::vector<ptrdiff_t> m_edgeY1;
    std::vector<ptrdiff_t> m_edgeZ0;
    std::vector<ptrdiff_t> m_edgeZ1;

    // corners are indexed with xyz
    std::vector<ptrdiff_t> m_cornerX0Y0;
    std::vector<ptrdiff_t> m_cornerX1Y0;
    std::vector<ptrdiff_t> m_cornerX0Y1;
    std::vector<ptrdiff_t> m_cornerX1Y1;
    std::vector<ptrdiff_t> m_cornerX0Z0;
    std::vector<ptrdiff_t> m_cornerX0Z1;
    std::vector<ptrdiff_t> m_cornerX1Z0;
    std::vector<ptrdiff_t> m_cornerX1Z1;
    std::vector<ptrdiff_t> m_cornerY0Z0;
    std::vector<ptrdiff_t> m_cornerY0Z1;
    std::vector<ptrdiff_t> m_cornerY1Z0;
    std::vector<ptrdiff_t> m_cornerY1Z1;

    std::vector<ptrdiff_t> m_cornerX0Y0Z0;
    std::vector<ptrdiff_t> m_cornerX1Y0Z0;
    std::vector<ptrdiff_t> m_cornerX0Y1Z0;
    std::vector<ptrdiff_t> m_cornerX1Y1Z0;
    std::vector<ptrdiff_t> m_cornerX0Y0Z1;
    std::vector<ptrdiff_t> m_cornerX0Y1Z1;
    std::vector<ptrdiff_t> m_cornerX1Y0Z1;
    std::vector<ptrdiff_t> m_cornerX1Y1Z1;

    // Forward neighborhood
    std::vector<ptrdiff_t> m_forward;
    std::vector<ptrdiff_t> m_fedgeX0;
    std::vector<ptrdiff_t> m_fedgeX1;
    std::vector<ptrdiff_t> m_fedgeY0;
    std::vector<ptrdiff_t> m_fedgeY1;
    std::vector<ptrdiff_t> m_fedgeZ0;
    std::vector<ptrdiff_t> m_fedgeZ1;

    // corners are indexed with xyz
    std::vector<ptrdiff_t> m_fcornerX0Y0;
    std::vector<ptrdiff_t> m_fcornerX1Y0;
    std::vector<ptrdiff_t> m_fcornerX0Y1;
    std::vector<ptrdiff_t> m_fcornerX1Y1;
    std::vector<ptrdiff_t> m_fcornerX0Z0;
    std::vector<ptrdiff_t> m_fcornerX0Z1;
    std::vector<ptrdiff_t> m_fcornerX1Z0;
    std::vector<ptrdiff_t> m_fcornerX1Z1;
    std::vector<ptrdiff_t> m_fcornerY0Z0;
    std::vector<ptrdiff_t> m_fcornerY0Z1;
    std::vector<ptrdiff_t> m_fcornerY1Z0;
    std::vector<ptrdiff_t> m_fcornerY1Z1;

    std::vector<ptrdiff_t> m_fcornerX0Y0Z0;
    std::vector<ptrdiff_t> m_fcornerX1Y0Z0;
    std::vector<ptrdiff_t> m_fcornerX0Y1Z0;
    std::vector<ptrdiff_t> m_fcornerX1Y1Z0;
    std::vector<ptrdiff_t> m_fcornerX0Y0Z1;
    std::vector<ptrdiff_t> m_fcornerX0Y1Z1;
    std::vector<ptrdiff_t> m_fcornerX1Y0Z1;
    std::vector<ptrdiff_t> m_fcornerX1Y1Z1;

    // Backward neighborhood
    std::vector<ptrdiff_t> m_backward;
    std::vector<ptrdiff_t> m_bedgeX0;
    std::vector<ptrdiff_t> m_bedgeX1;
    std::vector<ptrdiff_t> m_bedgeY0;
    std::vector<ptrdiff_t> m_bedgeY1;
    std::vector<ptrdiff_t> m_bedgeZ0;
    std::vector<ptrdiff_t> m_bedgeZ1;

    // corners are indexed with xyz
    std::vector<ptrdiff_t> m_bcornerX0Y0;
    std::vector<ptrdiff_t> m_bcornerX1Y0;
    std::vector<ptrdiff_t> m_bcornerX0Y1;
    std::vector<ptrdiff_t> m_bcornerX1Y1;
    std::vector<ptrdiff_t> m_bcornerX0Z0;
    std::vector<ptrdiff_t> m_bcornerX0Z1;
    std::vector<ptrdiff_t> m_bcornerX1Z0;
    std::vector<ptrdiff_t> m_bcornerX1Z1;
    std::vector<ptrdiff_t> m_bcornerY0Z0;
    std::vector<ptrdiff_t> m_bcornerY0Z1;
    std::vector<ptrdiff_t> m_bcornerY1Z0;
    std::vector<ptrdiff_t> m_bcornerY1Z1;

    std::vector<ptrdiff_t> m_bcornerX0Y0Z0;
    std::vector<ptrdiff_t> m_bcornerX1Y0Z0;
    std::vector<ptrdiff_t> m_bcornerX0Y1Z0;
    std::vector<ptrdiff_t> m_bcornerX1Y1Z0;
    std::vector<ptrdiff_t> m_bcornerX0Y0Z1;
    std::vector<ptrdiff_t> m_bcornerX0Y1Z1;
    std::vector<ptrdiff_t> m_bcornerX1Y0Z1;
    std::vector<ptrdiff_t> m_bcornerX1Y1Z1;

    ptrdiff_t m_currentPosition;
    kipl::base::eEdgeStatus m_edgeStatus;

    ptrdiff_t m_currentX;
    ptrdiff_t m_currentY;
    ptrdiff_t m_currentZ;


    ptrdiff_t m_rowStart;
    ptrdiff_t m_rowEnd;
};

}
}
#endif // PIXELITERATOR_H

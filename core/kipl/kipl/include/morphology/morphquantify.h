

#ifndef __MORPHQUANTIFY_H
#define __MORPHQUANTIFY_H

#include "../base/timage.h"
#include "morphology.h"

namespace Morphology {
/** The purpose of the class is to Quantify an image in terms of stereology
	\todo Finish the implementation...
*/
class CQuantifyImage 
{
public:
	/** \brief Constructor that computes the quantification of the image
		\param img Input image
		\param conn Selects the connectivity
		\param mask Optional ROI mask
	*/
    CQuantifyImage(CImage<char,2> &img, MorphConnect conn=conn4, CImage<char,2> *mask=nullptr);
	
	/** \brief Sets the connectivity of the computations and recomputes the values
		\param conn The new connectivity
	*/
	int SetConnectivity(MorphConnect conn);
	
	/** \brief Compute the Euler number of the image
		\param normalize Normalize the Euler-number by the number of tested pixels

		\author H-J Vogel, Origninal core 
		\author Anders Kaestner, Adjustments
	*/
	double EulerNumber(bool normalize=true);

	double VolumeDensity();

	double SurfaceDensity();

	double LengthDensity();

	double Curvature();

private:
	int ProcessData(MorphConnect conn);
	int h[16];
	int m_N;
	MorphConnect m_Conn;
	double m_DeltaX;
	double m_DeltaY;
	int m_iVol;
};

}

#endif

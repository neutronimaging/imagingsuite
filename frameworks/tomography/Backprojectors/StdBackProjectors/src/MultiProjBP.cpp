//<LICENSE>

#include "../include/MultiProjBP.h"
#include "../include/ReconException.h"
#include <base/timage.h>
#include <base/tpermuteimage.h>
#include <math/mathconstants.h>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

#ifdef __aarch64__
    #pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wcast-align"
	#pragma clang diagnostic ignored "-Wpedantic"
	#pragma clang diagnostic ignored "-W#warnings"
		#include <sse2neon/sse2neon.h>
	#pragma clang diagnostic pop
#else
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

MultiProjectionBP::MultiProjectionBP(kipl::interactors::InteractionBase *interactor) :
	StdBackProjectorBase("Multi projection BP",StdBackProjectorBase::MatrixZXY, interactor)
{
    logger(kipl::logging::Logger::LogMessage,"C'tor MultiProjectionBP");
    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner"}),
                                       "MuhRec - a new tomography reconstructor",
                                       "Nuclear Instruments and Methods Section A",
                                       2011,
                                       651,
                                       1,
                                       "156-160",
                                       "10.1016/j.nima.2011.01.129"));
}

MultiProjectionBP::~MultiProjectionBP(void)
{
}

void MultiProjectionBP::BackProject()
{
	std::stringstream msg;
	const size_t SizeY         = mask.size();		 // The mask size is used since there may be less elements than the matrix size.
	const size_t SizeZ         = volume.Size(0)/4; // Already adjusted by a factor 4
	const size_t SizeV4		   = SizeV/4;
	const int SizeUm2	   	   = static_cast<int>(SizeU-2);
	const size_t NProjections  = nProjectionBufferSize;

	__m128 column[2048];
	
	// This back projection is made for pillars in z
	__m128 a,b;
	float fPosU=0.0f;
	if (mConfig.ProjectionInfo.bCorrectTilt) {
		msg.str("");
		msg<<"Tilting axis by "<<mConfig.ProjectionInfo.fTiltAngle<<" degrees at "<<mConfig.ProjectionInfo.roi[1];
		logger(kipl::logging::Logger::LogVerbose,msg.str());
		for (size_t y=1; y<=SizeY; y++) 
		{
			const size_t cfStartX = mask[y-1].first;
			const size_t cfStopX  = mask[y-1].second;

			for (size_t i=0; i<nProjCounter; i++) {
				fLocalStartU[i]=fStartU[i] + fCos[i]*y;
			}
			
			const float centeroffset = (mConfig.ProjectionInfo.roi[1]-mConfig.ProjectionInfo.fTiltPivotPosition)*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180);
			const float centerinc    = 4*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180); // The SSE requires increments of 4
			for (size_t x=cfStartX+1; x<=cfStopX; x++)					
			{
				memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));
			
				for (size_t i=0; i<nProjCounter; i++)
				{
					fPosU  = fLocalStartU[i]-fSin[i]*x-centeroffset;
					int nPosU=static_cast<int>(fPosU);				// Compute position
							
					if ((nPosU<0) || ((SizeUm2)<nPosU)) {
						continue;
					}
		
					__m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
					__m128 * ProjColumnB = ProjColumnA+SizeV4;
					
					__m128 sum;
					for (size_t z=0; z<SizeZ; z++)	// Back-project on z 
					{
						float fpz=fPosU-z*centerinc;
                        nPosU = static_cast<int>(fpz);	
                        const float interpB = abs(fpz-nPosU);			// Interpolation weight right
                        const float interpA = 1.0f-interpB;				// Interpolation weight left

                        __m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
                        __m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left

                        a   = _mm_mul_ps(ProjColumnA[z],w0_128);
                        b   = _mm_mul_ps(ProjColumnB[z],w1_128);
                        sum = _mm_add_ps(a,b);
                        column[z]=_mm_add_ps(column[z],sum);

                        fPosU-=centerinc; // <<<<< Is this needed?
						// const float interpB = abs(fPosU-nPosU-z*centerinc);			// Interpolation weight right
						// const float interpA = 1.0f-interpB;				// Interpolation weight left

						// __m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
						// __m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left

						// a=_mm_mul_ps(ProjColumnA[z],w0_128);
						// b=_mm_mul_ps(ProjColumnB[z],w1_128);
						// sum=_mm_add_ps(a,b);
						// column[z]=_mm_add_ps(column[z],sum);
					}
				}
				memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128));
			}
		}
	}
	else {
		for (size_t y=1; y<=SizeY; y++) 
		{
			const size_t cfStartX = mask[y-1].first;
			const size_t cfStopX  = mask[y-1].second;

			for (size_t i=0; i<nProjCounter; i++) {
				fLocalStartU[i]=fStartU[i] + fCos[i]*y;
			}

			for (size_t x=cfStartX+1; x<=cfStopX; x++)					
			{
				memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));
			
				for (size_t i=0; i<nProjCounter; i++)
				{
					fPosU  = fLocalStartU[i]-fSin[i]*x;
					int nPosU=static_cast<int>(fPosU);				// Compute position
							
					if ((nPosU<0) || ((SizeUm2)<nPosU)) {
						continue;
					}
		
					__m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
					//__m128 * ProjColumnB = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU+1, i));
					__m128 * ProjColumnB = ProjColumnA+SizeV4;
					
					const float interpB = abs(fPosU-nPosU);			    // Interpolation weight right
					const float interpA = 1.0f-interpB;				// Interpolation weight left

					__m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
					__m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left
					__m128 sum;
					for (size_t z=0; z<SizeZ; z++)	// Back-project on z 
					{
						a=_mm_mul_ps(ProjColumnA[z],w0_128);
						b=_mm_mul_ps(ProjColumnB[z],w1_128);
						sum=_mm_add_ps(a,b);
						column[z]=_mm_add_ps(column[z],sum);
					}
				}
				memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128));
			}
		}
	}

}


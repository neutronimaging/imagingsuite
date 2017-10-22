//
// This file is part of the recon library by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $Date: 2010-09-27 15:20:52 +0200 (Mo, 27 Sep 2010) $
// $Rev: 726 $
//

#include "../include/MultiProjBPparallel.h"
#include "../include/ReconException.h"
#include <base/timage.h>
#include <base/tpermuteimage.h>
#include <math/mathconstants.h>
#include <math/mathfunctions.h>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <emmintrin.h>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

MultiProjectionBPparallel::MultiProjectionBPparallel(kipl::interactors::InteractionBase *interactor) :
	StdBackProjectorBase("Multi projection BP parallel",StdBackProjectorBase::MatrixZXY, interactor)
{

}

MultiProjectionBPparallel::~MultiProjectionBPparallel(void)
{
}

 
void MultiProjectionBPparallel::BackProject()
{
	std::stringstream msg;
	const ptrdiff_t SizeY         = mask.size();		 // The mask size is used since there may be less elements than the matrix size.
	const size_t SizeZ         = volume.Size(0)/4; // Already adjusted by a factor 4
	const size_t SizeV4		   = projections.Size(0)/4;
	const int SizeUm2	   	   = static_cast<int>(SizeU-2);
	const size_t NProjections  = nProjectionBufferSize;
	
	// This back projection is made for pillars in z
	if (mConfig.ProjectionInfo.bCorrectTilt) {
		msg.str("");
		msg<<"Tilting axis by "<<mConfig.ProjectionInfo.fTiltAngle<<" degrees at "<<mConfig.ProjectionInfo.roi[1];
		logger(kipl::logging::Logger::LogVerbose,msg.str());

		ptrdiff_t y=0;
		#pragma omp parallel
		{
            __m128 column[2048];
			__m128 a,b;
			float fPosU=0.0f;
            float fLocalStartUp[1024];
            memset(fLocalStartUp,0,1024*sizeof(float));
			#pragma omp for
			for (y=1; y<=SizeY; y++)
			{
				const size_t cfStartX = mask[y-1].first;
				const size_t cfStopX  = mask[y-1].second;

				const float centeroffset = (mConfig.ProjectionInfo.roi[1]-mConfig.ProjectionInfo.fTiltPivotPosition)
												*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180);
				for (size_t i=0; i<nProjCounter; i++) {
					fLocalStartUp[i]=fStartU[i] + fCos[i]*y-centeroffset;
				}


				const float centerinc    = 4*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180); // The SSE requires increments of 4
				for (size_t x=cfStartX+1; x<=cfStopX; x++)
				{
					memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));

					for (size_t i=0; i<nProjCounter; i++)
					{
						fPosU  = fLocalStartUp[i]-fSin[i]*x;
						int nPosU=static_cast<int>(fPosU);				// Compute position


						if ((nPosU<0) || ((SizeUm2)<nPosU)) {
							continue;
						}

						fPosU-=nPosU;
						__m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
						__m128 * ProjColumnB = ProjColumnA+SizeV4;

						__m128 sum=_mm_set_ps1(0.0f);
						for (size_t z=0; z<SizeZ; z++)	// Back-project on z
						{
							const float interpB = abs(fPosU-z*centerinc);			// Interpolation weight right
							const float interpA = 1.0f-interpB;				// Interpolation weight left

							__m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
							__m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left

							a=_mm_mul_ps(ProjColumnA[z],w0_128);
							b=_mm_mul_ps(ProjColumnB[z],w1_128);
							sum=_mm_add_ps(a,b);
							column[z]=_mm_add_ps(column[z],sum);
							fPosU-=centerinc;
						}
					}
					memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128));
				}
			}
		}
	}
	else {
		ptrdiff_t y=0;
		#pragma omp parallel
		{
			__m128 column[2048];
			__m128 a,b;
			float fPosU=0.0f;
			float fLocalStartUp[1024];
			#pragma omp for
			for (y=1; y<=SizeY; y++)
			{
				const size_t cfStartX = mask[y-1].first;
				const size_t cfStopX  = mask[y-1].second;

				for (size_t i=0; i<nProjCounter; i++) {
					fLocalStartUp[i]=fStartU[i] + fCos[i]*y;
				}

				for (size_t x=cfStartX+1; x<=cfStopX; x++)
				{
					memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));

					for (size_t i=0; i<nProjCounter; i++)
					{
						fPosU  = fLocalStartUp[i]-fSin[i]*x;
						int nPosU=static_cast<int>(fPosU);				// Compute position

						if ((nPosU<0) || ((SizeUm2)<nPosU)) {
							continue;
						}

						__m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
						__m128 * ProjColumnB = ProjColumnA+SizeV4;

						const float interpB = abs(fPosU-nPosU);			    // Interpolation weight right
						const float interpA = 1.0f-interpB;				// Interpolation weight left

						__m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
						__m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left
						__m128 sum=_mm_set_ps1(0.0f);
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

}


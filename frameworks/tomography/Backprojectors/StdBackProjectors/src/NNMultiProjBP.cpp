//<LICENSE>

#include "../include/NNMultiProjBP.h"
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

NearestNeighborBP::NearestNeighborBP(kipl::interactors::InteractionBase *interactor) :
	StdBackProjectorBase("Nearest Neighbor BP",StdBackProjectorBase::MatrixZXY, interactor)
{
    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner"}),
                                       "MuhRec - a new tomography reconstructor",
                                       "Nuclear Instruments and Methods Section A",
                                       2011,
                                       651,
                                       1,
                                       "156-160",
                                       "10.1016/j.nima.2011.01.129"));
}

NearestNeighborBP::~NearestNeighborBP(void)
{
}

 
void NearestNeighborBP::BackProject()
{
	std::stringstream msg;
	const size_t SizeY         = mask.size();		 // The mask size is used since there may be less elements than the matrix size.
	const size_t SizeZ         = volume.Size(0)>>2; // Already adjusted by a factor 4
	const size_t SizeV4		   = SizeV>>2;
	const int SizeUm2	   	   = static_cast<int>(SizeU-2);
	const size_t NProjections=nProjectionBufferSize;
	__m128 column[2048];
	
	// This back projection is made for pillars in z
	float fPosU=0.0f;
	if (mConfig.ProjectionInfo.bCorrectTilt) {
		msg.str("");
		msg<<"Tilting axis by "<<mConfig.ProjectionInfo.fTiltAngle<<" degrees.";
		logger(kipl::logging::Logger::LogVerbose,msg.str());
		for (size_t y=1; y<=SizeY; y++) 
		{
			const size_t cfStartX = mask[y-1].first;
			const size_t cfStopX  = mask[y-1].second;

			for (size_t i=0; i<NProjections; i++) {
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
					
					for (size_t z=0; z<SizeZ; z++)	// Back-project on z 
					{
						column[z]=_mm_add_ps(column[z],ProjColumnA[z]);
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

			for (size_t i=0; i<NProjections; i++) {
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
					for (size_t z=0; z<SizeZ; z++)	// Back-project on z 
					{
						column[z]=_mm_add_ps(column[z],ProjColumnA[z]);
					}
				}
				memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128));
			}
		}
	}
}


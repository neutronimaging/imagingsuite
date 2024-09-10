//<LICENSE>

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

#include <cmath>
#include <functional>
#include <thread>
#ifdef _OPENMP
#include <omp.h>
#endif

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

MultiProjectionBPparallel::MultiProjectionBPparallel(kipl::interactors::InteractionBase *interactor) :
	StdBackProjectorBase("Multi projection BP parallel",StdBackProjectorBase::MatrixZXY, interactor)
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

MultiProjectionBPparallel::~MultiProjectionBPparallel(void)
{
}

 
void MultiProjectionBPparallel::BackProject()
{
#if defined (__APPLE__)
    BackProjectSTL();
#else
    switch (mConfig.System.eThreadMethod)
    {
        case kipl::base::threadingSTL :    BackProjectSTL();    break;
        case kipl::base::threadingOpenMP : BackProjectOpenMP(); break;
    }
#endif

}

void MultiProjectionBPparallel::BackProjectOpenMP()
{
    std::stringstream msg;
    const ptrdiff_t SizeY      = mask.size();	   // The mask size is used since there may be less elements per row than the matrix size.
    const size_t SizeZ         = volume.Size(0)/4; // Already adjusted to be a multiple of 4
    const size_t SizeV4		   = projections.Size(0)/4;
    const int SizeUm2	   	   = static_cast<int>(SizeU-2);

#if defined (OMP)
    omp_set_dynamic(0);
    omp_set_number_of_threads(nMaxThreads);
#endif

    // This back projection is made for pillars in z
    if ( mConfig.ProjectionInfo.bCorrectTilt )
    {
        msg.str("");
        msg<<"Tilting axis by "<<mConfig.ProjectionInfo.fTiltAngle<<" degrees at "<<mConfig.ProjectionInfo.roi[1];
        logger(kipl::logging::Logger::LogVerbose,msg.str());

        ptrdiff_t y=0;
        #pragma omp parallel
        {
            const size_t SizeV4		   = projections.Size(0)/4;
            const int SizeUm2	   	   = static_cast<int>(SizeU-2);
	
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
                for (size_t i=0; i<nProjCounter; i++)
                {
                    fLocalStartUp[i] = fStartU[i] + fCos[i]*y-centeroffset;
                }


                const float centerinc    = 4*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180); // The SSE requires increments of 4
                for (size_t x=cfStartX+1; x<=cfStopX; x++)
                {
                    memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));

                    for (size_t i=0; i<nProjCounter; i++)
                    {
                        fPosU  = fLocalStartUp[i]-fSin[i]*x;
                        int nPosU=static_cast<int>(fPosU);				// Compute position


                        if ((nPosU<0) || ((SizeUm2)<nPosU))
                        {
                            continue;
                        }

                        fPosU-=nPosU;
                        __m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
                        __m128 * ProjColumnB = ProjColumnA+SizeV4;

                        __m128 sum=_mm_set_ps1(0.0f);
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
                            // const float interpB = abs(fPosU-z*centerinc);	// Interpolation weight right
                            // const float interpA = 1.0f-interpB;				// Interpolation weight left

                            // __m128 w0_128=_mm_set_ps1(interpA);				// Interpolation weight right
                            // __m128 w1_128=_mm_set_ps1(interpB);				// Interpolation weight left

                            // a=_mm_mul_ps(ProjColumnA[z],w0_128);
                            // b=_mm_mul_ps(ProjColumnB[z],w1_128);
                            // sum=_mm_add_ps(a,b);
                            // column[z]=_mm_add_ps(column[z],sum);
                            // fPosU-=centerinc;
                        }
                    }
                    memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128));
                }
            }
        }
    }
    else
    {
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

                for (size_t i=0; i<nProjCounter; i++)
                {
                    fLocalStartUp[i]=fStartU[i] + fCos[i]*y;
                }

                for (size_t x=cfStartX+1; x<=cfStopX; x++)
                {
                    memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128));

                    for (size_t i=0; i<nProjCounter; i++)
                    {
                        fPosU  = fLocalStartUp[i]-fSin[i]*x;
                        int nPosU=static_cast<int>(fPosU);				// Compute position

                        if ((nPosU<0) || ((SizeUm2)<nPosU))
                        {
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

void MultiProjectionBPparallel::BackProjectSTL()
{
    std::stringstream msg;

    msg.str("");
    msg<<"Tilting axis by "<<mConfig.ProjectionInfo.fTiltAngle<<" degrees at "<<mConfig.ProjectionInfo.roi[1];
    logger(kipl::logging::Logger::LogVerbose,msg.str());

    std::vector<std::thread> threads;

#ifdef LOADBALANCEDBLOCKS
    for (auto const & block : lineBlocks)
    {   // spawn threads
        threads.push_back(std::thread([=] { BackProjectSTL(block.first,block.second); }));
    }
#else
    const int SizeY      = mask.size();	   // The mask size is used since there may be less elements per row than the matrix size.

    int nThreads = std::min(nMaxThreads,SizeY);
    int blockSize  = SizeY / nThreads;
    int blockRest = SizeY % nThreads;

    int begin = 1;
    int end   = blockSize + (blockRest>0 ? 1 :0) ;

    for (int i = 0; i < nThreads; ++i)
    {   // spawn threads

        threads.push_back(std::thread([=] { BackProjectSTL(begin,end); }));

        --blockRest;
        begin  = end;
        end   += blockSize + (blockRest>0 ? 1 :0) ;
    }
#endif

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));


}

void MultiProjectionBPparallel::BackProjectSTL(int first, int last)
{
    const size_t SizeZ         = volume.Size(0)/4; // Already adjusted to be a multiple of 4
    const size_t SizeV4		   = projections.Size(0)/4;
    const int SizeUm2	   	   = static_cast<int>(SizeU-2);

    __m128 column[2048];
    __m128 a,b;
    float fPosU=0.0f;
    std::vector<float> fLocalStartUp(nProjCounter,0.0f);

    // This back projection is made for pillars in z
    if (mConfig.ProjectionInfo.bCorrectTilt)
    {
        const float centeroffset = (mConfig.ProjectionInfo.roi[1]-mConfig.ProjectionInfo.fTiltPivotPosition)
                                        *tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180);

        const float centerinc    = 4*tan(mConfig.ProjectionInfo.fTiltAngle*fPi/180); // The SSE requires increments of 4

        for (int y=first; y<last; y++)
        {
            const size_t cfStartX = mask[y-1].first;
            const size_t cfStopX  = mask[y-1].second;

            for (size_t i=0; i<nProjCounter; i++)
            {
                fLocalStartUp[i]=fStartU[i] + fCos[i]*y-centeroffset;
            }
            
            for (size_t x=cfStartX+1; x<=cfStopX; x++)
            {
                memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128)); // Must be memcpy due to different data types

                for (size_t i=0; i<nProjCounter; i++)
                {
                    fPosU  = fLocalStartUp[i]-fSin[i]*x;
                    int nPosU=static_cast<int>(fPosU);				// Compute position


                    if ((nPosU<0) || ((SizeUm2)<nPosU))
                    {
                        continue;
                    }

                    __m128 * ProjColumnA = reinterpret_cast<__m128 *>(projections.GetLinePtr(nPosU, i));
                    __m128 * ProjColumnB = ProjColumnA+SizeV4;

                    __m128 sum=_mm_set_ps1(0.0f);
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
                    }
                }
                memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128)); // Must be memcpy due to different data types
            }
        }
    }
    else
    {
        for (int y=first; y<last; y++)
        {
            const size_t cfStartX = mask[y-1].first;
            const size_t cfStopX  = mask[y-1].second;

            for (size_t i=0; i<nProjCounter; i++)
            {
                fLocalStartUp[i]=fStartU[i] + fCos[i]*y;
            }

            for (size_t x=cfStartX+1; x<=cfStopX; x++)
            {
                memcpy(column,volume.GetLinePtr(x-1,y-1),SizeZ*sizeof(__m128)); // Must be memcpy due to different data types

                for (size_t i=0; i<nProjCounter; i++)
                {
                    fPosU  = fLocalStartUp[i]-fSin[i]*x;
                    int nPosU=static_cast<int>(fPosU);				// Compute position

                    if ((nPosU<0) || ((SizeUm2)<nPosU))
                    {
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
                memcpy(volume.GetLinePtr(x-1,y-1),column,SizeZ*sizeof(__m128)); // Must be memcpy due to different data types

            }
        }
    }
}


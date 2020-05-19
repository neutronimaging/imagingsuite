//<LICENCE>

#ifndef MORPHDIST_HPP_
#define MORPHDIST_HPP_

#include <sstream>
#include <cmath>

#include "../../logging/logger.h"
#include "../../base/KiplException.h"
#include "../../base/timage.h"
#include "../../base/kiplenums.h"
#include "../pixeliterator.h"
#include "../morphdist.h"
#include "../morphology.h"

namespace kipl { namespace morphology {

inline ptrdiff_t abs(ptrdiff_t x) { return x<0 ? -x : x;}

/// Computes the squared Euclidean distance for a displacement
/// \param x Displacement along the x-axis
/// \param y Displacement along the y-axis
/// \param z Displacement along the z-axis (only for 3D)
///
/// \returns The squared Euclidean distance
inline float eudist2(float x,float y, float z=0.0f) {return z ? x*x+y*y+z*z : x*x+y*y;}

/// Computes the Euclidean distance for a displacement
/// \param x Displacement along the x-axis
/// \param y Displacement along the y-axis
/// \param z Displacement along the z-axis (only for 3D)
///
/// \returns The Euclidean distance
inline float eudist(float x,float y, float z=0.0f) {return std::sqrt(eudist2(x,y,z));}

/// Computes the x displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_x(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0)
{
    ptrdiff_t d=p0%sx - p1%sx;
    if (d<0) d=-d;
    return d;
}

/// Computes the y displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_y(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0)
{
    ptrdiff_t d=0;
    if (sxy!=0)
        d=((p0%sxy)/sx - (p1%sxy)/sx);
    else
        d=((p0 - p1)/sx);

    return d<0 ? -d : d;
}

/// Computes the z displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_z(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0)
{
    ptrdiff_t d=p0/sxy - p1/sxy;
    return d<0 ? -d : d;
}

/// Computes the fast Euclidean distance between two neighbor pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
///
/// \returns the distance
inline float pixdist_euclid(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx)
{
    ptrdiff_t d=p0-p1;
    if (d<0) d=-d;

    if (d==0)
        return 0.0f;

    if ((d==1) || (d==sx))
        return 1.0f;


    if (abs(d-sx)==1)
        return sqrt(2.0f);

    return eudist(static_cast<float>(pixdist_x(p0,p1,sx)),
        static_cast<float>(pixdist_y(p0,p1,sx)));
}

/// \brief Computes the Euclidean distance map of 2D and 3D images
/// \param mask bw image containing the structures
/// \param dist resulting distance map
/// \param conn neighborhood connectivity
///
/// \bug Something locks for large distances/large images
template<class MaskType, class DistType, size_t NDim>
int EuclideanDistance(kipl::base::TImage<MaskType,NDim> &mask,
        kipl::base::TImage<DistType,NDim> &dist, bool complement,
        kipl::base::eConnectivity conn)
{
    std::stringstream msg;
    kipl::logging::Logger logger("EuclideanDistance");
//	logger(kipl::logging::Logger::LogWarning,"Warning, the algorithm is buggy...");
    const auto dims=mask.dims();
    const size_t sx=dims[0];
    const size_t sxy=dims[0]*dims[1];

    ptrdiff_t pos;
    size_t p,j;
    kipl::base::PixelIterator NG(dims,conn);

    const unsigned short max_dist=numeric_limits<unsigned short>::max();
    const MaskType inqueue=static_cast<MaskType>(2);

    // Initializing the priority queue
    deque<ptrdiff_t> fifoA,fifoB;
    deque<ptrdiff_t> *rFifoA=&fifoA;
    deque<ptrdiff_t> *rFifoB=&fifoB;

    MaskType *pMask=mask.GetDataPtr();
    if (complement)
        for (size_t i=0; i<mask.Size(); i++)
            pMask[i]=(pMask[i]==0);
    else
        for (size_t i=0; i<mask.Size(); i++)
            pMask[i]=(pMask[i]!=0);

    kipl::base::TImage<unsigned short,NDim> imx(dims), imy(dims), imz;
    imx=0;
    imy=0;
    unsigned short *pImx=imx.GetDataPtr();
    unsigned short *pImy=imy.GetDataPtr();
    unsigned short *pImz=nullptr;

    if (NDim==3) {
        imz.resize(dims);
        imz=0;
        pImz=imz.GetDataPtr();
    }

    size_t maskCnt=0;

    // Initialize queue
    NG.setPosition(0UL);
    for (ptrdiff_t i=0; i<mask.Size(); ++i)
    {
        if (pMask[i]!=0)
        {
            pImx[i]=max_dist;
            pImy[i]=max_dist;
            if (pImz)
                pImy[i]=max_dist;

            NG.setPosition(i);
            for (const auto & p : NG.neighborhood())
            {
                pos = p + i;
                if (pMask[pos]==static_cast<MaskType>(0))
                {
                    fifoA.push_back(i);
                    pMask[i]=inqueue;
                    break;
                }
            }
        }
        else
            maskCnt++;
    }

    float dmin, dp, current_dist;

    size_t itcnt=0;
    ptrdiff_t pdx,pdy,pdz;
    size_t procCnt=0, maxCnt=mask.Size()-maskCnt;

    while ((!rFifoA->empty()) && (procCnt<maxCnt))
    {
        dmin=numeric_limits<float>::max();

        for (const auto &p : *rFifoA)
        {
            if (pImx[p]!=max_dist)
            {// Distance has already been computed
                continue;
            }
            dp=numeric_limits<float>::max();
            NG.setPosition(p);
            if (NDim==2)
            {
                for (const auto &neighborPix : NG.neighborhood())
                {
                    pos = p + neighborPix;
                    if (pImx[pos]!=max_dist)
                    {
                        pdx=pixdist_x(pos,p,sx);
                        pdy=pixdist_y(pos,p,sx);
                        current_dist=eudist2(static_cast<float>(pImx[pos]+pdx),
                            static_cast<float>(pImy[pos]+pdy));

                        if (current_dist<dp) {
                            pImx[p]=pImx[pos]+pdx;
                            pImy[p]=pImy[pos]+pdy;
                            dp=current_dist;
                        }
                    }
                }
            }
            else
            {
                for (const auto & neighborPix : NG.neighborhood())
                {
                    pos= p + neighborPix;
                    if (pImx[pos]!=max_dist)
                    {
                        pdx=pImx[pos]+pixdist_x(pos,p,sx,sxy);
                        pdy=pImy[pos]+pixdist_y(pos,p,sx,sxy);
                        pdz=pImz[pos]+pixdist_z(pos,p,sx,sxy);
                        current_dist=eudist2(static_cast<float>(pdx),
                            static_cast<float>(pdy),
                            static_cast<float>(pdz));

                        if (current_dist<dp)
                        {
                            pImx[p]=pdx;
                            pImy[p]=pdy;
                            pImz[p]=pdz;
                            dp=current_dist;
                        }
                    }
                }
                if (dp<dmin)
                    dmin=dp;
            }
        }

        size_t propCnt=0;
        size_t retCnt=0;
        while (!rFifoA->empty())
        {
            p=rFifoA->front();
            rFifoA->pop_front();

            if ((NDim==2 ? eudist2(pImx[p],pImy[p]) : eudist2(pImx[p],pImy[p],pImz[p]))>dmin)
            {
                // Delay the propagation for
                // distances greater than dmin
                rFifoB->push_back(p);
                propCnt++;
            }
            else
            {
                procCnt++;
                retCnt++;
                NG.setPosition(p);
                for (auto const & neighborPix : NG.neighborhood())
                {
                    pos = p + neighborPix;
                    if (pMask[pos]==1)
                    {
                        rFifoB->push_back(pos);
                        pMask[pos]=inqueue;
                    }
                }
            }
        }

        if (fifoA.empty())
        {
            rFifoA=&fifoB;
            rFifoB=&fifoA;
        }
        else
        {
            rFifoA=&fifoA;
            rFifoB=&fifoB;
        }
        msg.str("");
        msg<<"Processed "<<procCnt<<" of "<<maxCnt;

        itcnt++;
    }
    //logger(kipl::logging::Logger::LogVerbose,"Calculating distances from coordinate fields");

    dist.resize(dims);
    DistType * pDist=dist.GetDataPtr();
    if (NDim==2)
        for (p=0; p<imx.Size(); p++)
                pDist[p]=eudist(pImx[p],pImy[p]);
    else
        for (p=0; p<imx.Size(); p++)
                pDist[p]=static_cast<DistType>(eudist(pImx[p],pImy[p],pImz[p]));

    return 0;
}

//CImage<double,2> DistanceTransform(CImage<char,2> img,MorphConnect connectivity);

/// \brief Computes the distance transform based a given metric
/// \param img input image, must be black and white
/// \param dist output image containing the distance map of img
/// \param metric an instance of the desired metric
/// \param complement Compute the distance map of the complementary image
template<class MaskType>
int DistanceTransform3D(kipl::base::TImage<MaskType,3> &img,
        kipl::base::TImage<float,3> &dist,
        kipl::morphology::CMetricBase &metric, bool complement)
{
    kipl::logging::Logger logger("DistanceTransform3D");

    std::ostringstream msg;
    const float object=10000.0f;
    if (metric.dim()!=3) {
        msg.str("");
        msg<<"The selected metric ("<<metric.getName()<<") does not have 3D support.";
        logger(kipl::logging::Logger::LogError,msg.str());

        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    msg.str("");
    msg<<"using metric "<<metric.getName();
    logger(kipl::logging::Logger::LogMessage,msg.str());

    auto dims=img.Dims();

    dist.resize(dims);

    MaskType *pImg;
    float *pDist;

    metric.initialize(dims);

    int start=metric.start();
    ptrdiff_t N=static_cast<size_t>(img.Size());

    pDist=dist.GetDataPtr();
    pImg=img.GetDataPtr();

    // Initialize
    if (complement) {
        for (ptrdiff_t i=0; i<N; i++)
            pDist[i]= pImg[i]!=static_cast<MaskType>(0) ? 0.0f: object;
    }
    else {
        for (ptrdiff_t i=0; i<N; i++)
            pDist[i]= pImg[i]==static_cast<MaskType>(0) ? 0.0f: object;
    }

    // Forward scan
    for (size_t z=start; z<dims[2]-start; z++) {
        for (size_t y=start; y<dims[1]-start ; y++) {
            pDist=dist.GetLinePtr(y,z);
            for (size_t x=start; x<dims[0]-start; x++) {
                if (pDist[x])
                    pDist[x]=metric.forward(pDist+x);
            }
        }
    }

    // Backward scan
    for (size_t z=dims[2]-1-start; start<=z; z--) {
        for (size_t y=dims[1]-1-start; start<=y ; y--) {
            pDist=dist.GetLinePtr(y,z);
            for (size_t x=dims[0]-1-start; start<=x;  x--) {
                if (pDist[x])
                    pDist[x]=metric.backward(pDist+x);
            }
        }
    }

    // Cleanup
    pDist=dist.GetDataPtr();

    for (ptrdiff_t i=0; i<N; i++) {
        if (pDist[i]==object)
            pDist[i]=0.0f;
    }

    return 0;
}

/// Computes a 2D distance map using different metrics
/// \param img mask image
/// \param dist Resulting distnace map
/// \param metric A metric object
template<typename ImgType, typename DistType>
int DistanceTransform2D(kipl::base::TImage<ImgType,2> &img,
        kipl::base::TImage<DistType,2> &dist, CMetricBase &metric)
{
    int x,y;
    size_t const * const dims=img.Dims();
    dist.Resize(dims);

    ImgType *pImg;
    DistType *pDist;


    metric.initialize(dims);
    int start=metric.start();
    dist=numeric_limits<DistType>::max();

    for (y=start; y<dims[1]-start; y++) {
        pDist=dist.GetLinePtr(y)+start;
        pImg=img.GetLinePtr(y)+start;
        for (x=start; x<dims[0]-start; x++,pDist++, pImg++) {
            *pDist= *pImg==0 ? 0: numeric_limits<DistType>::max();
        }
    }

    for (y=start; y<dims[1]-start; y++) {
        pDist=dist.GetLinePtr(y)+start;
        for (x=start; x<dims[0]-start; x++,pDist++) {
            if (*pDist)
                *pDist=(DistType)metric.forward(pDist);
        }
    }

    for (y=dims[1]-1-start; y>=start; y--) {
        pDist=dist.GetLinePtr(y)+dims[0]-1-start;
        for (x=dims[0]-1-start; x>=start; x--,pDist--) {
            if (*pDist)
                *pDist=(DistType)metric.backward(pDist);
        }
    }

    pDist=dist.GetDataPtr();
    for (x=0; x<dist.Size(); x++, pDist++)
        if (*pDist==numeric_limits<DistType>::max()) *pDist=(DistType)1;

    return 1;
}





}}

namespace kipl { namespace morphology { namespace old {

inline ptrdiff_t abs(ptrdiff_t x) { return x<0 ? -x : x;}

/// Computes the squared Euclidean distance for a displacement
/// \param x Displacement along the x-axis
/// \param y Displacement along the y-axis
/// \param z Displacement along the z-axis (only for 3D)
///
/// \returns The squared Euclidean distance
inline float eudist2(float x,float y, float z=0.0f) {return z ? x*x+y*y+z*z : x*x+y*y;}

/// Computes the Euclidean distance for a displacement
/// \param x Displacement along the x-axis
/// \param y Displacement along the y-axis
/// \param z Displacement along the z-axis (only for 3D)
///
/// \returns The Euclidean distance
inline float eudist(float x,float y, float z=0.0f) {return sqrt(eudist2(x,y,z));}

/// Computes the x displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_x(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0) 
{
	ptrdiff_t d=p0%sx - p1%sx;
	if (d<0) d=-d;
	return d;
}

/// Computes the y displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_y(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0) 
{
	ptrdiff_t d=0;
	if (sxy!=0)
		d=((p0%sxy)/sx - (p1%sxy)/sx);
	else
		d=((p0 - p1)/sx);

	return d<0 ? -d : d;
}

/// Computes the z displacement between two pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
/// \param sxy number of pixels in an xy-plane (only for 3D)
///
/// \returns the displacement
inline ptrdiff_t pixdist_z(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx, ptrdiff_t sxy=0) 
{
	ptrdiff_t d=p0/sxy - p1/sxy;
	return d<0 ? -d : d;
}

/// Computes the fast Euclidean distance between two neighbor pixels
/// \param p0 index of pixel p0
/// \param p1 index of pixel p1
/// \param sx dimension of the x-axis
///
/// \returns the distance
inline float pixdist_euclid(ptrdiff_t p0, ptrdiff_t p1, ptrdiff_t sx)
{
	ptrdiff_t d=p0-p1;
	if (d<0) d=-d;

	if (d==0)
		return 0.0f;

	if ((d==1) || (d==sx))
		return 1.0f;


	if (abs(d-sx)==1)
		return sqrt(2.0f);

	return eudist(static_cast<float>(pixdist_x(p0,p1,sx)),
		static_cast<float>(pixdist_y(p0,p1,sx)));
}

/// \brief Computes the Euclidean distance map of 2D and 3D images
/// \param mask bw image containing the structures
/// \param dist resulting distance map
/// \param conn neighborhood connectivity
///
/// \bug Something locks for large distances/large images
template<class MaskType, class DistType, size_t NDim>
int EuclideanDistance(kipl::base::TImage<MaskType,NDim> &mask,
		kipl::base::TImage<DistType,NDim> &dist, bool complement,
		MorphConnect conn)
{
	std::stringstream msg;
	kipl::logging::Logger logger("EuclideanDistance");
//	logger(kipl::logging::Logger::LogWarning,"Warning, the algorithm is buggy...");
    auto dims=mask.dims();
	const size_t sx=dims[0];
	const size_t sxy=dims[0]*dims[1];

	ptrdiff_t pos;
	size_t p,j;
	CNeighborhood NG(dims,NDim,conn);
	size_t Nng=NG.N();

	kipl::base::TImage<unsigned short,NDim> imx(dims), imy(dims), imz;


	const unsigned short max_dist=numeric_limits<unsigned short>::max();
	const MaskType inqueue=static_cast<MaskType>(2);

	// Initializing the priority queue
	// multimap<float,int,cmpkey> fifo;
	deque<size_t> fifoA,fifoB;
	deque<size_t> *rFifoA=&fifoA;
	deque<size_t> *rFifoB=&fifoB;

//	logger(kipl::logging::Logger::LogVerbose,"Initializing");
	MaskType *pMask=mask.GetDataPtr();
	if (complement)
		for (int i=0; i<mask.Size(); i++)
			pMask[i]=(pMask[i]==0);
	else
		for (int i=0; i<mask.Size(); i++)
			pMask[i]=(pMask[i]!=0);


	unsigned short *pImx=imx.GetDataPtr();
	unsigned short *pImy=imy.GetDataPtr();
	unsigned short *pImz=NULL;
	if (NDim==3) {
        imz.resize(dims);
		imz=0;
		pImz=imz.GetDataPtr();
	}

	imx=0;
	imy=0;
	size_t maskCnt=0;

	// Initialize queue
	for (size_t i=0; i<mask.Size(); i++) {
		if (pMask[i]) {
			pImx[i]=max_dist;
			pImy[i]=max_dist;
			if (pImz)
				pImy[i]=max_dist;
			for (j=0; j<Nng; j++) {
				if ((pos=NG.neighbor(i,j))!=-1l) {
					if (pMask[pos]==static_cast<MaskType>(0)) {
						fifoA.push_back(i);
						pMask[i]=inqueue;
						break;
					}
				}
			}
		}
		else
			maskCnt++;
	}

//	logger(kipl::logging::Logger::LogVerbose,"Euclidean ordered propagation");
	float dmin, dp, current_dist;

	deque<size_t>::iterator it;
	size_t itcnt=0;
	ptrdiff_t pdx,pdy,pdz;
	size_t procCnt=0, maxCnt=mask.Size()-maskCnt;

	while ((!rFifoA->empty()) && (procCnt<maxCnt)) {
		dmin=numeric_limits<float>::max();
	
		for (it=rFifoA->begin(); it!=rFifoA->end(); it++) {
			p=*it;
			if (pImx[p]!=max_dist) {// Distance has already been computed
				continue;
			}
			dp=numeric_limits<float>::max();
			if (NDim==2) {
				for (j=0; j<Nng; j++) {
					if ((pos=NG.neighbor(p,j))!=-1l) {
						if (pImx[pos]!=max_dist) {
							pdx=pixdist_x(pos,p,sx);
							pdy=pixdist_y(pos,p,sx);
							current_dist=eudist2(static_cast<float>(pImx[pos]+pdx),
								static_cast<float>(pImy[pos]+pdy));

							if (current_dist<dp) {
								pImx[p]=pImx[pos]+pdx;
								pImy[p]=pImy[pos]+pdy;
								dp=current_dist;
							}
						}
					}
				}
			}
			else {
				for (j=0; j<Nng; j++) {
					if ((pos=NG.neighbor(p,j))!=-1) {
						if (pImx[pos]!=max_dist) {
							pdx=pImx[pos]+pixdist_x(pos,p,sx,sxy);
							pdy=pImy[pos]+pixdist_y(pos,p,sx,sxy);
							pdz=pImz[pos]+pixdist_z(pos,p,sx,sxy);
							current_dist=eudist2(static_cast<float>(pdx),
								static_cast<float>(pdy),
								static_cast<float>(pdz));

							if (current_dist<dp) {
								pImx[p]=pdx;
								pImy[p]=pdy;
								pImz[p]=pdz;
								dp=current_dist;
							}
						}
					}
				}
				if (dp<dmin)
					dmin=dp;
			}
		}
		size_t propCnt=0;
		size_t retCnt=0;
		while (!rFifoA->empty()) {
			p=rFifoA->front();
			rFifoA->pop_front();

			if ((NDim==2 ? eudist2(pImx[p],pImy[p]) : eudist2(pImx[p],pImy[p],pImz[p]))>dmin) {
				// Delay the propagation for
				// distances greater than dmin
				rFifoB->push_back(p);
				propCnt++;
			}
			else {
				procCnt++;
				retCnt++;
				for (j=0; j<Nng; j++) {
					if ((pos=NG.neighbor(p,j))!=-1) {
						if (pMask[pos]==1) {
							rFifoB->push_back(pos);
							pMask[pos]=inqueue;
						}
					}
				}
			}
		}
		
		if (fifoA.empty()) {
			rFifoA=&fifoB;
			rFifoB=&fifoA;
		}
		else {
			rFifoA=&fifoA;
			rFifoB=&fifoB;
		}
		msg.str("");
		msg<<"Processed "<<procCnt<<" of "<<maxCnt;
	//	logger(kipl::logging::Logger::LogDebug,msg.str());
		
		itcnt++;
	}
	//logger(kipl::logging::Logger::LogVerbose,"Calculating distances from coordinate fields");
	
    dist.resize(dims);
	DistType * pDist=dist.GetDataPtr();
	if (NDim==2)
		for (p=0; p<imx.Size(); p++)
				pDist[p]=eudist(pImx[p],pImy[p]);
	else
		for (p=0; p<imx.Size(); p++)
				pDist[p]=static_cast<DistType>(eudist(pImx[p],pImy[p],pImz[p]));

	return 0;
}

//CImage<double,2> DistanceTransform(CImage<char,2> img,MorphConnect connectivity);

/// \brief Computes the distance transform based a given metric
/// \param img input image, must be black and white
/// \param dist output image containing the distance map of img
/// \param metric an instance of the desired metric
/// \param complement Compute the distance map of the complementary image
template<class MaskType>
int DistanceTransform3D(kipl::base::TImage<MaskType,3> &img,
		kipl::base::TImage<float,3> &dist,
        kipl::morphology::CMetricBase &metric, bool complement)
{
	kipl::logging::Logger logger("DistanceTransform3D");

	std::ostringstream msg;
	const float object=10000.0f;
    if (metric.dim()!=3) {
    	msg.str("");
        msg<<"The selected metric ("<<metric.getName()<<") does not have 3D support.";
    	logger(kipl::logging::Logger::LogError,msg.str());

    	throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }

    msg.str("");
    msg<<"using metric "<<metric.getName();
    logger(kipl::logging::Logger::LogMessage,msg.str());

    auto dims=img.dims();

    dist.resize(dims);

    MaskType *pImg;
    float *pDist;

    metric.initialize(dims);

    int start=metric.start();
    ptrdiff_t N=static_cast<size_t>(img.Size());

    pDist=dist.GetDataPtr();
    pImg=img.GetDataPtr();

    // Initialize
    if (complement) {
		for (ptrdiff_t i=0; i<N; i++)
			pDist[i]= pImg[i]!=static_cast<MaskType>(0) ? 0.0f: object;
    }
    else {
		for (ptrdiff_t i=0; i<N; i++)
			pDist[i]= pImg[i]==static_cast<MaskType>(0) ? 0.0f: object;
    }

    // Forward scan
    for (size_t z=start; z<dims[2]-start; z++) {
    	for (size_t y=start; y<dims[1]-start ; y++) {
    		pDist=dist.GetLinePtr(y,z);
    		for (size_t x=start; x<dims[0]-start; x++) {
    			if (pDist[x])
    				pDist[x]=metric.forward(pDist+x);
    		}
    	}
    }

    // Backward scan
    for (size_t z=dims[2]-1-start; start<=z; z--) {
    	for (size_t y=dims[1]-1-start; start<=y ; y--) {
    		pDist=dist.GetLinePtr(y,z);
    		for (size_t x=dims[0]-1-start; start<=x;  x--) {
    			if (pDist[x])
    				pDist[x]=metric.backward(pDist+x);
    		}
    	}
    }

    // Cleanup
    pDist=dist.GetDataPtr();

    for (ptrdiff_t i=0; i<N; i++) {
        if (pDist[i]==object)
			pDist[i]=0.0f;
    }

    return 0;
}

/// Computes a 2D distance map using different metrics
/// \param img mask image
/// \param dist Resulting distnace map
/// \param metric A metric object
template<typename ImgType, typename DistType>
int DistanceTransform2D(kipl::base::TImage<ImgType,2> &img,
		kipl::base::TImage<DistType,2> &dist, CMetricBase &metric)
{
	int x,y;
    size_t const * const dims=img.Dims();
    dist.Resize(dims);

	ImgType *pImg;
	DistType *pDist;


    metric.initialize(dims);
    int start=metric.start();
	dist=numeric_limits<DistType>::max();

	for (y=start; y<dims[1]-start; y++) {
		pDist=dist.GetLinePtr(y)+start;
		pImg=img.GetLinePtr(y)+start;
		for (x=start; x<dims[0]-start; x++,pDist++, pImg++) {
			*pDist= *pImg==0 ? 0: numeric_limits<DistType>::max();
		}
 	}

	for (y=start; y<dims[1]-start; y++) {
		pDist=dist.GetLinePtr(y)+start;
		for (x=start; x<dims[0]-start; x++,pDist++) {
			if (*pDist)
				*pDist=(DistType)metric.forward(pDist);
		}
	}

	for (y=dims[1]-1-start; y>=start; y--) {
		pDist=dist.GetLinePtr(y)+dims[0]-1-start;
		for (x=dims[0]-1-start; x>=start; x--,pDist--) {
			if (*pDist)
				*pDist=(DistType)metric.backward(pDist);
		}
	}

    pDist=dist.GetDataPtr();
    for (x=0; x<dist.Size(); x++, pDist++)
        if (*pDist==numeric_limits<DistType>::max()) *pDist=(DistType)1;

    return 1;
}





}}}

#endif /*MORPHDIST_HPP_*/

//<LICENSE>

#include <vector>
#include <iterator>
#include <limits>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <morphology/morphology.h>
#include <morphology/morphfilters.h>
#include <math/mathfunctions.h>
#include <math/image_statistics.h>
#include <morphology/label.h>
#include <filters/medianfilter.h>
#include "../include/ImagingException.h"
#include "../include/SpotClean.h"

namespace ImagingAlgorithms {

SpotClean::SpotClean() :
	logger("ImagingAlgorithms::SpotClean"),
    mark(std::numeric_limits<float>::max()),
	m_fGamma(0.1f),
	m_fSigma(0.001f),
	m_nIterations(1),
	m_fMaxLevel(32000.0f),
	m_fMinLevel(600.0f),
	m_nMaxArea(100),
	m_eDetectionMethod(Detection_Ring),
	mLUT(1<<15,0.1f,0.0075f),
	eEdgeProcessingStyle(kipl::filters::FilterBase::EdgeMirror)
{
}

SpotClean::~SpotClean()
{
}

int SpotClean::Setup(size_t iterations,
		float threshold, float width,
		float minlevel, float maxlevel,
		int maxarea,
		ImagingAlgorithms::DetectionMethod method)
{
	m_fGamma           = threshold;
	m_fSigma           = width;
    m_nIterations      = static_cast<int>(iterations);
	m_fMaxLevel        = maxlevel;
	m_fMinLevel        = minlevel;
	m_nMaxArea         = maxarea;
	m_eDetectionMethod = method;

	mLUT.Setup(1<<15,m_fGamma,m_fSigma);

	return 0;
}

int SpotClean::Process(kipl::base::TImage<float,2> & img)
{
	std::ostringstream msg;
	msg.str("");
	msg<<Version();
	logger(kipl::logging::Logger::LogMessage,msg.str());

	kipl::base::TImage<float,2> res;

	kipl::containers::ArrayBuffer<PixelInfo> spots(img.Size());
	res=DetectSpots(img,&spots);
	img=CleanByArray(res,&spots);

	return 0;
}

int SpotClean::Process(kipl::base::TImage<float,3> & img)
{
	std::ostringstream msg;
	msg.str("");
	msg<<Version();
	logger(kipl::logging::Logger::LogVerbose,msg.str());

	for (int j=0; j<m_nIterations; j++) {
		msg.str("");
		msg<<"Spot clean iteration "<<j+1;
		logger(kipl::logging::Logger::LogVerbose,msg.str());

		#pragma omp parallel
		{
			kipl::base::TImage<float,2> res;

			kipl::base::TImage<float,2> proj(img.Dims());
			kipl::containers::ArrayBuffer<PixelInfo> spots(proj.Size());
            int nSlices=static_cast<int>(img.Size(2));
			#pragma omp for
            for (ptrdiff_t i=0; i<nSlices; i++) {
				memcpy(proj.GetDataPtr(),img.GetLinePtr(0,i),sizeof(float)*proj.Size());
				res=DetectSpots(proj,&spots);
				proj=CleanByArray(res,&spots);
				memcpy(img.GetLinePtr(0,i),proj.GetDataPtr(),sizeof(float)*proj.Size());
				spots.reset();
			}
		}
	}
	return 0;
}

kipl::base::TImage<float,2> SpotClean::CleanByList(kipl::base::TImage<float,2> img)
{
	kipl::base::TImage<float,2> s=DetectionImage(img);

	kipl::base::TImage<float,2> result=img;
	result.Clone();

	mLUT.InPlace(s.GetDataPtr(),s.Size());

	// Correction
	float *pWeight=s.GetDataPtr();
	float *pRes=result.GetDataPtr();

    float mark = numeric_limits<float>::max();
	for (size_t i=0; i<img.Size(); i++) {
		if (pWeight[i]!=0) {
			processList.push_back(PixelInfo(i,pRes[i],pWeight[i]));
			pRes[i]=mark;
		}
	}

	PrepareNeighborhood(img.Size(0),img.Size());
	std::list<PixelInfo >::iterator it, tmp;

	std::list<PixelInfo > corrected, remaining;
	PixelInfo pixel;

	float neigborhood[8];
	while (!processList.empty())
	{
		size_t N=processList.size();

		while (!processList.empty())
		{
			pixel=processList.front();
			// Extract neighborhood values
			int cnt=Neighborhood(pRes,pixel.pos,neigborhood);

			if (cnt!=0) {
				// Compute replacement value
				float mean=0.0f;

				for (int i=0; i<cnt; i++)
					mean+=neigborhood[i];
				mean=mean/cnt;

				pixel.value=(1-pixel.weight)*pixel.value + pixel.weight * mean;
				corrected.push_back(pixel);
			}
			else {
				remaining.push_back(pixel);
			}
			processList.pop_front();
		}

		if (N!=(corrected.size()+remaining.size()))
			throw ImagingException("List sizes doesn't match in correction loop",__FILE__,__LINE__);

		while (!corrected.empty()) {
			pixel=corrected.front();
			pRes[pixel.pos]=pixel.value;
			corrected.pop_front();
		}
		std::copy(remaining.begin(),remaining.end(),std::back_inserter(processList));
		remaining.clear();
	}

	int cnt=0;
	for (size_t i=0; i<result.Size(); i++) {
		if (pRes[i]==mark)
			cnt++;
	}
	if (cnt!=0) {
		ostringstream msg;
		msg<<"Failed to correct "<<cnt<<" pixels";
		throw ImagingException(msg.str(),__FILE__,__LINE__);
	}
	return result;
}


kipl::base::TImage<float,2> SpotClean::DetectSpots(kipl::base::TImage<float,2> img, kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
	kipl::base::TImage<float,2> s=DetectionImage(img);
//	kipl::base::TImage<float,2> s=StdDev(img,3);

	kipl::base::TImage<float,2> result=img;
	result.Clone();

	mLUT.InPlace(s.GetDataPtr(),s.Size());

//	float *pS=s.GetDataPtr();
//	for (size_t i=0; i<s.Size(); i++)
//		pS[i]=pS[i]<m_fGamma ? 0.0f : 1.0f;

	ExcludeLargeRegions(s);
	// Correction
	float *pWeight=s.GetDataPtr();
	float *pRes=result.GetDataPtr();

	kipl::containers::ArrayBuffer<PixelInfo> processArray(img.Size());

	for (size_t i=0; i<img.Size(); i++) {
		if ((pRes[i]<m_fMinLevel) || (m_fMaxLevel<pRes[i])) {
			pixels->push_back(PixelInfo(i,pRes[i],1.0f));
			pRes[i]=mark;
		}
		else if (pWeight[i]!=0) {
			pixels->push_back(PixelInfo(i,pRes[i],pWeight[i]));
			pRes[i]=mark;
		}
	}

	if (img.Size()<4*pixels->size()) {
		std::ostringstream msg;

		msg<<"Detected "<<static_cast<float>(pixels->size())/static_cast<float>(img.Size())<<"pixels. The result may be too smooth.";
		logger(kipl::logging::Logger::LogWarning,msg.str());
	}

	return result;
}

void SpotClean::ExcludeLargeRegions(kipl::base::TImage<float,2> &img)
{
	std::ostringstream msg;
	if (m_nMaxArea==0)
		return;
	kipl::base::TImage<float,2> thimg=img;
	thimg.Clone();

	float *pTh=thimg.GetDataPtr();

	for (size_t i=0; i<thimg.Size(); i++)
		pTh[i]= pTh[i]!=0.0f;

	kipl::base::TImage<int,2> lbl;
	size_t N=LabelImage(thimg, lbl,kipl::morphology::conn8);

	vector<pair<size_t,size_t> > area;
	vector<size_t> removelist;

	kipl::morphology::LabelArea(lbl,N,area);
	vector<pair<size_t,size_t> >::iterator it;

	for (it=area.begin(); it!=area.end(); it++) {
		if (m_nMaxArea<(it->first))
			removelist.push_back(it->second);
	}
	msg<<"Found "<<N<<" regions, "<<removelist.size()<<" are larger than "<<m_nMaxArea;
	logger(kipl::logging::Logger::LogVerbose,msg.str());

	RemoveConnectedRegion(lbl, removelist, kipl::morphology::conn8);

	int *pLbl=lbl.GetDataPtr();
	float *pImg=img.GetDataPtr();

	for (size_t i=0; i<img.Size(); i++)
		if (pLbl[i]==0)
			pImg[i]=0.0f;

}

kipl::base::TImage<float,2> SpotClean::CleanByArray(kipl::base::TImage<float,2> img,
													 kipl::containers::ArrayBuffer<PixelInfo> *pixels)
{
	PrepareNeighborhood(img.Size(0),img.Size());

	kipl::containers::ArrayBuffer<PixelInfo > toProcess(img.Size()), corrected(img.Size()), remaining(img.Size());

	float neigborhood[8];
	std::ostringstream msg;
	toProcess.copy(pixels);
	float *pRes=img.GetDataPtr();

	while (!toProcess.empty())
	{
		size_t N=toProcess.size();
		PixelInfo *pixel=toProcess.dataptr();

		for (size_t i=0; i<N; i++)
		{
			// Extract neighborhood values
			int cnt=Neighborhood(pRes,pixel[i].pos,neigborhood);

			if (cnt!=0) {
				// Compute replacement value. Here the mean is used, other replacements posible.
				float mean=0.0f;

				for (int j=0; j<cnt; j++)
					mean+=neigborhood[j];
				mean=mean/cnt;

				pixel[i].value+= pixel[i].weight * (mean - pixel[i].value);
				corrected.push_back(pixel[i]);
			}
			else {
				remaining.push_back(pixel[i]);
			}
		}

		if (N!=(corrected.size()+remaining.size()))
			throw ImagingException("List sizes doesn't match in correction loop",__FILE__,__LINE__);

		// Insert the replacements
		PixelInfo *correctedpixel=corrected.dataptr();
		size_t correctedN=corrected.size();

		for (size_t i=0; i<correctedN; i++) {
			pRes[correctedpixel[i].pos]=correctedpixel[i].value;
		}

		toProcess.copy(&remaining);
		remaining.reset();
		corrected.reset();
	}

	int cnt=0;
	for (size_t i=0; i<img.Size(); i++) {
		if (pRes[i]==mark)
			cnt++;
	}
	if (cnt!=0) {
		msg<<"Failed to correct "<<cnt<<" pixels";
		throw ImagingException(msg.str(),__FILE__,__LINE__);
	}
	return img;
}

int SpotClean::PrepareNeighborhood(int dimx, int N)
{
	sx=dimx;
	first_line=sx;
	last_line=N-sx;
	ng[0] = -sx-1;  ng[1] = -sx; ng[2] = -sx+1;
	ng[3] = -1;     ng[4] = 1;
	ng[5] = sx-1;   ng[6] = sx;  ng[7] = sx+1;

	return 0;
}

int SpotClean::Neighborhood(float * pImg, int idx, float * neigborhood)
{
	int cnt=0;
	int start = first_line < idx       ? 0 : (idx!=0 ? 3: 4);
	int stop  = idx        < last_line ? 8 : 5;

	for (int i=start; i<stop ; i++) {
		float val=pImg[idx+ng[i]];
		if (val!=mark) {
			neigborhood[cnt]=val;
			cnt++;
		}
	}

	return cnt;
}

double SpotClean::ChangeStatistics(kipl::base::TImage<float,2> img)
{
	const size_t N=img.Size();
	size_t cnt=0;
	float *pData=img.GetDataPtr();
	for (size_t i=0; i<N; i++) {
		cnt+=(pData[i]!=0.0f);
	}
	return static_cast<double>(cnt)/static_cast<double>(N);
}

kipl::base::TImage<float,2> SpotClean::DetectionImage(kipl::base::TImage<float,2> img, DetectionMethod method, size_t dims)
{
	return DetectionImage(img);
}


kipl::base::TImage<float,2> SpotClean::DetectionImage(kipl::base::TImage<float,2> img)
{
	kipl::base::TImage<float,2> det_img;
	switch (m_eDetectionMethod) {
    case Detection_StdDev    : det_img = StdDevDetection(img,3); break;
    case Detection_Ring      : det_img = RingDetection(img); break;
    case Detection_Median    : det_img = MedianDetection(img); break;
    case Detection_MinMax    : det_img = MinMaxDetection(img); break;
    case Detection_TriKernel : det_img = TriKernel(img); break;
	};

	return det_img;
}

kipl::base::TImage<float,2> SpotClean::RingDetection(kipl::base::TImage<float,2> img)
{
	const float w=1.0f/12.0f;
	const float c=-1.0f;
	const float z=0.0f;

	float k[25]={
			z, w, w, w, z,
			w, z, z, z, w,
			w, z, c, z, w,
			w, z, z, z, w,
			z, w, w, w, z};

	size_t kdims[2]={5,5};

	kipl::filters::TFilter<float,2> ring2(k,kdims);

	kipl::base::TImage<float,2> det_img;
	det_img=ring2(img, eEdgeProcessingStyle);
	det_img=kipl::math::abs(det_img);

	return det_img;
}

kipl::base::TImage<float,2> SpotClean::MedianDetection(kipl::base::TImage<float,2> img)
{
	size_t nFilterSize = 5;

	size_t nFiltDimsV[]={1, nFilterSize};
	size_t nFiltDimsH[]={nFilterSize, 1};
	kipl::filters::TMedianFilter<float,2> medianV(nFiltDimsV);
	kipl::filters::TMedianFilter<float,2> medianH(nFiltDimsH);

	kipl::base::TImage<float,2> mimg;
	mimg=medianV(img);
	mimg=medianH(mimg);

	return kipl::math::abs(img-mimg);
}

kipl::base::TImage<float,2> SpotClean::MinMaxDetection(kipl::base::TImage<float,2> img)
{
	size_t nFilterSize = 5;

	size_t nFiltDimsV[]={1, nFilterSize};
	size_t nFiltDimsH[]={nFilterSize, 1};
	float se[]={1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
	kipl::morphology::TErode<float,2> minV(se,nFiltDimsV);
	kipl::morphology::TErode<float,2> minH(se,nFiltDimsH);

	kipl::base::TImage<float,2> min_img;
	min_img=minV(img,kipl::filters::FilterBase::EdgeMirror);
	min_img=minH(min_img,kipl::filters::FilterBase::EdgeMirror);

	kipl::morphology::TDilate<float,2> maxV(se,nFiltDimsV);
	kipl::morphology::TDilate<float,2> maxH(se,nFiltDimsH);

	kipl::base::TImage<float,2> max_img;
	max_img=maxV(img,kipl::filters::FilterBase::EdgeMirror);
	max_img=maxH(max_img,kipl::filters::FilterBase::EdgeMirror);

	return kipl::math::abs(max_img-min_img);
}

kipl::base::TImage<float,2> SpotClean::BoxFilter(kipl::base::TImage<float,2> img, size_t dim)
{
	size_t dimsU[]={dim,1};
	size_t dimsV[]={1,dim};
	size_t N=dim*dim;
	float *fKernel=new float[N];
	for (size_t i=0; i<N; i++)
		fKernel[i]=1.0f;

	kipl::filters::TFilter<float,2> filterU(fKernel,dimsU);
	kipl::filters::TFilter<float,2> filterV(fKernel,dimsV);

	kipl::base::TImage<float,2> imgU=filterU(img, eEdgeProcessingStyle);
	delete [] fKernel;

	return filterV(imgU, eEdgeProcessingStyle);
}

kipl::base::TImage<float,2> SpotClean::TriKernel(kipl::base::TImage<float,2> img)
{
    kipl::base::TImage<float,2> dimg(img.Dims());
    dimg=0.0f;

    OneKernel(img,3,m_fGamma,dimg);
    OneKernel(img,5,m_fGamma,dimg);
    OneKernel(img,7,m_fGamma,dimg);

    return dimg;
}

void SpotClean::OneKernel(kipl::base::TImage<float,2> img, size_t size, float sigma, kipl::base::TImage<float,2> & dimg)
{
    kipl::base::TImage<float,2> fimg;
    float *pImg  = img.GetDataPtr();


    fimg=BoxFilter(img,size);
    float *pFImg = fimg.GetDataPtr();

    for (size_t i=0; i<dimg.Size(); i++) {
        pFImg[i]=fabs(pImg[i]-pFImg[i]);
    }

    pair<double,double> stats=kipl::math::statistics(pFImg,fimg.Size());

    float level=stats.first+sigma*stats.second;

    float *pDImg = dimg.GetDataPtr();
    for (size_t i=0; i<dimg.Size(); i++) {
        pDImg[i]=pDImg[i] || (level < pFImg[i]);
    }
}

kipl::base::TImage<float,2> SpotClean::StdDevDetection(kipl::base::TImage<float,2> img, size_t dim)
{
	kipl::base::TImage<float,2> img2=kipl::math::sqr(img);

	kipl::base::TImage<float,2> s2=BoxFilter(img2,dim);
	kipl::base::TImage<float,2> m=BoxFilter(img,dim);

	img2=0.0f;
	float *pRes=img2.GetDataPtr();
	float *pS2=s2.GetDataPtr();
	float *pM=m.GetDataPtr();

	const float M=1/float(dim*dim);
	const float M1=1/float(dim*dim-1);

	for (size_t i=0; i<img2.Size(); i++) {
		pRes[i]=sqrt(M1*(pS2[i]-M*pM[i]*pM[i]));
	}

	return img2;
}

}

void string2enum(std::string str, ImagingAlgorithms::DetectionMethod & method)
{
	method = ImagingAlgorithms::Detection_Ring;
	if (str=="stddev") method=ImagingAlgorithms::Detection_StdDev;
	if (str=="ring")   method=ImagingAlgorithms::Detection_Ring;
	if (str=="median") method=ImagingAlgorithms::Detection_Median;
	if (str=="minmax") method=ImagingAlgorithms::Detection_MinMax;
    if (str=="trikernel") method=ImagingAlgorithms::Detection_TriKernel;
}

std::ostream & operator<<(std::ostream &s, ImagingAlgorithms::DetectionMethod method)
{
	s<<enum2string(method);

	return s;
}

std::string enum2string(ImagingAlgorithms::DetectionMethod method)
{
	switch (method) {
	case ImagingAlgorithms::Detection_StdDev : return "stddev";
	case ImagingAlgorithms::Detection_Ring   : return "ring";
	case ImagingAlgorithms::Detection_Median : return "median";
	case ImagingAlgorithms::Detection_MinMax : return "minmax";
    case ImagingAlgorithms::Detection_TriKernel : return "trikernel";
	};

	return "ring";
}

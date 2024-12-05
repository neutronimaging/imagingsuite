

//
//#include "../../include/base/KiplException.h"
//#include "../../include/wavelets/wavelets.h"


//
//static void
//dwt_step (const gsl_wavelet * w, double *a, size_t stride, size_t n,
//          gsl_wavelet_direction dir, gsl_wavelet_workspace * work);
//namespace kipl { namespace wavelets {
//
//WaveletTransform1D::WaveletTransform1D(std::string waveletname, size_t k, size_t maxlen) :
//		w(nullptr),
//		work(nullptr),
//		m_nMaxLen(maxlen)
//{
//	w=gsl_wavelet_alloc(gsl_wavelet_daubechies,k);
//	if (w==nullptr)
//		throw kipl::base::KiplException("WaveletTransform1D: Failed to allocate wavelet",__FILE__,__LINE__);
//
//	work=gsl_wavelet_workspace_alloc(m_nMaxLen);
//	if (work==nullptr)
//		throw kipl::base::KiplException("WaveletTransform1D: Failed to allocate wavelet workspace",__FILE__,__LINE__);
//
//	data=new double[m_nMaxLen];
//
//}
//
//WaveletTransform1D::~WaveletTransform1D()
//{
//	if (w!=nullptr)
//		gsl_wavelet_free(w);
//
//	if (work!=nullptr)
//		gsl_wavelet_workspace_free(work);
//
//	if (data!=nullptr)
//		delete [] data;
//}
//
//kipl::base::TImage<float,1> WaveletTransform1D::TransformForward(kipl::base::TImage<float,1> &f)
//{
//	size_t N=f.Size();
//	if (N % 2)
//		throw kipl::base::KiplException("WaveletTransform1D: The signal has an odd number of elements.",__FILE__,__LINE__);
//
//	if (m_nMaxLen<N)
//		throw kipl::base::KiplException("WaveletTransform1D: The signal is longer than the allocated workspace.",__FILE__,__LINE__);
//
//	for (size_t i=0; i<N; i++)
//		data[i]=static_cast<double>(f[i]);
//
//	gsl_wavelet_transform_forward(w,data,1,N,work);
//
//	kipl::base::TImage<float,1> wf(&N);
//
//	for (size_t i=0; i<N; i++)
//		wf[i]=static_cast<float>(data[i]);
//
//
//	return wf;
//}
//
//kipl::base::TImage<float,1> WaveletTransform1D::TransformInverse(kipl::base::TImage<float,1> &wf)
//{
//	size_t N=wf.Size();
//
//	if (m_nMaxLen<N)
//		throw kipl::base::KiplException("WaveletTransform1D: The signal is longer than the allocated workspace.",__FILE__,__LINE__);
//
//	for (size_t i=0; i<N; i++)
//		data[i]=static_cast<double>(wf[i]);
//
//	gsl_wavelet_transform_inverse(w,data,1,N,work);
//
//	kipl::base::TImage<float,1> s(&N);
//
//	for (size_t i=0; i<N; i++)
//		wf[i]=static_cast<float>(data[i]);
//
//	return s;
//}
//
//
//WaveletTransform2D::WaveletTransform2D(std::string waveletname, size_t k, size_t maxsize) :
//	logger("WaveletTransform2D"),
//	w(nullptr),
//	work(nullptr)
//{
//	logger(kipl::logging::Logger::LogMessage,"Enter ctor");
//	w=gsl_wavelet_alloc(gsl_wavelet_daubechies,k);
//
//	if (w==nullptr)
//		throw kipl::base::KiplException("WaveletTransform1D: Failed to allocate wavelet",__FILE__,__LINE__);
//
//	logger(kipl::logging::Logger::LogMessage,"Wavelet allocation ok");
//	work=gsl_wavelet_workspace_alloc(m_nMaxLen);
//
//	if (work==nullptr)
//		throw kipl::base::KiplException("WaveletTransform1D: Failed to allocate wavelet workspace",__FILE__,__LINE__);
//
//	logger(kipl::logging::Logger::LogMessage,"Wavelet workspace ok");
//
//}
//
//WaveletQuad WaveletTransform2D::TransformForward(kipl::base::TImage<float,2> &img)
//{
//	kipl::base::TImage<double,2> data(img.Dims());
//
//	float *pImg=img.GetDataPtr();
//	double *pData=data.GetDataPtr();
//
//
//	for (size_t i=0; i<img.Size(); i++)
//		pData[i]=static_cast<double>(pImg[i]);
//
//	logger(kipl::logging::Logger::LogMessage,"Transfered data to double, starting horizontal transform");
//	for (size_t i=0; i<data.Size(1); i++)
////  	gsl_wavelet_transform_forward(w,data.GetLinePtr(i),1,data.Size(0),work);
//		 dwt_step (w, data.GetLinePtr(i), 1, data.Size(0), gsl_wavelet_forward, work);
//
//	logger(kipl::logging::Logger::LogMessage,"Starting vertical transform");
//	for (size_t i=0; i<data.Size(0); i++)
////		gsl_wavelet_transform_forward(w,pData+i,data.Size(0),data.Size(1),work);
//		dwt_step (w, pData+i, data.Size(0), data.Size(1), gsl_wavelet_forward, work);
//
//	logger(kipl::logging::Logger::LogMessage,"Wavelet transform done");
//
//	WaveletQuad wq=Transfer2Quad(data);
//
//	return wq;
//}
//
//kipl::base::TImage<float,2> WaveletTransform2D::TransformInverse(WaveletQuad &wq)
//{
//	kipl::base::TImage<double,2> data=Transfer2Image(wq);
//
//
//	kipl::base::TImage<float,2> img(data.Dims());
//	float *pImg=img.GetDataPtr();
//	double *pData=data.GetDataPtr();
//
//	for (size_t i=0; i<img.Size(); i++)
//		pData[i]=static_cast<double>(pImg[i]);
//
//
//
//	return img;
//}
//
//WaveletQuad WaveletTransform2D::Transfer2Quad(kipl::base::TImage<double,2> &img)
//{
//	size_t dims[2]={img.Size(0)/2,img.Size(1)/2};
//
//	WaveletQuad wq(dims);
//
//	// Transfer A
//	for (size_t i=0; i<dims[1]; i++) {
//		double *pImg=img.GetLinePtr(i);
//		float *pA=wq.a.GetLinePtr(i);
//		for (size_t j=0; j<dims[0]; j++) {
//			pA[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	// Transfer H
//	for (size_t i=0; i<dims[1]; i++) {
//		double *pImg=img.GetLinePtr(i)+dims[0];
//		float *pH=wq.h.GetLinePtr(i);
//		for (size_t j=0; j<dims[0]; j++) {
//			pH[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	// Transfer V
//	for (size_t i=0; i<dims[1]; i++) {
//		double *pImg=img.GetLinePtr(i+dims[1]);
//		float *pV=wq.v.GetLinePtr(i);
//		for (size_t j=0; j<dims[0]; j++) {
//			pV[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	// Transfer D
//	for (size_t i=0; i<dims[1]; i++) {
//		double *pImg=img.GetLinePtr(i+dims[1])+dims[0];
//		float *pD=wq.d.GetLinePtr(i);
//		for (size_t j=0; j<dims[0]; j++) {
//			pD[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	return wq;
//}
//
//kipl::base::TImage<double,2> WaveletTransform2D::Transfer2Image(WaveletQuad &wq)
//{
//	size_t dims[2]={wq.a.Size(0)*2,wq.a.Size(1)*2};
//	kipl::base::TImage<double,2> img(dims);
//
//	size_t const * const qdims=wq.a.Dims();
//
//	// Transfer A
//	for (size_t i=0; i<qdims[1]; i++) {
//		double *pImg=img.GetLinePtr(i);
//		float *pA=wq.a.GetLinePtr(i);
//		for (size_t j=0; j<qdims[0]; j++) {
//			pImg[j]=static_cast<float>(pA[j]);
//		}
//	}
//
//	// Transfer H
//	for (size_t i=0; i<qdims[1]; i++) {
//		double *pImg=img.GetLinePtr(i)+qdims[0];
//		float *pH=wq.h.GetLinePtr(i);
//		for (size_t j=0; j<qdims[0]; j++) {
//			pH[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	// Transfer V
//	for (size_t i=0; i<qdims[1]; i++) {
//		double *pImg=img.GetLinePtr(i+qdims[1]);
//		float *pV=wq.v.GetLinePtr(i);
//		for (size_t j=0; j<qdims[0]; j++) {
//			pV[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//	// Transfer D
//	for (size_t i=0; i<qdims[1]; i++) {
//		double *pImg=img.GetLinePtr(i+qdims[1])+qdims[0];
//		float *pD=wq.d.GetLinePtr(i);
//		for (size_t j=0; j<qdims[0]; j++) {
//			pD[j]=static_cast<float>(pImg[j]);
//		}
//	}
//
//
//	return img;
//}
//
//}}

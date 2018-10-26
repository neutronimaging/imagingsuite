#include <string>
#include <map>

#include <base/textractor.h>
#include <ParameterHandling.h>
#include <strings/miscstring.h>

#include "stripefiltermodule.h"
#include "ImagingModules_global.h"
#include <ImagingException.h>
#include <math/mathconstants.h>

#ifdef _OPENMP
#include <omp.h>
#endif


IMAGINGMODULESSHARED_EXPORT StripeFilterModule::StripeFilterModule(kipl::interactors::InteractionBase *interactor) : KiplProcessModuleBase("StripeFilterModule",false, interactor),
    m_StripeFilter(nullptr),
    m_sWaveletName("daub15"),
    m_nLevels(3),
    m_fSigma(0.05),
//    m_nDecNum(2),
    m_bParallelProcessing(false),
    op(ImagingAlgorithms::VerticalComponentFFT),
    plane(kipl::base::ImagePlaneXY)
{

}

IMAGINGMODULESSHARED_EXPORT StripeFilterModule::~StripeFilterModule()
{}


bool IMAGINGMODULESSHARED_EXPORT StripeFilterModule::updateStatus(float val, std::string msg)
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->SetProgress(val,msg);
    }
    return false;
}

int IMAGINGMODULESSHARED_EXPORT StripeFilterModule::Configure(KiplProcessConfig config, std::map<std::string, std::string> parameters)
{
    m_sWaveletName=GetStringParameter(parameters,"waveletname");
//    scale=GetIntParameter(parameters,"scale");
    m_fSigma=GetFloatParameter(parameters,"sigma");
    string2enum(GetStringParameter(parameters,"filtertype"), op);
    string2enum(GetStringParameter(parameters,"plane"), plane);
    m_nLevels             = GetIntParameter(parameters,"decnum");
    m_bParallelProcessing = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));

    return 0;
}

std::map<std::string, std::string> IMAGINGMODULESSHARED_EXPORT StripeFilterModule::GetParameters()
{
    std::map<std::string, std::string> parameters;

    parameters["waveletname"]=m_sWaveletName;
//    parameters["scale"]=kipl::strings::value2string(scale);
    parameters["sigma"]=kipl::strings::value2string(m_fSigma);
    parameters["filtertype"]=enum2string(op);
    parameters["decnum"]   = kipl::strings::value2string(m_nLevels);
    parameters["parallel"] = m_bParallelProcessing ? "true" : "false";
    parameters["plane"]=enum2string(plane);

    return parameters;
}

int IMAGINGMODULESSHARED_EXPORT StripeFilterModule::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    size_t Nslices=0;

    switch (plane) {
    case kipl::base::ImagePlaneXY:
        Nslices=img.Size(2);
        break;
    case kipl::base::ImagePlaneXZ:
        Nslices=img.Size(1);
        break;
    case kipl::base::ImagePlaneYZ:
        Nslices=img.Size(0);
        break;
    default:
        break;
    }

    kipl::base::TImage<float,2> slice;
    slice=kipl::base::ExtractSlice(img,0,plane,nullptr);

    m_StripeFilter = new ImagingAlgorithms::StripeFilter(slice.Dims(),m_sWaveletName,m_nLevels,m_fSigma);

//    #pragma omp for
    for (size_t i=0; (i<Nslices && (updateStatus(float(i)/Nslices,"Processing Stripe Filter")==false) ); i++) {
        slice=kipl::base::ExtractSlice(img,i,plane,nullptr);
        m_StripeFilter->Process(slice,op);
        kipl::base::InsertSlice(slice,img,i,plane);
    }

    delete m_StripeFilter;

    m_StripeFilter=nullptr;

    return 0;

}

//int StripeFilterModule::ProcessSingle(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
//{
//	size_t dims[2]={img.Size(0), img.Size(2)};
//	bool fail=false;
//		kipl::base::TImage<float,2> sinogram;
//		ImagingAlgorithms::StripeFilter *filter=NULL;
//		try {
//			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
//		}
//		catch (kipl::base::KiplException &e) {
//				logger(kipl::logging::Logger::LogError,e.what());
//				fail=true;
//		}
//		if (!fail) {
//            const size_t N=img.Size(1);
//            for (size_t j=0; (j<N) && (UpdateStatus(float(j)/N,m_sModuleName)==false); ++j)
//			{
//				ExtractSinogram(img,sinogram,j);

//				filter->Process(sinogram);

//				InsertSinogram(sinogram,img,j);
//			}
//			delete filter;
//		}

//	if (fail) {
//		throw ImagingException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
//	}
//	return 0;


//}

//int StripeFilterModule::ProcessParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
//{
//	size_t dims[2]={img.Size(0), img.Size(2)};
//	bool fail=false;
//    int N=static_cast<int>(img.Size(1));
//	#pragma omp parallel
//	{
//		kipl::base::TImage<float,2> sinogram;
//		ImagingAlgorithms::StripeFilter *filter=NULL;
//		try {
//			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
//		}
//		catch (kipl::base::KiplException &e) {
//			#pragma omp critical
//			{
//				logger(kipl::logging::Logger::LogError,e.what());
//				fail=true;
//			}
//		}
//		if (!fail) {
//			#pragma omp for
//            for (int j=0; j<N; j++)
//			{
//				std::cout<<"Processing sinogram "<<j<<std::endl;
//				ExtractSinogram(img,sinogram,j);

//				filter->Process(sinogram);

//				InsertSinogram(sinogram,img,j);
//			}
//			delete filter;
//		}
//	}

//	if (fail) {
//		throw ImagingException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
//	}
//	return 0;
//}



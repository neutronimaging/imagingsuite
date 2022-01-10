//<LICENSE>

#include "../include/StdPreprocModules_global.h"
#include "../include/WaveletRingClean.h"

#include <ParameterHandling.h>
#include <StripeFilter.h>
#include <ReconException.h>
#include <functional>
#include <tuple>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <math/mathconstants.h>

WaveletRingClean::WaveletRingClean(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("WaveletRingClean",interactor),
    m_sWName("daub15"),
    m_fSigma(0.05f),
    m_nDecNum(2),
	m_eCleanMethod(ImagingAlgorithms::VerticalComponentFFT)
{
    m_bThreading = true;
    publications.push_back(Publication({"B. Muench","P. Trtik","F. Marone","M. Stampanoni"},
                                        "Stripe and ring artifact removal with combined wavelet-Fourier filtering",
                                        "Optics express",
                                        2009,
                                        17,
                                        10,
                                        "8567--8591",
                                        "10.1364/oe.17.008567"));
}

WaveletRingClean::~WaveletRingClean() 
{
}

int WaveletRingClean::Configure(ReconConfig UNUSED(config), std::map<std::string, std::string> parameters)
{
	m_sWName              = GetStringParameter(parameters,"wname");
	m_fSigma              = GetFloatParameter(parameters,"sigma");
	m_nDecNum             = GetIntParameter(parameters,"decnum");
	string2enum(GetStringParameter(parameters,"method"),m_eCleanMethod);
    m_bThreading          = kipl::strings::string2bool(GetStringParameter(parameters,"threading"));


	return 0;
}

std::map<std::string, std::string> WaveletRingClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["wname"]    = m_sWName;
	parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
	parameters["decnum"]   = kipl::strings::value2string(m_nDecNum);
	parameters["method"]   = enum2string(m_eCleanMethod);
    parameters["threading"] = kipl::strings::bool2string(m_bThreading);

	return parameters;
}

bool WaveletRingClean::SetROI(const std::vector<size_t> &roi)
{
    std::ignore = roi;

	return false;
}

int WaveletRingClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    m_nCounter = 0;
	int retval=0;
    if (m_bThreading)
        retval=ProcessParallelStd(img);
	else
		retval=ProcessSingle(img,coeff);

	return retval;
}

int WaveletRingClean::ProcessSingle(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    std::vector<size_t> dims={img.Size(0), img.Size(2)};
	bool fail=false;

    kipl::base::TImage<float,2> sinogram;
    ImagingAlgorithms::StripeFilter *filter=nullptr;

    try
    {
        filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
    }
    catch (kipl::base::KiplException &e)
    {
            logger(kipl::logging::Logger::LogError,e.what());
            fail=true;
    }

    if (!fail) {
        const size_t N=img.Size(2);
        for (size_t j=0; (j<N) && (UpdateStatus(float(j)/N,m_sModuleName)==false); ++j)
        {
            ExtractSinogram(img,sinogram,j);

            filter->process(sinogram);

            InsertSinogram(sinogram,img,j);
        }
        delete filter;
    }

	if (fail) {
		throw ReconException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
	}
	return 0;


}

int WaveletRingClean::ProcessParallel(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & UNUSED(coeff))
{
    std::vector<size_t> dims={img.Size(0), img.Size(2)};
	bool fail=false;
    int N=static_cast<int>(img.Size(1));
	#pragma omp parallel
	{
		kipl::base::TImage<float,2> sinogram;
        ImagingAlgorithms::StripeFilter *filter=nullptr;
		try {
			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
		}
		catch (kipl::base::KiplException &e) {
			#pragma omp critical
			{
				logger(kipl::logging::Logger::LogError,e.what());
				fail=true;
			}
		}
        if (!fail)
        {
			#pragma omp for
            for (int j=0; j<N; j++)
			{
                ++m_nCounter;
                UpdateStatus(static_cast<float>(j)/static_cast<float>(N),"Wavelet ring clean (OMP)");
				std::cout<<"Processing sinogram "<<j<<std::endl;
				ExtractSinogram(img,sinogram,j);

                filter->process(sinogram);

				InsertSinogram(sinogram,img,j);
			}
			delete filter;
		}
	}

	if (fail) {
		throw ReconException("Failed to process the projections using stripe filter.",__FILE__,__LINE__);
	}
	return 0;
}


int WaveletRingClean::ProcessParallelStd(kipl::base::TImage<float,3> & img)
{
    std::ostringstream msg;
    const size_t concurentThreadsSupported = std::thread::hardware_concurrency();


    std::vector<std::thread> threads;
    const size_t N = img.Size(1);

    size_t M=N/concurentThreadsSupported;

    msg.str("");
    msg<<N<<" sinograms on "<<concurentThreadsSupported<<" threads, "<<M<<" sinograms per thread";
    logger(logger.LogMessage,msg.str());
    m_nCounter = 0;

    size_t begin = 0;
    size_t end   = 0;
    ptrdiff_t rest  = N % concurentThreadsSupported ; // Take care of the rest slices

    for(size_t i = 0; i < concurentThreadsSupported; ++i,--rest)
    {
        if ( 0 < rest )
            end = begin + M + 1;
        else
            end = begin + M;

        auto pImg = &img;
        threads.push_back(std::thread([=] { ProcessParallelStdBlock(i,pImg,begin,end); }));
        begin = end;
    }

    // call join() on each thread in turn
    for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));

    return 0;
}

int WaveletRingClean::ProcessParallelStdBlock(size_t tid, kipl::base::TImage<float, 3> *img, size_t begin, size_t end)
{
    std::ostringstream msg;
    bool fail = false;

    msg<<"Starting waveletringclean thread number="<<tid<<", N="<<end-begin;
    logger.verbose(msg.str());
    msg.str("");
    msg<<"Thread "<<tid<<" progress :";

    std::vector<size_t> dims={img->Size(0), img->Size(2)};

    kipl::base::TImage<float,2> sinogram;
    ImagingAlgorithms::StripeFilter *filter=nullptr;
    try
    {
        filter = new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
    }
    catch (kipl::base::KiplException &e)
    {
        {
            logger(kipl::logging::Logger::LogError,e.what());
            fail=true;
        }
    }

    if (!fail)
    {
        for (size_t j=begin; j<end; j++)
        {
            ExtractSinogram(*img,sinogram,j);

            filter->process(sinogram);
            ++m_nCounter;

            UpdateStatus(static_cast<float>(m_nCounter.load())/static_cast<float>(img->Size(1)),"Wavelet ring-clean parallel");
            InsertSinogram(sinogram,*img,j);
        }
        delete filter;
    }

    logger.verbose(msg.str());

    return 0;
}

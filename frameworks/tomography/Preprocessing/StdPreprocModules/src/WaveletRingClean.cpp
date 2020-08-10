//<LICENSE>

#include "../include/StdPreprocModules_global.h"
#include "../include/WaveletRingClean.h"

#include <ParameterHandling.h>
#include <StripeFilter.h>
#include <ReconException.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <math/mathconstants.h>

WaveletRingClean::WaveletRingClean(kipl::interactors::InteractionBase *interactor) :
    PreprocModuleBase("WaveletRingClean",interactor),
    m_sWName("daub15"),
    m_fSigma(0.05f),
    m_nDecNum(2),
	m_bParallelProcessing(false),
	m_eCleanMethod(ImagingAlgorithms::VerticalComponentFFT)
{
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
	m_bParallelProcessing = kipl::strings::string2bool(GetStringParameter(parameters,"parallel"));
	string2enum(GetStringParameter(parameters,"method"),m_eCleanMethod);

	return 0;
}

std::map<std::string, std::string> WaveletRingClean::GetParameters()
{
	std::map<std::string, std::string> parameters;

	parameters["wname"]    = m_sWName;
	parameters["sigma"]    = kipl::strings::value2string(m_fSigma);
	parameters["decnum"]   = kipl::strings::value2string(m_nDecNum);
	parameters["parallel"] = m_bParallelProcessing ? "true" : "false";
	parameters["method"]   = enum2string(m_eCleanMethod);

	return parameters;
}

bool WaveletRingClean::SetROI(size_t * UNUSED(roi))
{
	return false;
}

int WaveletRingClean::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
	int retval=0;
	if (m_bParallelProcessing)
		retval=ProcessParallel(img,coeff);
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
		try {
			filter=new ImagingAlgorithms::StripeFilter(dims,m_sWName,m_nDecNum, m_fSigma);
		}
		catch (kipl::base::KiplException &e) {
				logger(kipl::logging::Logger::LogError,e.what());
				fail=true;
		}
		if (!fail) {
            const size_t N=img.Size(1);
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
		if (!fail) {
			#pragma omp for
            for (int j=0; j<N; j++)
			{
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





//<LICENSE>

#include "stdafx.h"
#include "CountNANs.h"

CountNANs::CountNANs() :
	PreprocModuleBase("CountNANs")
{

}

CountNANs::~CountNANs() {
}

std::map<std::basic_string<char>, std::basic_string<char> > CountNANs::GetParameters()
{
	std::map<std::basic_string<char>, std::basic_string<char> > parameters;

	return parameters;
}

int CountNANs::Configure(ReconConfig config, std::map<std::basic_string<char>, std::basic_string<char> > parameters)
{
	return 0;
}

int CountNANs::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{
	size_t cnt=0;
	float *pImg=NULL;
//	size_t N=img.Size(0)*img.Size(1);
	std::ostringstream msg;

	for (size_t slice=0; slice<img.Size(1); slice++) {
		cnt=0;
		for (size_t proj=0; proj<img.Size(2); proj++) {
			pImg=img.GetLinePtr(slice,proj);
			

			for (size_t i=0; i<img.Size(0); i++) {
				cnt+=(pImg[i]!=pImg[i]);
			}

		}
		if (cnt!=0) {
			msg.str("");
			msg<<"Found "<<cnt<<" NaNs in slice "<<slice;
			logger(kipl::logging::Logger::LogMessage,msg.str());
		}

	}

	return 0;
}


int CountNANs::ProcessCore(kipl::base::TImage<float,2> &img, std::map<std::string,std::string> &parameters)
{return 0;}

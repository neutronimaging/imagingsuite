/*
 * GeneralFilter.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: kaestner
 */
#include "../include/StdPreprocModules_global.h"

#include <algorithm>

#include <filters/filter.h>
#include <ParameterHandling.h>

#include <ReconException.h>

#include "../include/GeneralFilter.h"


GeneralFilter::GeneralFilter() :
    PreprocModuleBase("GeneralFilter"),
    filterType(FilterBox),
    filterSize(3.0)
{

}

GeneralFilter::~GeneralFilter() {

}


int GeneralFilter::Configure(ReconConfig config, std::map<std::string, std::string> parameters)
{
    string2enum(GetStringParameter(parameters,"type"),filterType);

    filterSize = GetFloatParameter(parameters,"size");

    return 0;
}

std::map<std::string, std::string> GeneralFilter::GetParameters()
{
	std::map<std::string, std::string> parameters;

    parameters["type"]=enum2string(filterType);
    parameters["size"]=kipl::strings::value2string(filterSize);

	return parameters;
}

bool GeneralFilter::SetROI(size_t *roi)
{
	return false;
}

int GeneralFilter::ProcessCore(kipl::base::TImage<float,2> & img, std::map<std::string, std::string> & coeff)
{
    float *kernel=nullptr;
    size_t N  = 0;
    size_t N2 = 0;
    float w   = 0.0f;
    float sum = 0.0f;

    switch (filterType) {
    case FilterBox:
        N=static_cast<size_t>(filterSize);
        kernel=new float[N];
        w=1.0f/N;
        for (size_t i=0; i<N; ++i)
            kernel[i]=w;
        break;
    case FilterGauss:
        N=2*ceil(2.5*filterSize)+1;
        N2=N/2;
        kernel=new float[N];
        sum = kernel[N2] = 1.0f;
        for (size_t i=1; i<=N2; ++i)
        {
            float x=static_cast<float>(i);
            kernel[N2+i] = kernel[N2-i] = exp(-x*x/(2*filterSize*filterSize));
            sum+=kernel[N2+i]*2;
        }
        for (size_t i=0; i<N; ++i)
            kernel[i]=kernel[i]/sum;
        break;
    default:
        throw ReconException("Unknown filter type selected",__FILE__,__LINE__);
    }

    size_t dims[2]={N,1};
    kipl::filters::TFilter<float,2> fltH(kernel,dims);

    dims[0]=1; dims[1]=N;
    kipl::filters::TFilter<float,2> fltV(kernel,dims);

    kipl::base::TImage<float,2> tmp=fltH(img,kipl::filters::FilterBase::EdgeMirror);

    img=fltV(tmp,kipl::filters::FilterBase::EdgeMirror);

	return 0;
}

int GeneralFilter::ProcessCore(kipl::base::TImage<float,3> & img, std::map<std::string, std::string> & coeff)
{
    kipl::base::TImage<float,2> slice(img.Dims());

    for (size_t i=0; i<img.Size(2); ++i) {
        std::copy(img.GetLinePtr(0,i),img.GetLinePtr(0,i)+slice.Size(),slice.GetDataPtr());

        ProcessCore(slice,coeff);

        std::copy(slice.GetDataPtr(),slice.GetDataPtr()+slice.Size(),img.GetLinePtr(0,i));
    }

    return 0;
}

void string2enum(const std::string str,GeneralFilter::eGeneralFilter &ft)
{
    std::map<std::string, GeneralFilter::eGeneralFilter> types;

    types["FilterBox"]   = GeneralFilter::FilterBox;
    types["filterbox"]   = GeneralFilter::FilterBox;
    types["FilterGauss"] = GeneralFilter::FilterGauss;
    types["filtergauss"] = GeneralFilter::FilterGauss;

    auto it=types.find(str);

    if (it!=types.end())
        ft=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to filter type enum", __FILE__, __LINE__);

}

std::string enum2string(GeneralFilter::eGeneralFilter ft)
{
    std::map<GeneralFilter::eGeneralFilter, std::string> types;

    types[GeneralFilter::FilterBox]   = "FilterBox";
    types[GeneralFilter::FilterGauss] = "FilterGauss";

    auto it=types.find(ft);
    std::string name;
    if (it!=types.end())
        name=it->second;
    else
        throw kipl::base::KiplException("Could not transform string to filter type enum", __FILE__, __LINE__);

    return name;
}

std::ostream & operator<<(std::ostream & s, GeneralFilter::eGeneralFilter ft)
{
    s<<enum2string(ft);

    return s;
}


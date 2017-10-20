//<LICENSE>

#include "stdafx.h"
#include "ProjectionInspector.h"

ProjectionInspector::ProjectionInspector() :
	PreprocModuleBase("ProjectionInspector")
{

}

ProjectionInspector::~ProjectionInspector() {

}

std::map<std::basic_string<char>, std::basic_string<char> > ProjectionInspector::GetParameters()
{
	std::map<std::basic_string<char>, std::basic_string<char> > parameters;

	return parameters;
}

int ProjectionInspector::Configure(ReconConfig, std::map<std::basic_string<char>, std::basic_string<char> >)
{

	return 0;
}

int ProjectionInspector::ProcessCore(kipl::base::TImage<float,3> &img, std::map<std::string,std::string> &parameters)
{

	return 0;
}

//
// This file is part of the Inspector library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author: kaestner $
// $File$
// $Date: 2012-05-21 16:13:06 +0200 (Mon, 21 May 2012) $
// $Rev: 1315 $
// $Id: ProjectionInspector.cpp 1315 2012-05-21 14:13:06Z kaestner $
//

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

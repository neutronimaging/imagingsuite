//
// This file is part of the preprocessing modules recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
// $Id$
//

//#include "stdafx.h"
#include "../include/StdPreprocModules_global.h"
#include <iostream>
#include "../include/ReconException.h"
#include "../include/PreprocEnums.h"
#include <string>
#include <strings/miscstring.h>

std::ostream & operator<<(std::ostream & s, eNormFunctionType nf)
{
	switch (nf) {
		case Norm_NegLog :
			s<<"NegLog";
			break;
		case Norm_Log :
			s<<"Log";
			break;
		case Norm_DoseWeight :
			s<<"DoseWeight";
			break;
		default: throw ReconException("Unknown NormFunctionType found in ostream operator",__FILE__,__LINE__);
	}
	return s;
}

void string2enum(const std::string str, eNormFunctionType &nf)
{
	
	if (str=="NegLog")		nf = Norm_NegLog ;
	else if (str=="Log")		nf = Norm_Log ;
		else if (str=="DoseWeight")	nf = Norm_DoseWeight ;
	else throw ReconException("Unknown NormFunctionType found in string2enum",__FILE__,__LINE__);
}


void string2enum(const std::string str,eSpotCleanAlgorithm &alg)
{
	std::string s=kipl::strings::toLower(str);
	if (s=="spotclean_iterations") alg=SpotClean_Iterations;
	else if (s=="spotclean_pixellist") alg=SpotClean_PixelList;
	else if (s=="spotclean_levelsets") alg=SpotClean_LevelSets;
	else if (s=="spotclean_absunbiased") alg=SpotClean_AbsUnbiased;
	else throw ReconException("Could not interpret string in SpotClean::string2enum",__FILE__,__LINE__);
}

void string2enum(const std::string str,eSpotDetection &method)
{
	std::string s=kipl::strings::toLower(str);
	if (s=="spotdetection_mean") method=SpotDetection_Mean;
	else if (s=="spotdetection_median") method=SpotDetection_Median;
	else if (s=="spotdetection_stddev_mean") method=SpotDetection_StdDev_Mean;
	else if (s=="spotdetection_stddev_median") method=SpotDetection_StdDev_Median;
	else throw ReconException("Could not interpret string in SpotClean::string2enum",__FILE__,__LINE__);
}

std::ostream & operator<<(std::ostream & s, eSpotCleanAlgorithm alg)  
{
	switch (alg) {
	case SpotClean_Iterations  : s<<"spotclean_iterations"; break;
	case SpotClean_PixelList   : s<<"spotclean_pixellist" ; break;
	case SpotClean_LevelSets   : s<<"spotclean_levelsets" ; break;
	case SpotClean_AbsUnbiased : s<<"spotclean_absunbiased" ; break;
	default : throw ReconException("Could not convert spot clean enum to string",__FILE__,__LINE__);
	}

	return s;
}

std::ostream & operator<<(std::ostream & s, eSpotDetection method)  
{
	switch (method) {
	case SpotDetection_Mean          : s<<"spotdetection_mean"; break;
	case SpotDetection_Median        : s<<"spotdetection_median" ; break;
	case SpotDetection_StdDev_Mean   : s<<"spotdetection_stddev_mean" ; break;
	case SpotDetection_StdDev_Median : s<<"spotdetection_stddev_median" ; break;
	default : throw ReconException("Could not convert spot detection enum to string",__FILE__,__LINE__);
	}

	return s;
}

void string2enum(const std::string str,eWeightFunctionType &wf)
{
	if (str=="weigthfunction_linear") wf=WeightFunction_Linear;
	else if (str=="weigthfunction_sigmoid") wf=WeightFunction_Sigmoid;
	else throw ReconException("Could not interpret string in WeightFunctionType::string2enum",__FILE__,__LINE__);
}

std::ostream & operator<<(std::ostream & s, eWeightFunctionType wf)
{
	switch (wf) {
		case WeightFunction_Linear  : s<<"weigthfunction_linear" ; break;
		case WeightFunction_Sigmoid : s<<"weigthfunction_sigmoid" ; break;
		default : throw ReconException("Could not convert weight function enum to string",__FILE__,__LINE__);
	}

	return s;
}

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


#ifndef __RECONENUMS_H
#define __RECONENUMS_H
#include "StdPreprocModules_global.h"
#include <iostream>
/// \brief Enum to select the norm plugin
enum eNormFunctionType {
	Norm_NegLog,	//!< Negative logarithm
	Norm_Log,		//!< Logarithm of projection
	Norm_DoseWeight	//!< Dose weight only
};

std::ostream & operator<<(std::ostream & s, eNormFunctionType nf);
void string2enum(const std::string str, eNormFunctionType &nf);

enum eSpotCleanAlgorithm {
	SpotClean_Iterations,
	SpotClean_PixelList,
	SpotClean_LevelSets,
	SpotClean_AbsUnbiased
};

enum eSpotDetection {
	SpotDetection_Mean,
	SpotDetection_Median,
	SpotDetection_StdDev_Mean,
	SpotDetection_StdDev_Median
};

enum eWeightFunctionType {
	WeightFunction_Sigmoid,
	WeightFunction_Linear
};


void string2enum(const std::string str,eWeightFunctionType &wf);
std::ostream & operator<<(std::ostream & s, eWeightFunctionType wf);

void string2enum(const std::string str,eSpotCleanAlgorithm &alg);
std::ostream & operator<<(std::ostream & s, eSpotCleanAlgorithm alg);

void string2enum(const std::string str,eSpotDetection &method);
std::ostream & operator<<(std::ostream & s, eSpotDetection method);

#endif

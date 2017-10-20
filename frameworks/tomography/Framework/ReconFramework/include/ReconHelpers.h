//<LICENSE>

#ifndef RECONHELPERS_H_
#define RECONHELPERS_H_

#include "ReconFramework_global.h"
//#include "stdafx.h"
#include <string>
#include <map>
#include <set>

#include "../include/ReconConfig.h"


struct RECONFRAMEWORKSHARED_EXPORT ProjectionInfo {
	ProjectionInfo() : name(""),angle(0.0f), weight(0.0f) {}
	ProjectionInfo(std::string _name, float _angle, float _weight=1.0f) : name(_name), angle(_angle), weight(_weight) {}
	std::string name;
	float angle;
	float weight;
};

bool RECONFRAMEWORKSHARED_EXPORT BuildFileList(ReconConfig const * const config,
                   std::map<float, ProjectionInfo> * ProjectionList, bool ignore_skiplist=false, char eolchar='\n');

bool RECONFRAMEWORKSHARED_EXPORT BuildFileList(std::string sFileMask, std::string sPath,
                   int nFirstIndex, int nLastIndex, int nProjectionStep,
                   float fScanArc[2], ReconConfig::cProjections::eScanType scantype,
                   std::set<size_t> * nlSkipList,
                   std::map<float, ProjectionInfo>  * ProjectionList);

int RECONFRAMEWORKSHARED_EXPORT ComputeWeights(ReconConfig const * const config,
                                               std::multimap<float, ProjectionInfo> &multiProjectionList,
                                               std::map<float, ProjectionInfo>  * ProjectionList);



#endif // RECONHELPERS_H_

//
// This file is part of the recon2 library by Anders Kaestner
// (c) 2011 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $File$
// $Date$
// $Rev$
// $Id$
//

#include "stdafx.h"
#include <sstream>
#include <string>
#include <map>

#include <strings/string2array.h>

#include "../include/ReconConfig.h"
#include "../include/ReconException.h"
#include "../include/ReconHelpers.h"

//#define EQUIDISTANT_WEIGHTS

bool BuildFileList(ReconConfig const * const config, std::map<float, ProjectionInfo>  * ProjectionList, bool ignore_skiplist)
{
	kipl::logging::Logger logger("BuildFileList");
	std::ostringstream msg;
	msg<<config->ProjectionInfo.nlSkipList.size()<<" projections will be skipped";
	ProjectionList->clear();

	bool sequence=true;

	std::string fname,ext;
	ext=config->ProjectionInfo.sFileMask.substr(config->ProjectionInfo.sFileMask.rfind('.'));

	if ((ext==".lst") || (ext==".txt")) {
		fname=config->ProjectionInfo.sPath+config->ProjectionInfo.sFileMask;
		ifstream listfile(fname.c_str());

		if (listfile.fail())
			throw ReconException("Could not open projection list file",__FILE__,__LINE__);

		std::string line;
		char cline[2048];
		size_t line_cnt=1;
		listfile.getline(cline,2048);
		while ((line_cnt < config->ProjectionInfo.nFirstIndex) && !listfile.eof()) {
			line_cnt++;
			listfile.getline(cline,2048);
		}

		while ((line_cnt <= config->ProjectionInfo.nLastIndex) && !listfile.eof()) {
			line_cnt++;
			line=cline;
			float angle=static_cast<float>(atof(line.c_str()));

			std::string fname=line.substr(line.find_first_of("\t")+1);
			(*ProjectionList)[angle]=ProjectionInfo(config->ProjectionInfo.sPath+fname,angle);
			listfile.getline(cline,2048);
		}
		listfile.close();
		sequence=false;
	}
	else {
        size_t skip=0;
		switch (config->ProjectionInfo.scantype) {
			case ReconConfig::cProjections::SequentialScan : {
					//const float fAngleStep=(config->ProjectionInfo.fScanArc[1]-config->ProjectionInfo.fScanArc[0])/(config->ProjectionInfo.nLastIndex-config->ProjectionInfo.nFirstIndex+1);
					const float fAngleStep=(config->ProjectionInfo.fScanArc[1]-config->ProjectionInfo.fScanArc[0])/
							static_cast<float>(config->ProjectionInfo.nLastIndex-config->ProjectionInfo.nFirstIndex+1);
					for (size_t i=config->ProjectionInfo.nFirstIndex;
						(i<=config->ProjectionInfo.nLastIndex);
						i+=config->ProjectionInfo.nProjectionStep)
					{
                        if (!ignore_skiplist) {
                            while (config->ProjectionInfo.nlSkipList.find(i)!=config->ProjectionInfo.nlSkipList.end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
						kipl::strings::filenames::MakeFileName(config->ProjectionInfo.sPath+config->ProjectionInfo.sFileMask,i,fname,ext,'#','0');
						float angle=(i-config->ProjectionInfo.nFirstIndex)*fAngleStep+config->ProjectionInfo.fScanArc[0];
						(*ProjectionList)[fmod(angle,180.0f)]=ProjectionInfo(fname,angle);
					}
				}
				break;
			case ReconConfig::cProjections::GoldenSectionScan : {
					const float fGoldenSection=0.5f*(1.0f+sqrt(5.0f));
					float arc=config->ProjectionInfo.fScanArc[1];
					if ((arc!=180.0f) && (arc!=360.0f))
						throw ReconException("The golden ratio reconstruction requires arc to be 180 or 360 degrees",__FILE__,__LINE__);

					for (size_t i=0; i<config->ProjectionInfo.nFirstIndex; i++) {
                        if (!ignore_skiplist) {
                            if (config->ProjectionInfo.nlSkipList.find(i)!=config->ProjectionInfo.nlSkipList.end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
					}

					for (size_t i=config->ProjectionInfo.nFirstIndex;
						(i<(config->ProjectionInfo.nLastIndex+skip));
						i++)
					{
                        if (!ignore_skiplist) {
                            while (config->ProjectionInfo.nlSkipList.find(i)!=config->ProjectionInfo.nlSkipList.end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
						kipl::strings::filenames::MakeFileName(config->ProjectionInfo.sPath+config->ProjectionInfo.sFileMask,i,fname,ext,'#','0');

						//float angle=static_cast<float>(fmod((i-config->ProjectionInfo.nFirstIndex)*fGoldenSection*arc,arc));
						float angle=static_cast<float>(fmod(static_cast<float>(i-1-skip)*fGoldenSection*arc,arc));
						(*ProjectionList)[fmod(angle,180.0f)]=ProjectionInfo(fname,angle);
					}
				}
				break;

			default:
				throw ReconException("Unknown scan type in BuildFileList",__FILE__,__LINE__);
		}
	}
#ifdef EQUIDISTANT_WEIGHTS
	std::map<float, ProjectionInfo>::iterator q0;
	for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++) {
		q0->second.weight=1.0f/static_cast<float>(ProjectionList->size());
	}

#else
	// Compute the projection weights
	std::map<float, ProjectionInfo>::iterator q0,q1,q2;

	q0=q2=ProjectionList->begin();
	q1=++q2;
	q2++;

	for (; q2!=ProjectionList->end(); q0++,q1++,q2++) {
		q1->second.weight=(q2->first-q0->first)/360;
	}

	if (175.0f<(config->ProjectionInfo.fScanArc[1]-config->ProjectionInfo.fScanArc[0])) {
		logger(kipl::logging::Logger::LogMessage,"Normal arc");
		// Compute last weight
		q2=ProjectionList->begin();
	//	q1->second.weight=((180.0f-q1->first)+(q1->first-q0->first))/(2*360.0f);
	//	std::cout<<"Last: q2->first= "<<q2->first<<", q0->first= "<<q0->first<<std::endl;
		q1->second.weight=((q2->first+180)-(q0->first))/360.0f;
		// Compute first weight
		q0=q1;
		q1=q2++;
		q1->second.weight=((q2->first)-(q0->first-180))/360.0f;
	}
	else {
		logger(kipl::logging::Logger::LogMessage,"Short arc");

		q0=q2=ProjectionList->begin();
		q2++;
		q0->second.weight=q2->second.weight;
		q1->second.weight=q2->second.weight;;
	}


#endif

	std::map<float, ProjectionInfo> ProjectionList2;

//	std::ofstream listfile1("projlist_unsorted.csv");

//	for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++) {
//		listfile1<< q0->first
//				<<"\t"<< q0->second.angle
//				<<"\t"<< q0->second.weight
//				<<"\t"<< q0->second.name<<"\n";
//
//	}

	for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++)
		ProjectionList2[q0->second.angle]=q0->second;

	*ProjectionList=ProjectionList2;

//	std::ofstream listfile2("projlist_sorted.csv");

//	for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++) {
//		listfile2<< q0->first
//				<<"\t"<< q0->second.angle
//				<<"\t"<< q0->second.weight
//				<<"\t"<< q0->second.name<<"\n";

//	}

	return sequence;
}


bool BuildFileList(std::string sFileMask, std::string sPath,
                   int nFirstIndex, int nLastIndex, int nProjectionStep,
                   float fScanArc[2], ReconConfig::cProjections::eScanType scantype,
                   std::set<size_t> * nlSkipList,
                   std::map<float, ProjectionInfo>  * ProjectionList)
{
    kipl::logging::Logger logger("BuildFileList");
    std::ostringstream msg;
    if (nlSkipList!=NULL)
    {
        msg.str();
        msg<<nlSkipList->size()<<" projections will be skipped";
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
    ProjectionList->clear();

    bool sequence=true;

    std::string fname,ext;
    ext=sFileMask.substr(sFileMask.rfind('.'));

    if ((ext==".lst") || (ext==".txt")) {
        fname=sPath+sFileMask;
        ifstream listfile(fname.c_str());

        if (listfile.fail())
            throw ReconException("Could not open projection list file",__FILE__,__LINE__);

        std::string line;
        char cline[2048];
        size_t line_cnt=1;
        listfile.getline(cline,2048);
        while ((line_cnt < nFirstIndex) && !listfile.eof()) {
            line_cnt++;
            listfile.getline(cline,2048);
        }

        while ((line_cnt <= nLastIndex) && !listfile.eof()) {
            line_cnt++;
            line=cline;
            float angle=static_cast<float>(atof(line.c_str()));

            std::string fname=line.substr(line.find_first_of("\t")+1);
            (*ProjectionList)[angle]=ProjectionInfo(sPath+fname,angle);
            listfile.getline(cline,2048);
        }
        listfile.close();
        sequence=false;
    }
    else {
        size_t skip=0;
        switch (scantype) {
            case ReconConfig::cProjections::SequentialScan : {
                    //const float fAngleStep=(config->ProjectionInfo.fScanArc[1]-config->ProjectionInfo.fScanArc[0])/(config->ProjectionInfo.nLastIndex-config->ProjectionInfo.nFirstIndex+1);
                    const float fAngleStep=(fScanArc[1]-fScanArc[0])/
                                            static_cast<float>(nLastIndex-nFirstIndex+1);
                    for (size_t i=nFirstIndex; i<=nLastIndex; i+=nProjectionStep)
                    {
                        if (nlSkipList!=NULL) {
                            while (nlSkipList->find(i)!=nlSkipList->end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
                        kipl::strings::filenames::MakeFileName(sPath+sFileMask,i,fname,ext,'#','0');
                        float angle=(i-nFirstIndex)*fAngleStep+fScanArc[0];
                        (*ProjectionList)[fmod(angle,180.0f)]=ProjectionInfo(fname,angle);
                    }
                }
                break;
            case ReconConfig::cProjections::GoldenSectionScan : {
                    const float fGoldenSection=0.5f*(1.0f+sqrt(5.0f));
                    float arc=fScanArc[1];
                    if ((arc!=180.0f) && (arc!=360.0f))
                        throw ReconException("The golden ratio reconstruction requires arc to be 180 or 360 degrees",__FILE__,__LINE__);

                    for (size_t i=0; i<nFirstIndex; i++) {
                        if (nlSkipList!=NULL) {
                            if (nlSkipList->find(i)!=nlSkipList->end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
                    }

                    for (size_t i=nFirstIndex; i<(nLastIndex+skip); i++)
                    {
                        if (nlSkipList!=NULL) {
                            while (nlSkipList->find(i)!=nlSkipList->end()) {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
                        kipl::strings::filenames::MakeFileName(sPath+sFileMask,i,fname,ext,'#','0');

                        //float angle=static_cast<float>(fmod((i-config->ProjectionInfo.nFirstIndex)*fGoldenSection*arc,arc));
                        float angle=static_cast<float>(fmod(static_cast<float>(i-1-skip)*fGoldenSection*arc,arc));
                        (*ProjectionList)[fmod(angle,180.0f)]=ProjectionInfo(fname,angle);
                    }
                }
                break;

            default:
                throw ReconException("Unknown scan type in BuildFileList",__FILE__,__LINE__);
        }
    }
#ifdef EQUIDISTANT_WEIGHTS
    std::map<float, ProjectionInfo>::iterator q0;
    for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++) {
        q0->second.weight=1.0f/static_cast<float>(ProjectionList->size());
    }

#else
    // Compute the projection weights
    std::map<float, ProjectionInfo>::iterator q0,q1,q2;

    q0=q2=ProjectionList->begin();
    q1=++q2;
    q2++;

    for (; q2!=ProjectionList->end(); q0++,q1++,q2++) {
        q1->second.weight=(q2->first-q0->first)/360;
    }

    if (175.0f<(fScanArc[1]-fScanArc[0])) {
        logger(kipl::logging::Logger::LogMessage,"Normal arc");
        // Compute last weight
        q2=ProjectionList->begin();
    //	q1->second.weight=((180.0f-q1->first)+(q1->first-q0->first))/(2*360.0f);
    //	std::cout<<"Last: q2->first= "<<q2->first<<", q0->first= "<<q0->first<<std::endl;
        q1->second.weight=((q2->first+180)-(q0->first))/360.0f;
        // Compute first weight
        q0=q1;
        q1=q2++;
        q1->second.weight=((q2->first)-(q0->first-180))/360.0f;
    }
    else {
        logger(kipl::logging::Logger::LogMessage,"Short arc");

        q0=q2=ProjectionList->begin();
        q2++;
        q0->second.weight=q2->second.weight;
        q1->second.weight=q2->second.weight;;
    }


#endif

    std::map<float, ProjectionInfo> ProjectionList2;
    for (q0=ProjectionList->begin(); q0!=ProjectionList->end(); q0++)
        ProjectionList2[q0->second.angle]=q0->second;

    *ProjectionList=ProjectionList2;


    return sequence;
}

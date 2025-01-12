//<LICENSE>

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <map>

#include <strings/string2array.h>
#include <strings/filenames.h>
#include <analyzefileext.h>

#include "../include/ReconConfig.h"
#include "../include/ReconException.h"
#include "../include/ReconHelpers.h"


//#define EQUIDISTANT_WEIGHTS

bool BuildFileList( const ReconConfig & config, 
                    std::map<float, ProjectionInfo>  & ProjectionList, 
                    char eolchar)
{
	kipl::logging::Logger logger("BuildFileList");
	std::ostringstream msg;
    msg<<config.ProjectionInfo.nlSkipList.size()<<" projections will be skipped";
    logger.debug(msg.str());
	ProjectionList.clear();

    std::multimap<float, ProjectionInfo> multiProjectionList;

	bool sequence=true;

    std::string fname;
    std::string ext;

    auto exttype = readers::GetFileExtensionType(config.ProjectionInfo.sFileMask);

    if ((exttype==readers::ExtensionLST) ||
        (exttype==readers::ExtensionTXT) ||
        (exttype==readers::ExtensionCSV))
    {
        logger.message("Using list file");
        fname=config.ProjectionInfo.sPath+config.ProjectionInfo.sFileMask;
        std::ifstream listfile(fname.c_str());

        if (listfile.fail())
            throw ReconException("Could not open projection list file",__FILE__,__LINE__);

        std::string line;
        char cline[2048];
        size_t line_cnt=1;
        listfile.getline(cline,2048);
        while ((line_cnt < config.ProjectionInfo.nFirstIndex) && !listfile.eof())
        {
            line_cnt++;
            listfile.getline(cline,2048,eolchar);
            logger.message(cline);

        }

        while ((line_cnt <= config.ProjectionInfo.nLastIndex) && !listfile.eof())
        {
            line_cnt++;
            line=cline;
            float angle=static_cast<float>(atof(line.c_str()));

            std::string current_fname=line.substr(line.find_first_of(",\t;")+1);
            current_fname=current_fname.substr(current_fname.find_first_not_of("\t "));
            current_fname=current_fname.substr(0,current_fname.find_first_of("\n\r"));

            multiProjectionList.insert(std::make_pair(fmod(angle,180.0f),ProjectionInfo(config.ProjectionInfo.sPath+current_fname,angle)));
            listfile.getline(cline,2048,eolchar);
        }
        listfile.close();
        sequence=false;
    }
    else
    {
        size_t skip=0;
        switch (config.ProjectionInfo.scantype)
        {
            case ReconConfig::cProjections::SequentialScan :
               {
                    size_t nCountAdjust = config.ProjectionInfo.skipListMode == ReconConfig::cProjections::SkipMode_Drop ? config.ProjectionInfo.nlSkipList.size() : 0;
                    const float fAngleStep  = (config.ProjectionInfo.fScanArc[1]-config.ProjectionInfo.fScanArc[0])/
                                               static_cast<float>(config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex-nCountAdjust);

                    float angle=0.0f;

                    for (size_t i=config.ProjectionInfo.nFirstIndex;
                        (i<=config.ProjectionInfo.nLastIndex);
                        i+=config.ProjectionInfo.nProjectionStep)
                    {
                        while (config.ProjectionInfo.nlSkipList.find(i) != config.ProjectionInfo.nlSkipList.end()) // Loop until no more skips
                        {
                            switch (config.ProjectionInfo.skipListMode)
                            {
                                case ReconConfig::cProjections::SkipMode_None:
                                    break;
                                case ReconConfig::cProjections::SkipMode_Skip:
                                    if (config.ProjectionInfo.nlSkipList.find(i) != config.ProjectionInfo.nlSkipList.end())
                                    {
                                        logger.message("Skipped projection "+std::to_string(i));
                                        i+=config.ProjectionInfo.nProjectionStep;
                                    }
                                    break;
                                case ReconConfig::cProjections::SkipMode_Drop:
                                    if (config.ProjectionInfo.nlSkipList.find(i) != config.ProjectionInfo.nlSkipList.end())
                                    {
                                        logger.message("Dropped projection " + std::to_string(i));
                                        ++i;
                                        ++skip;
                                    }
                                    break;
                                default:
                                    throw ReconException("Unknown skip list mode in BuildFileList",__FILE__,__LINE__);
                            }
                        }

                        angle=(i-config.ProjectionInfo.nFirstIndex-skip)*fAngleStep+config.ProjectionInfo.fScanArc[0];

						kipl::strings::filenames::MakeFileName(config.ProjectionInfo.sPath+config.ProjectionInfo.sFileMask,i,fname,ext,'#','0');
						
                        if ( exttype != readers::ExtensionHDF5 )
                        {
                            multiProjectionList.insert(std::make_pair(fmod(angle,180.0f),ProjectionInfo(fname,angle)));
                        }
                        else
                        {
                            multiProjectionList.insert(std::make_pair(fmod(angle,180.0f),ProjectionInfo(config.ProjectionInfo.sFileMask,angle)));
                        }
					}
				}
				break;
            case ReconConfig::cProjections::GoldenSectionScan :
            case ReconConfig::cProjections::InvGoldenSectionScan :
                {
                    float fGoldenSection=0.5f*(1.0f+sqrt(5.0f));
                    if (config.ProjectionInfo.scantype == ReconConfig::cProjections::InvGoldenSectionScan)
                        fGoldenSection = 1.0f/fGoldenSection;

					float arc=config.ProjectionInfo.fScanArc[1]- config.ProjectionInfo.fScanArc[0];
					// if ((arc!=180.0f) && (arc!=360.0f))
					// 	throw ReconException("The golden ratio reconstruction requires arc to be 180 or 360 degrees",__FILE__,__LINE__);

                    // Fixing the skips before the first projection
                    for (size_t i=0; i<config.ProjectionInfo.nFirstIndex; i++)
                    {
                        switch (config.ProjectionInfo.skipListMode) 
                        {
                            case ReconConfig::cProjections::SkipMode_None: break;
                            case ReconConfig::cProjections::SkipMode_Skip: 
                                logger.message("Skipped projection "+std::to_string(i));
                                ++i;
                            break;
                            case ReconConfig::cProjections::SkipMode_Drop:
                                logger.message("Dropped projection "+std::to_string(i)); 
                                ++i;
                                ++skip;
                            break;
                        }
					}

					for (size_t i=config.ProjectionInfo.nFirstIndex;
                        (i<=(config.ProjectionInfo.nLastIndex+skip));
						i++)
					{
                        switch (config.ProjectionInfo.skipListMode) 
                        {
                            case ReconConfig::cProjections::SkipMode_None: break;
                            case ReconConfig::cProjections::SkipMode_Skip: 
                                logger.message("Skipped projection "+std::to_string(i));
                                ++i;
                            break;
                            case ReconConfig::cProjections::SkipMode_Drop:
                                logger.message("Dropped projection "+std::to_string(i)); 
                                ++i;
                                ++skip;
                            break;
                        }

                        kipl::strings::filenames::MakeFileName(config.ProjectionInfo.sPath+config.ProjectionInfo.sFileMask,i,fname,ext,'#','0');

                        int idx = i - config.ProjectionInfo.nGoldenStartIdx
                                    - skip ;

                        float angle=static_cast<float>(fmod(static_cast<float>(idx)*fGoldenSection*arc,arc))-config.ProjectionInfo.fScanArc[0]; // TODO Update equation to handle 360 deg scans

                        if (exttype != readers::ExtensionHDF5 )
                        {
                            multiProjectionList.insert(std::make_pair(fmod(angle,180.0f),ProjectionInfo(fname,angle)));
                        }
                        else
                        {
                            multiProjectionList.insert(std::make_pair(fmod(angle,180.0f),ProjectionInfo(config.ProjectionInfo.sFileMask,angle)));
                        }
					}
				}
				break;

			default:
				throw ReconException("Unknown scan type in BuildFileList",__FILE__,__LINE__);
		}
	}

    ComputeWeights(config,multiProjectionList,ProjectionList);
    msg.str(""); msg<<"proj list size="<<ProjectionList.size();
    logger.verbose(msg.str());

	return sequence;
}

bool BuildFileList( std::string sFileMask, 
                    std::string sPath,
                    int nFirstIndex, 
                    int nLastIndex, 
                    int nProjectionStep,
                    const std::vector<float> &fScanArc, 
                    ReconConfig::cProjections::eScanType scantype, 
                    int goldenStartIdx,
                    std::set<size_t> * nlSkipList,
                    std::map<float, ProjectionInfo>  * ProjectionList)
{
    kipl::logging::Logger logger("BuildFileList");
    std::ostringstream msg;
    if (nlSkipList!=nullptr)
    {
        msg.str("");
        msg<<nlSkipList->size()<<" projections will be skipped";
        logger(kipl::logging::Logger::LogMessage,msg.str());
    }
    ProjectionList->clear();

    bool sequence=true;

    std::string fname,ext;
    ext=sFileMask.substr(sFileMask.rfind('.'));

    if ((ext==".lst") || (ext==".txt"))
    {
        fname=sPath+sFileMask;
        std::ifstream listfile(fname.c_str());

        if (listfile.fail())
            throw ReconException("Could not open projection list file",__FILE__,__LINE__);

        std::string line;
        char cline[2048];
        size_t line_cnt=1;
        listfile.getline(cline,2048);
        while ((line_cnt < static_cast<size_t>(nFirstIndex)) && !listfile.eof())
        {
            line_cnt++;
            listfile.getline(cline,2048);
        }

        while ((line_cnt <= static_cast<size_t>(nLastIndex)) && !listfile.eof())
        {
            line_cnt++;
            line=cline;
            float angle=static_cast<float>(atof(line.c_str()));

            std::string proj_fname=line.substr(line.find_first_of("\t")+1);
            (*ProjectionList)[angle]=ProjectionInfo(sPath+proj_fname,angle);
            listfile.getline(cline,2048);
        }
        listfile.close();
        sequence=false;
    }
    else
    {
        int skip=0;
        switch (scantype)
        {
            case ReconConfig::cProjections::SequentialScan :
                {
                    //const float fAngleStep=(config->ProjectionInfo.fScanArc[1]-config->ProjectionInfo.fScanArc[0])/(config->ProjectionInfo.nLastIndex-config->ProjectionInfo.nFirstIndex+1);
                    const float fAngleStep=(fScanArc[1]-fScanArc[0])/
                                            static_cast<float>(nLastIndex-nFirstIndex+1);
                    for (int i=nFirstIndex; i<=nLastIndex; i+=nProjectionStep)
                    {
                        if (nlSkipList!=nullptr)
                        {
                            while (nlSkipList->find(static_cast<size_t>(i))!=nlSkipList->end())
                            {
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
            case ReconConfig::cProjections::GoldenSectionScan :
            {
                    const float fGoldenSection=0.5f*(1.0f+sqrt(5.0f));
                    float arc=fScanArc[1];
                    if ((arc!=180.0f) && (arc!=360.0f))
                        throw ReconException("The golden ratio reconstruction requires arc to be 180 or 360 degrees",__FILE__,__LINE__);

                    for (int i=0; i<nFirstIndex; i++)
                    {
                        if (nlSkipList!=nullptr)
                        {
                            if (nlSkipList->find(static_cast<size_t>(i))!=nlSkipList->end())
                            {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
                    }

                    for (int i=nFirstIndex; i<(nLastIndex+skip); i++)
                    {
                        if (nlSkipList!=nullptr)
                        {
                            while (nlSkipList->find(static_cast<size_t>(i))!=nlSkipList->end())
                            {
                                msg.str("");
                                msg<<"Skipped projection "<<i;
                                logger(kipl::logging::Logger::LogMessage,msg.str());
                                i++;
                                skip++;
                            }
                        }
                        kipl::strings::filenames::MakeFileName(sPath+sFileMask,i,fname,ext,'#','0');

                        //float angle=static_cast<float>(fmod((i-config->ProjectionInfo.nFirstIndex)*fGoldenSection*arc,arc));
                        float angle=static_cast<float>(fmod(static_cast<float>(i-goldenStartIdx-skip)*fGoldenSection*arc,arc));
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

    for (; q2!=ProjectionList->end(); q0++,q1++,q2++)
    {
        q1->second.weight=(q2->first-q0->first)/360;
    }

    if (175.0f<(fScanArc[1]-fScanArc[0]))
    {
        logger(kipl::logging::Logger::LogMessage,"Normal arc");
        // Compute last weight
        q2=ProjectionList->begin();
        if (q1->first!=q0->first)
            q1->second.weight=((q2->first+180)-(q0->first))/360.0f;
        else
            q1->second.weight=0.0f;

        // Compute first weight
        q0=q1;
        q1=q2++;
        q1->second.weight=((q2->first)-(q0->first-180))/360.0f;
    }
    else
    {
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

int ComputeWeights(const ReconConfig & config, 
std::multimap<float, ProjectionInfo> &multiProjectionList, 
std::map<float, ProjectionInfo>  & ProjectionList)
{
    kipl::logging::Logger logger("ComputeWeights");
    if (multiProjectionList.size()<3) 
    {
        for (const auto & item : multiProjectionList)
        {
            ProjectionInfo info=item.second;
            info.weight=1.0/multiProjectionList.size();
            ProjectionList.insert(std::make_pair(item.first,info));
        }
        return 0;
    }

    //
    // Compute the projection weights
    std::multimap<float, ProjectionInfo>::iterator q0,q1,q2;

    q0=q2=multiProjectionList.begin();
    q1=++q2;
    ++q2;

    for (  ; q2!=multiProjectionList.end(); ++q0,++q1,++q2)
    {
        if (q1->first!=q0->first)
            q1->second.weight=(q2->first-q0->first)/360.0f;
        else
            q1->second.weight=0.0f;
    }

    //
    // Compute weights for list start and end
    if (175.0f<(config.ProjectionInfo.fScanArc[1]-config.ProjectionInfo.fScanArc[0]))
    {
        logger(kipl::logging::Logger::LogDebug,"Normal arc");
        // Compute last weight
        bool repeatedLast=false;
        q2=multiProjectionList.begin();
        if (q1->first!=q0->first)
            q1->second.weight=((q2->first+180)-(q0->first))/360.0f;
        else
        {
            q1->second.weight=0.0f;
            repeatedLast=true;
        }

        // Compute first weight
        q0=q1;
        q1=q2++;
        if (repeatedLast
                || config.ProjectionInfo.scantype == config.ProjectionInfo.GoldenSectionScan
                || config.ProjectionInfo.scantype == config.ProjectionInfo.InvGoldenSectionScan)
        {
            q1->second.weight=((q2->first)-(q0->first-180))/360.0f;
        }
        else
            q1->second.weight=((q2->first)-(q0->first-180))/180.0f;
    }
    else
    {
        logger.debug("Short arc");

        q0=q2=multiProjectionList.begin();
        q2++;
        q0->second.weight=q2->second.weight;
        q1->second.weight=q2->second.weight;
    }

    //
    // Fix weighting for multiple views on same angle
    std::list<std::multimap<float, ProjectionInfo>::iterator > stack;
    std::multimap<float, ProjectionInfo>::iterator q00=multiProjectionList.begin();
    std::multimap<float, ProjectionInfo>::iterator q11=q00;
    ++q11;
    float weight=0;

    while (q11!=multiProjectionList.end())
    {
        if (q11->second.weight==0.0f)
        {
            stack.clear();
            if (q11!=q00)
            {
                q00=q11; --q00;
            }

            do
            {
                stack.push_back(q11);
                ++q11;
            }
            while ((q11->second.weight==0.0f) && q11!=multiProjectionList.end());

            weight=q00->second.weight/(stack.size()+1);
            q00->second.weight = weight;

            while (!stack.empty())
            {
                stack.front()->second.weight = weight;

                stack.pop_front();
            }
        }
        else
            ++q11;
    }

    //
    // Transfer weighted multimap to destination map
    std::map<float, ProjectionInfo> ProjectionList2;

    float sum=0.0f;
    for (q0=multiProjectionList.begin(); q0!=multiProjectionList.end(); q0++)
    {
        ProjectionList2[q0->second.angle]=q0->second;
        sum+=q0->second.weight;
    }


    if (fabs(sum-0.5f)<0.001f)
    {
        for (auto & item : ProjectionList2)
            item.second.weight*=2;
    }

    ProjectionList=ProjectionList2;
    
    return 0;
}

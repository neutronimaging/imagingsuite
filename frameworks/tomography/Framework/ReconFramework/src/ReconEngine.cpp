//<LICENSE>

#include "stdafx.h"

#include "../include/ReconFramework_global.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <algorithm>

#include <logging/logger.h>
#include <base/timage.h>
#include <base/thistogram.h>
#include <strings/miscstring.h>
#include <base/textractor.h>
#include <algorithms/datavalidator.h>
#include <io/io_stack.h>
#include <folders.h>
#ifdef HAVE_NEXUS
    #include <io/io_nexus.h>
#endif

#include <ParameterHandling.h>
#include <ModuleException.h>
#include <publication.h>

#include "../include/ReconException.h"
#include "../include/ReconHelpers.h"
#include "../include/processtiminglogger.h"
#include "../include/ReconEngine.h"


ReconEngine::ReconEngine(std::string name, kipl::interactors::InteractionBase *interactor) :
	logger(name),
    m_Config(""),
	m_FirstSlice(0),
    m_ProjectionMargin(0),
	m_ProjectionReader(interactor),
    m_BackProjector(nullptr),
    nProcessedBlocks(0),
	nProcessedProjections(0),
	nTotalProcessedProjections(0),
	nTotalBlocks(0),
	m_bCancel(false),
	m_Interactor(interactor)
{
    logger(kipl::logging::Logger::LogMessage,"C'tor Recon engine");
    if (m_Interactor!=nullptr) 
    {
		logger(kipl::logging::Logger::LogMessage,"Got an interactor");
	}
	else 
    {
		logger(kipl::logging::Logger::LogMessage,"An interactor was not provided");
	}

    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner"}),
                                       "MuhRec - a new tomography reconstructor",
                                       "Nuclear Instruments and Methods Section A",
                                       2011,
                                       651,
                                       1,
                                       "156-160",
                                       "10.1016/j.nima.2011.01.129"));

    publications.push_back(Publication(std::vector<std::string>({"A.P. Kaestner","C. Carminati"}),
                                       "MuhRec software",
                                       "Zenodo",
                                       2019,
                                       1,
                                       1,
                                       "no pages",
                                       "10.5281/zenodo.1117850"));
}

ReconEngine::~ReconEngine(void)
{
	std::ostringstream msg;
    logger(logger.LogVerbose,"Enter destructor");

    for (auto &module : m_PreprocList)
    {
        msg.str("");
        msg<<"Removing "<<module->GetModule()->ModuleName()<<" from the module list ("<<m_PreprocList.size()<<")";
        logger(kipl::logging::Logger::LogMessage,msg.str());
        if (module!=nullptr)
        {
            delete module;
        }
        msg.str("");
        msg<<"Removed the module ("<<m_PreprocList.size()<<")";
        logger(logger.LogVerbose,msg.str());
    }
    m_PreprocList.clear();

    if (m_BackProjector!=nullptr)
		delete m_BackProjector;
}

void ReconEngine::SetConfig(ReconConfig &config)
{
    std::ostringstream msg;
    config.SanityCheck();

	m_Config=config;

    m_ProjectionMargin = config.ProjectionInfo.nMargin;
    std::string fname,ext;

    msg.str(""); msg<<"set config.roi=["<<m_Config.ProjectionInfo.roi[0]<<","
                <<m_Config.ProjectionInfo.roi[1]<<","
                <<m_Config.ProjectionInfo.roi[2]<<","
                <<m_Config.ProjectionInfo.roi[3]<<"]";
    logger.message(msg.str());

    //kipl::strings::filenames::MakeFileName(m_Config.ProjectionInfo.sFileMask,m_Config.ProjectionInfo.nFirstIndex,fname,ext,'#','0');
    std::map<float, ProjectionInfo> ProjectionList;
    BuildFileList( m_Config, ProjectionList);

    fname = ProjectionList.begin()->second.name;

    msg.str(""); msg<<"Projection file to check size on "<<fname;
    logger(logger.LogMessage,msg.str());

    try 
    {
        m_Config.ProjectionInfo.nDims = m_ProjectionReader.GetImageSize(fname,m_Config.ProjectionInfo.fBinning);
    }
    catch (ReconException &e) {
        logger(logger.LogError,"Failed to get image size while configuring recon engine.");
        throw ReconException(e.what());
    }
    catch (kipl::base::KiplException &e) {
        logger(logger.LogError,"Failed to get image size while configuring recon engine.");
        throw kipl::base::KiplException(e.what());
    }
    catch (exception &e) {
        logger(logger.LogError,"Failed to get image size while configuring recon engine.");
        throw std::runtime_error(e.what());
    }

    // check if I am writing to disk:
    if (m_Config.MatrixInfo.bAutomaticSerialize==true) 
    {

        float res=0.0f;
        if (m_Config.ProjectionInfo.beamgeometry==ReconConfig::cProjections::BeamGeometry_Parallel)
        {
            res = m_Config.ProjectionInfo.fResolution[0];
        }

        if (m_Config.ProjectionInfo.beamgeometry==ReconConfig::cProjections::BeamGeometry_Cone)
        {
            res = m_Config.MatrixInfo.fVoxelSize[0];
        }


        if (m_Config.MatrixInfo.FileType==kipl::io::NeXusfloat)
        {

            // todo here: add the MatrixRoi option
            size_t dims[3];

            if (m_Config.MatrixInfo.bUseROI)
            {
                dims[0] = m_Config.MatrixInfo.roi[2]-m_Config.MatrixInfo.roi[0]+1;
                dims[1] = m_Config.MatrixInfo.roi[3]-m_Config.MatrixInfo.roi[1]+1;
            }
            else
            {
                dims[0] = (m_Config.ProjectionInfo.projection_roi[2]-m_Config.ProjectionInfo.projection_roi[0]);
                dims[1] = dims[0];
            }
            dims[2] =  (m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.roi[1]); // it is not necessarelly the entire dataset

            kipl::base::TImage<float, 3> img;

            std::stringstream str;
            str.str("");
            str<<m_Config.MatrixInfo.sDestinationPath<<m_Config.MatrixInfo.sFileMask;

            try
            {

                kipl::io::PrepareNeXusFileFloat(str.str().c_str(), dims, res, img);

            }
            catch (ReconException &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw ReconException(e.what());
            }
            catch (kipl::base::KiplException &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw kipl::base::KiplException(e.what());
            }
            catch (exception &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw std::runtime_error(e.what());
            }
        }

        if (m_Config.MatrixInfo.FileType==kipl::io::NeXus16bits)
        {

            // todo here: add the MatrixRoi option
            size_t dims[3];

            if (m_Config.MatrixInfo.bUseROI)
            {
                dims[0] = m_Config.MatrixInfo.roi[2]-m_Config.MatrixInfo.roi[0]+1;
                dims[1] = m_Config.MatrixInfo.roi[3]-m_Config.MatrixInfo.roi[1]+1;
            }
            else
            {
                dims[0] = (m_Config.ProjectionInfo.projection_roi[2]-m_Config.ProjectionInfo.projection_roi[0]);
                dims[1] = dims[0];
            }

            dims[2] =  (m_Config.ProjectionInfo.roi[3]-m_Config.ProjectionInfo.roi[1]); // it is not necessarelly the entire dataset

            kipl::base::TImage<float, 3> img;


            std::stringstream str;
            str.str("");
            str<<m_Config.MatrixInfo.sDestinationPath<<m_Config.MatrixInfo.sFileMask;

            try
            {

                kipl::io::PrepareNeXusFile16bit(str.str().c_str(), dims, res, img);

            }
            catch (ReconException &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw ReconException(e.what());
            }
            catch (kipl::base::KiplException &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw kipl::base::KiplException(e.what());
            }
            catch (exception &e)
            {
                logger(logger.LogError,"Failed to PrepareNeXusFile while configuring recon engine.");
                throw std::runtime_error(e.what());
            }
         }

    }
}

size_t ReconEngine::AddPreProcModule(ModuleItem *module)
{
    if (module!=nullptr)
    {
		if (module->Valid())
			m_PreprocList.push_back(module);
	}
	else
		throw ReconException("Failed to add module",__FILE__,__LINE__);

	return m_PreprocList.size();
}

void ReconEngine::SetBackProjector(BackProjItem *module)
{
    if (m_BackProjector!=nullptr)
        delete m_BackProjector;

    if (module!=nullptr)
    {
		if (module->Valid())
			m_BackProjector=module;
	}
	else
		throw ReconException("Failed to add back projector module",__FILE__,__LINE__);
}

int ReconEngine::Run()
{

	std::stringstream msg;

	BuildFileList(m_Config,m_ProjectionList);

	size_t roi[4]={
		m_Config.ProjectionInfo.roi[0],
		m_Config.ProjectionInfo.roi[1],
		m_Config.ProjectionInfo.roi[2],
		m_Config.ProjectionInfo.roi[3]
    };

    size_t voi[6] = {
        m_Config.MatrixInfo.voi[0],
        m_Config.MatrixInfo.voi[1],
        m_Config.MatrixInfo.voi[2],
        m_Config.MatrixInfo.voi[3],
        m_Config.MatrixInfo.voi[4],
        m_Config.MatrixInfo.voi[5]
    };


    size_t totalSlices=0;

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram)
    {
        m_Config.MatrixInfo.nDims[2] = roi[3];
        totalSlices=roi[3];
    }
    else
    {
        m_Config.MatrixInfo.nDims[2] = roi[3]-roi[1]+1;
        totalSlices=roi[3]-roi[1];
    }
    
    m_Volume.resize(m_Config.MatrixInfo.nDims);

	msg.str("");
	msg<<"ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogVerbose,msg.str());

    switch (m_Config.ProjectionInfo.beamgeometry)
    {
        case ReconConfig::cProjections::BeamGeometry_Parallel:
            m_FirstSlice=roi[1];
            break;
        case ReconConfig::cProjections::BeamGeometry_Cone:
            m_FirstSlice=roi[3]-voi[5];
            break;
        case ReconConfig::cProjections::BeamGeometry_Helix:
            logger(logger.LogError,"Helix is not supported by the engine.");
            throw ReconException("Helix is not supported by the engine",__FILE__,__LINE__);
            break;
        default:
            logger(logger.LogError,"Unsupported geometry type.");
            throw ReconException("Unsupported geometry type.",__FILE__,__LINE__);
            break;
    }

	kipl::profile::Timer totalTimer;

	totalTimer.Tic();


	size_t nSliceBlock=GetIntParameter(m_Config.backprojector.parameters,"SliceBlock");
	nTotalBlocks=totalSlices/nSliceBlock;

	msg.str("");
	msg<<"Interval "<<totalSlices
			<<" is divided into "<<nTotalBlocks
			<<" blocks ("<<(totalSlices)
			<<"/"<<nSliceBlock<<")";

	logger(kipl::logging::Logger::LogMessage,msg.str());
	m_bCancel=false;
	int result=0;
    //float radius = (static_cast<float>(m_Config.ProjectionInfo.roi[2])-static_cast<float>(m_Config.ProjectionInfo.roi[0]))/2;


	try {
        for (nProcessedBlocks=0; (nProcessedBlocks<nTotalBlocks) && (m_bCancel==false); nProcessedBlocks++)
        {
            nProcessedProjections=0;
            m_Config.ProjectionInfo.roi[3]=m_Config.ProjectionInfo.roi[1]+nSliceBlock;

            msg.str("");
            msg<<__FUNCTION__<<" Fk "<<nProcessedBlocks<<" ["
                <<m_Config.ProjectionInfo.roi[0]<<", "
                <<m_Config.ProjectionInfo.roi[1]<<", "
                <<m_Config.ProjectionInfo.roi[2]<<", "
                <<m_Config.ProjectionInfo.roi[3]<<"]";
            logger(kipl::logging::Logger::LogMessage,msg.str());

            result=Process(m_Config.ProjectionInfo.roi);
            m_Config.ProjectionInfo.roi[1]=m_Config.ProjectionInfo.roi[3];

		}

        if (totalSlices!=nSliceBlock*nTotalBlocks)
        {
            nProcessedProjections=0;
            m_Config.ProjectionInfo.roi[3]=roi[3];
            msg.str("");
            msg<<__FUNCTION__<<" Processing block "<<nProcessedBlocks<<" ["
                <<m_Config.ProjectionInfo.roi[0]<<", "
                <<m_Config.ProjectionInfo.roi[1]<<", "
                <<m_Config.ProjectionInfo.roi[2]<<", "
                <<m_Config.ProjectionInfo.roi[3]<<"]";
            logger(kipl::logging::Logger::LogMessage,msg.str());

            result=Process(m_Config.ProjectionInfo.roi);
		}
	}
    catch (ReconException &e)
    {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (ModuleException &e)
    {
            msg.str("");
			msg<<"The reconstruction failed with "<<e.what();
			throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

    catch (std::exception &e)
    {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
		msg.str("");
		msg<<"The reconstruction failed with an unknown error";
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}


	totalTimer.Toc();
	msg.str("");
    msg<<"Totals for "<<totalSlices<<" slices"<<std::endl
        <<totalTimer.elapsedTime(kipl::profile::Timer::seconds)<<" ("<<totalTimer.elapsedTime(kipl::profile::Timer::seconds)/static_cast<double>(totalSlices)<<" s/slice)";

	logger(kipl::logging::Logger::LogMessage,msg.str());

	return result;
}

int ReconEngine::Process(const std::vector<size_t>  &roi)
{
	std::stringstream msg;
	m_bCancel=false;
	//status=ReconStatusRunning;
    size_t margin=0;
    auto extroi=roi;

    extroi[1]  = margin<extroi[1] ? extroi[1] : extroi[1]-margin;
    extroi[3]  = margin+extroi[3] < m_Config.ProjectionInfo.nDims[1] ? margin+extroi[3] : extroi[3];


	msg<<"Processing ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
	logger(kipl::logging::Logger::LogMessage,msg.str());
	// Initialize the plugins with the current ROI

    std::string moduleName;
	try {
        for (auto &module : m_PreprocList)
		{
			msg.str("");
            moduleName = module->GetModule()->ModuleName();
            msg<<"Setting ROI for module "<< moduleName;
			logger(kipl::logging::Logger::LogVerbose,msg.str());
            module->GetModule()->SetROI(extroi);
		}
	}
    catch (ReconException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a ReconException for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a KiplException for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg.str("");
        msg<<"SetROI failed with an STL-exception for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
		msg.str("");
        msg<<"SetROI failed with an unknown exception for "<<moduleName;
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

    m_BackProjector->GetModule()->SetROI(roi);

	std::map<std::string, std::string> parameters;

	// Start processing
	kipl::base::TImage<float,2> projection;
	kipl::profile::Timer timer;
    msg.str("");
    msg<<"Allocated preprocessors "<<m_PreprocList.size()<<" using "<<m_BackProjector->GetModule()->Name()<<std::endl
        <<"Arc=["<<m_Config.ProjectionInfo.fScanArc[0]<<", "<<m_Config.ProjectionInfo.fScanArc[1]<<"]"<<std::endl
		<<"Target matrix "<<m_BackProjector->GetModule()->GetVolume();

	logger(kipl::logging::Logger::LogMessage,msg.str());
	msg.str("");

	timer.Tic();
	std::map<float,ProjectionInfo>::iterator it_Proj;
	size_t i=0;
    for (it_Proj=m_ProjectionList.begin();
         (it_Proj!=m_ProjectionList.end()) && (m_bCancel==false) ;
         ++it_Proj, ++i)
	{
		float fWeight=it_Proj->second.weight/(0<m_Config.ProjectionInfo.fResolution[0] ? m_Config.ProjectionInfo.fResolution[0]*0.1f : 1.0f);
		float fAngle=it_Proj->second.angle+m_Config.MatrixInfo.fRotation;

		nProcessedProjections=i;

		projection=m_ProjectionReader.Read(it_Proj->second.name,
				m_Config.ProjectionInfo.eFlip,
				m_Config.ProjectionInfo.eRotate,
				m_Config.ProjectionInfo.fBinning,
                extroi);
		parameters["dose"]=kipl::strings::value2string(
				m_ProjectionReader.GetProjectionDose(it_Proj->second.name,
						m_Config.ProjectionInfo.eFlip,
						m_Config.ProjectionInfo.eRotate,
						m_Config.ProjectionInfo.fBinning,
						m_Config.ProjectionInfo.dose_roi));

		msg.str("");
		msg<<"Block "<<nProcessedBlocks<<", Projection "<<i<<" (weight="<<fWeight<<", angle="<<fAngle<<")";
		logger(kipl::logging::Logger::LogVerbose, msg.str());

        // float moduleCnt=0.0f;
        //float fNumberOfModules=static_cast<float>(m_PreprocList.size());

        for (auto &module : m_PreprocList)
        {
            // ++moduleCnt;

            module->GetModule()->Process(projection,parameters);
		}

		m_BackProjector->GetModule()->Process(projection, fAngle, fWeight,m_ProjectionList.size()<(i+1));
	}

    if (m_bCancel==true)
    {
		logger(kipl::logging::Logger::LogVerbose,"Reconstruction was cancelled by the user.");
		return 1;
	}
    else
    {
		logger(kipl::logging::Logger::LogVerbose,"Reconstruction finished");

        std::vector<size_t> dims;


		if (m_Config.MatrixInfo.bAutomaticSerialize==true)
            Serialize(dims);
        else
        {
			TransferMatrix(roi);
		}
	}

	return 0;
}

bool ReconEngine::TransferMatrix(const std::vector<size_t> &dims)
{
    std::ostringstream msg;
	bool bTransposed=false;


	kipl::base::TImage<float,2> slice;

    try {

        for (size_t i=0; i<(dims[3]-dims[1]); i++)
        {
            slice=m_BackProjector->GetModule()->GetSlice(i);
            float *pVol=m_Volume.GetLinePtr(0,dims[1]-m_FirstSlice+i);
            memcpy(pVol,slice.GetDataPtr(),slice.Size()*sizeof(float));
        }
    }
    catch (ReconException &e)
    {
        msg.str("");
        msg<<"Transfer matrix from backprojector failed with ReconException ("<<m_Config.ProjectionInfo.beamgeometry<<")"<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);

    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Transfer matrix from backprojector failed with KiplException ("<<m_Config.ProjectionInfo.beamgeometry<<")"<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg.str("");
        msg<<"Transfer matrix from backprojector failed stl exception ("<<m_Config.ProjectionInfo.beamgeometry<<")"<<std::endl<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
        msg<<"Transfer matrix from backprojector failed with unhandled exception ("<<m_Config.ProjectionInfo.beamgeometry<<")"<<std::endl;
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

	return bTransposed;
}

bool ReconEngine::Serialize(std::vector<size_t> &dims)
{
	std::stringstream msg;

	std::stringstream str;
	kipl::base::TImage<float,3> img=m_BackProjector->GetModule()->GetVolume();
	img.info.SetMetricX(m_Config.ProjectionInfo.fResolution[0]);
	img.info.SetMetricY(m_Config.ProjectionInfo.fResolution[1]);
	img.info.sArtist=m_Config.UserInformation.sOperator;
	img.info.sCopyright=m_Config.UserInformation.sOperator;
    img.info.sSoftware="MuhRec CT reconstructor";
	img.info.sDescription=m_Config.UserInformation.sSample;

	str.str("");
	str<<m_Config.MatrixInfo.sDestinationPath<<m_Config.MatrixInfo.sFileMask;
	
    CheckFolders(m_Config.MatrixInfo.sDestinationPath,true);
    
	bool bTransposed=false;

    kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY;

    if (m_BackProjector->GetModule()->MatrixAlignment == BackProjectorModuleBase::MatrixZXY)
        plane=kipl::base::ImagePlaneYZ;

    size_t nSlices = m_BackProjector->GetModule()->GetNSlices();

    msg.str("");
    msg<<"Writing dims=["<<img.Size(0)<<", "<<img.Size(1)<<", "<<img.Size(2)
        <<"], plane="<<enum2string(plane);

    logger.message(msg.str());

    if (m_Config.MatrixInfo.FileType==kipl::io::NeXusfloat) 
    {
       size_t nSliceBlock=GetIntParameter(m_Config.backprojector.parameters,"SliceBlock");

       size_t Start = nSliceBlock*nProcessedBlocks;
       if (m_Config.MatrixInfo.bUseROI){
            kipl::io::WriteNeXusStack(img, str.str().c_str(), Start,nSlices, plane, m_Config.MatrixInfo.roi);
       }
       else {
           kipl::io::WriteNeXusStack(img, str.str().c_str(), Start,nSlices, plane, {});
       }

	}
    else if (m_Config.MatrixInfo.FileType==kipl::io::NeXus16bits) 
    {
        size_t nSliceBlock = GetIntParameter(m_Config.backprojector.parameters,"SliceBlock");

        size_t Start = nSliceBlock*nProcessedBlocks;
        if (m_Config.MatrixInfo.bUseROI)
        {
            kipl::io::WriteNeXusStack16bit(img, str.str().c_str(), Start,nSlices, m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1], plane, m_Config.MatrixInfo.roi);
        }
        else 
        {
            kipl::io::WriteNeXusStack16bit(img, str.str().c_str(), Start,nSlices,m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1], plane);
        }

    }
	else 
    {
		msg.str("");
		msg<<"Serializing "<<nSlices<<" slices to "<<m_Config.MatrixInfo.sDestinationPath;
		logger.message(msg.str());
        msg.str("");
        try {
            if (m_Config.MatrixInfo.bUseROI)
            {
                logger.message("Serializing matrix with ROI");
                if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Parallel)
                {
                    kipl::io::WriteImageStack(img,
                        str.str(),
                        m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1],
                        0UL,nSlices-1UL,m_Config.ProjectionInfo.roi[1],
                        m_Config.MatrixInfo.FileType,
                        plane,m_Config.MatrixInfo.roi,
                        m_Config.ProjectionInfo.roi[1]!=m_FirstSlice);
                }
                else if (m_Config.ProjectionInfo.beamgeometry == m_Config.ProjectionInfo.BeamGeometry_Cone)
                {
                    kipl::io::WriteImageStack(img,
                                                  str.str(),
                                                  m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1],
                                                  0UL,nSlices-1UL, CBroi[1], m_Config.MatrixInfo.FileType,plane,m_Config.MatrixInfo.roi);
                }
            }
            else
            {
                logger.message("Serializing full matrix");
                if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Parallel)
                {
                    kipl::io::WriteImageStack(img,
                        str.str(),
                        m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1],
                        0UL,nSlices-1UL,m_Config.ProjectionInfo.roi[1],
                        m_Config.MatrixInfo.FileType,plane,
                        {},
                        m_Config.ProjectionInfo.roi[1]!=m_FirstSlice);
                }
                else if (m_Config.ProjectionInfo.beamgeometry == m_Config.ProjectionInfo.BeamGeometry_Cone)
                {
                    kipl::io::WriteImageStack(img,
                        str.str(),
                        m_Config.MatrixInfo.fGrayInterval[0],m_Config.MatrixInfo.fGrayInterval[1],
                        0UL,nSlices-1UL, CBroi[1], m_Config.MatrixInfo.FileType,plane);
                }
            }
        }
        catch (ReconException & e)
        {
            msg<<"Serializing failed with a ReconException: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
        catch (kipl::base::KiplException & e)
        {
            msg<<"Serializing failed with a KiplException: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
        catch (std::exception & e)
        {
            msg<<"Serializing failed with an STL exception: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
        catch (...)
        {
            throw ReconException("An unhandled exception was thrown.",__FILE__,__LINE__);
        }

	}

    dims = img.dims();

    writePublicationList();
	return bTransposed;
}

kipl::base::TImage<float,2> ReconEngine::GetSlice(size_t index, kipl::base::eImagePlanes plane)
{
	kipl::base::TImage<float,2> img;

    if (m_Volume.Size()!=0UL)
        img=kipl::base::ExtractSlice(m_Volume,index,plane,{});

    return img;
}

std::string ReconEngine::citations()
{
    std::ostringstream cites;

    cites<<"MuhRec\n=============================\n";
    for (const auto &pub : publications)
    {
        cites<<pub<<"\n";
    }
    cites<<"\n";

    cites<<"\nPreprocessing\n=============================\n";
    for (const auto & module : m_PreprocList)
    {
        cites<<module->GetModule()->ModuleName()<<"\n";
        for (const auto &pub : module->GetModule()->publicationList())
        {
            cites<<pub<<"\n";
        }
        cites<<"\n";
    }

    cites<<"\nBack-projector\n=============================\n";
    cites<<m_BackProjector->GetModule()->Name()<<"\n";
    for (const auto &pub : m_BackProjector->GetModule()->publicationList())
    {
        cites<<pub<<"\n";
    }


    return cites.str();
}

std::vector<Publication> ReconEngine::publicationList()
{
    std::vector<Publication> pubList;

    pubList = publications;
    for (const auto & module : m_PreprocList)
    {
        for (const auto &pub : module->GetModule()->publicationList())
        {
            pubList.push_back(pub);
        }
    }

    for (const auto &pub : m_BackProjector->GetModule()->publicationList())
    {
        pubList.push_back(pub);
    }

    return pubList;
}

void ReconEngine::writePublicationList(const std::string &fname)
{
    std::string destName=fname;
    if (fname.empty())
    {
        destName = m_Config.MatrixInfo.sDestinationPath + "citations.txt";
    }

    logger.message(std::string("Writing citations to ")+destName);
    kipl::strings::filenames::CheckPathSlashes(destName,false);
    std::ofstream citefile(destName.c_str());

    citefile<<citations();
}

size_t ReconEngine::GetHistogram(float *axis, size_t *hist, size_t nBins)
{
	m_BackProjector->GetModule()->GetHistogram(axis,hist,nBins);

	return nBins;
}

size_t ReconEngine::GetHistogram(std::vector<float> &axis, std::vector<size_t> &hist, size_t nBins)
{
    if (m_histAxis.size()!=nBins)
        kipl::base::Histogram(m_Volume.GetDataPtr(),m_Volume.Size(),nBins,m_histBins,m_histAxis,0.0f,0.0f,true);

    axis = m_histAxis;
    hist = m_histBins;

	return nBins;
}

bool ReconEngine::Serialize(ReconConfig::cMatrix *matrixconfig)
{

	std::stringstream msg;
	std::stringstream str;

	m_Volume.info.sArtist      = m_Config.UserInformation.sOperator;
	m_Volume.info.sCopyright   = m_Config.UserInformation.sOperator;
    m_Volume.info.sSoftware    = "MuhRec CT reconstructor";
	m_Volume.info.sDescription = m_Config.UserInformation.sSample;

	str.str("");
	str<<matrixconfig->sDestinationPath<<matrixconfig->sFileMask;

    CheckFolders(matrixconfig->sDestinationPath,true);

    writePublicationList(matrixconfig->sDestinationPath+"citations.txt");

	bool bTransposed=false;

    float res=0.0f;
    if (m_Config.ProjectionInfo.beamgeometry==ReconConfig::cProjections::BeamGeometry_Parallel)
    {
        res = m_Config.ProjectionInfo.fResolution[0];
    }

    if (m_Config.ProjectionInfo.beamgeometry==ReconConfig::cProjections::BeamGeometry_Cone)
    {
        res = m_Config.MatrixInfo.fVoxelSize[0];
    }

    m_Volume.info.SetMetricX(res);
    m_Volume.info.SetMetricY(res);

    if (matrixconfig->FileType==kipl::io::NeXusfloat){
       kipl::io::WriteNexusFloat(m_Volume,str.str().c_str(),res);
    }
    else if (matrixconfig->FileType==kipl::io::NeXus16bits){
        kipl::io::WriteNexus16bits(m_Volume, str.str().c_str(),matrixconfig->fGrayInterval[0],matrixconfig->fGrayInterval[1], res);
    }
    else
    {
		kipl::base::eImagePlanes plane=kipl::base::ImagePlaneXY;
		size_t nSlices=0;
		nSlices=m_Volume.Size(2);
		msg.str("");
		msg<<"Serializing "<<nSlices<<" slices to "<<str.str();
		logger(kipl::logging::Logger::LogMessage,msg.str());

		kipl::io::WriteImageStack(m_Volume,
				str.str(),
                matrixconfig->fGrayInterval[0],
                matrixconfig->fGrayInterval[1],
                0,
                nSlices-1,
                m_FirstSlice,
                matrixconfig->FileType,
                plane);
	}


	return bTransposed;
}

int ReconEngine::Run3D(bool bRerunBackproj)
{
    std::stringstream msg;
    m_histAxis.clear();
    m_histBins.clear();
    
    int res=0;
    msg<<"Rerun backproj: "<<(bRerunBackproj ? "true" : "false")<<", status projection blocks "<<(m_ProjectionBlocks.empty() ? "empty" : "has data");

    logger(kipl::logging::Logger::LogMessage,msg.str());
    try {
        kipl::profile::Timer totalTimer;
        totalTimer.reset();
        totalTimer.Tic();

        msg.str(""); msg<<"run3d "<<m_Config.ProjectionInfo.beamgeometry;
        logger.message(msg.str());

        resetTimers();
        if ((bRerunBackproj==true) && (m_ProjectionBlocks.empty()==false))
            res=Run3DBackProjOnly();
        else
            res=Run3DFull();

        totalTimer.Toc();

        std::map<std::string, std::map<std::string,std::string>> timingLogList;
        std::map<std::string,std::string> timingList;
        timingList["total"]=std::to_string(totalTimer.cumulativeTime());

        for (auto &module : m_PreprocList)
        {
            msg<<module->GetModule()->ModuleName()<<": "<<module->GetModule()->execTime()<<"s\n";
            timingList[module->GetModule()->ModuleName()] = std::to_string(module->GetModule()->execTime());
        }
        timingList[m_BackProjector->GetModule()->Name()] = std::to_string(m_BackProjector->GetModule()->execTime());
        timingLogList["timing"]=timingList;

        timingLogList["data"]= {{"projections",std::to_string(m_Config.ProjectionInfo.nLastIndex-m_Config.ProjectionInfo.nFirstIndex)},
                                {"sizeu",std::to_string(m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0])},
                                {"sizev",std::to_string(m_Config.MatrixInfo.nDims[2])}, // Temporary fix
                                {"sizex",std::to_string(m_Config.MatrixInfo.nDims[0])},
                                {"sizey",std::to_string(m_Config.MatrixInfo.nDims[1])},
                                {"sizez",std::to_string(m_Config.MatrixInfo.nDims[2])},
                               };

        timingLogList["system"]= {{"threads",std::to_string(m_Config.System.nMaxThreads)}};
        ProcessTimingLogger ptl(ReconConfig::homePath()+"/.imagingtools/recontiming.json");

        ptl.addLogEntry(timingLogList);
    }
    catch (ReconException &e)
    {
        msg.str("");
        msg<<"Run3D failed with "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"Run3D failed with "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg.str("");
        msg<<"Run3D failed with "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
        msg.str("");
        msg<<"Run3D failed with an unknown error";
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    return res;
}

void ReconEngine::ConfigSanityCheck(ReconConfig &config)
{
    std::ignore = config;
}

int ReconEngine::Run3DFull()
{
	std::stringstream msg;

    logger(kipl::logging::Logger::LogVerbose,"Entering Run3DFull");
    m_ProjectionBlocks.clear();
    auto roi = m_Config.ProjectionInfo.roi;

    CBroi = m_Config.ProjectionInfo.roi;

    size_t totalSlices=0;

    if (m_Config.MatrixInfo.bUseROI)
    {
        m_Config.MatrixInfo.nDims[0] = m_Config.MatrixInfo.roi[2]-m_Config.MatrixInfo.roi[0]+1;
        m_Config.MatrixInfo.nDims[1] = m_Config.MatrixInfo.roi[3]-m_Config.MatrixInfo.roi[1]+1;
    }
    else
    {
        m_Config.MatrixInfo.nDims[0] = roi[2]-roi[0];
        m_Config.MatrixInfo.nDims[1] = m_Config.MatrixInfo.nDims[0];
    }

    if (m_Config.ProjectionInfo.imagetype==ReconConfig::cProjections::ImageType_Proj_RepeatSinogram) {
        m_Config.MatrixInfo.nDims[2] = roi[3];
        totalSlices=roi[3];
    }
    else
    {
        m_Config.MatrixInfo.nDims[2] = roi[3]-roi[1];
    }

    totalSlices=m_Config.MatrixInfo.nDims[2];


	msg.str("");
    if (!m_Config.MatrixInfo.bAutomaticSerialize)
    {
        try
        {
                m_Volume.resize(m_Config.MatrixInfo.nDims);
                m_Volume = 0.0f;
		}
        catch (kipl::base::KiplException &e)
        {
			msg<<"Failed to allocate target matrix with dimensions "
					<<m_Config.MatrixInfo.nDims[0]<<"x"
					<<m_Config.MatrixInfo.nDims[1]<<"x"
					<<m_Config.MatrixInfo.nDims[2]<<"\n"<<e.what();
			throw ReconException(msg.str(),__FILE__,__LINE__);

		}
	}

	msg.str("");
    msg<<": ROI=["<<roi[0]<<" "<<roi[1]<<" "<<roi[2]<<" "<<roi[3]<<"]";
	logger(kipl::logging::Logger::LogVerbose,msg.str());

    m_FirstSlice=roi[1];

	kipl::profile::Timer totalTimer;

	totalTimer.Tic();


	size_t nSliceBlock=GetIntParameter(m_Config.backprojector.parameters,"SliceBlock");
	nTotalBlocks=totalSlices/nSliceBlock;

	msg.str("");
    msg<<": Interval "<<totalSlices
			<<" is divided into "<<nTotalBlocks
			<<" blocks ("<<(totalSlices)
			<<"/"<<nSliceBlock<<")";


	logger(kipl::logging::Logger::LogMessage,msg.str());
	m_bCancel=false;
	int result=0;
    float radius = static_cast<float>(m_Config.ProjectionInfo.roi[2]-m_Config.ProjectionInfo.roi[0])*m_Config.MatrixInfo.fVoxelSize[0]/2;

    msg.str("");

    msg<<"Run3DFull beam geometry: "<<m_Config.ProjectionInfo.beamgeometry;
    logger.message(msg.str());
    try {
        for (nProcessedBlocks=0;
             (nProcessedBlocks<nTotalBlocks) && (!UpdateProgress(static_cast<float>(nProcessedBlocks)/nTotalBlocks, "Blocks"));
             ++nProcessedBlocks)
        {
            msg.str("");
            msg<<"Run3DFull Processing block "<<nProcessedBlocks<<"\n     ROI = ["
                <<m_Config.ProjectionInfo.roi[0]<<", "
                <<m_Config.ProjectionInfo.roi[1]<<", "
                <<m_Config.ProjectionInfo.roi[2]<<", "
                <<m_Config.ProjectionInfo.roi[3]<<"]";

            logger.message(msg.str(),__FUNCTION__);
            if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Cone)
            {
                UpdateProgress(static_cast<float>(nProcessedBlocks)/static_cast<float>(nTotalBlocks),"Starting block");

                std::vector<size_t> CBCT_roi(4,0UL);
                CBCT_roi[0] = m_Config.ProjectionInfo.roi[0];
                CBCT_roi[2] = m_Config.ProjectionInfo.roi[2];

                nProcessedProjections=0;
                m_Config.ProjectionInfo.roi[3]=m_Config.ProjectionInfo.roi[1]+nSliceBlock;


                if (    m_Config.ProjectionInfo.fpPoint[1]>=static_cast<float>(m_Config.ProjectionInfo.roi[1])
                     && m_Config.ProjectionInfo.fpPoint[1]>=static_cast<float>(m_Config.ProjectionInfo.roi[3]))
                {
                    CBCT_roi[3] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]
                            -((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[3]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD+radius))
                            /m_Config.ProjectionInfo.fResolution[0]);

                    float value = m_Config.ProjectionInfo.fpPoint[1]
                            -((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))
                            /m_Config.ProjectionInfo.fResolution[0];

                    if(value<=0)
                        CBCT_roi[1] = 0;
                    else
                        CBCT_roi[1] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]
                                -((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))
                                /m_Config.ProjectionInfo.fResolution[0]);
                }

                if (    m_Config.ProjectionInfo.fpPoint[1] < static_cast<float>(m_Config.ProjectionInfo.roi[1])
                     && m_Config.ProjectionInfo.fpPoint[1] < static_cast<float>(m_Config.ProjectionInfo.roi[3]))
                {
                    float value   = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[1])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD+radius))/m_Config.ProjectionInfo.fResolution[0];
                     CBCT_roi[1]  = static_cast<size_t>(value);
                     float value2 = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[3])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                     if ( value2>=m_Config.ProjectionInfo.projection_roi[3] )
                         CBCT_roi[3] = m_Config.ProjectionInfo.projection_roi[3];
                     else
                         CBCT_roi[3] = static_cast<float>(value2);
                }

               if (    m_Config.ProjectionInfo.fpPoint[1] >= static_cast<float>(m_Config.ProjectionInfo.roi[1])
                    && m_Config.ProjectionInfo.fpPoint[1] <  static_cast<float>(m_Config.ProjectionInfo.roi[3]))
               {
                   float value = m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                   if(value<=0)
                       CBCT_roi[1] = 0;
                   else
                       CBCT_roi[1] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0]);

                   float value2 = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[3])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                   if (value2>=m_Config.ProjectionInfo.projection_roi[3])
                       CBCT_roi[3] = m_Config.ProjectionInfo.projection_roi[3];
                   else
                       CBCT_roi[3] = static_cast<float>(value2);
               }


               if ( (8<CBCT_roi[1]) && (CBCT_roi[1]!=0) )
                   CBCT_roi[1] -=8;
               if ( CBCT_roi[3]+8<=m_Config.ProjectionInfo.projection_roi[3])
                   CBCT_roi[3] +=8;

                msg.str("");
                msg<<"CBCT Processing block "<<nProcessedBlocks<<"\n     ROI = ["
                    <<m_Config.ProjectionInfo.roi[0]<<", "
                    <<m_Config.ProjectionInfo.roi[1]<<", "
                    <<m_Config.ProjectionInfo.roi[2]<<", "
                    <<m_Config.ProjectionInfo.roi[3]<<"] \n"
                    <<"CBCT ROI = ["
                    <<CBCT_roi[0]<<", "
                    <<CBCT_roi[1]<<", "
                    <<CBCT_roi[2]<<", "
                    <<CBCT_roi[3]<<"]\n";
                logger.message(msg.str(),__FUNCTION__);

                CBroi = m_Config.ProjectionInfo.roi;

                result = Process3D(CBCT_roi);
                m_Config.ProjectionInfo.roi[1] = m_Config.ProjectionInfo.roi[3];

            }
            else
            {
                    UpdateProgress(static_cast<float>(nProcessedBlocks)/static_cast<float>(nTotalBlocks),"Starting block");
                    nProcessedProjections=0;
                    m_Config.ProjectionInfo.roi[3]=m_Config.ProjectionInfo.roi[1]+nSliceBlock;

                    msg.str("");
                    msg<<"Processing block "<<nProcessedBlocks<<" ["
                        <<m_Config.ProjectionInfo.roi[0]<<", "
                        <<m_Config.ProjectionInfo.roi[1]<<", "
                        <<m_Config.ProjectionInfo.roi[2]<<", "
                        <<m_Config.ProjectionInfo.roi[3]<<"]";

                    logger.message(msg.str(),__FUNCTION__);

                    result=Process3D(m_Config.ProjectionInfo.roi);
                    m_Config.ProjectionInfo.roi[1]=m_Config.ProjectionInfo.roi[3];
            }
		}

        msg.str(""); msg<<"pre call proc3d "<<m_Config.ProjectionInfo.beamgeometry;
        logger.message(msg.str());
        if ((totalSlices!=nSliceBlock*nTotalBlocks) && !UpdateProgress(1.0f, "Last block"))
        {
            if (m_Config.ProjectionInfo.beamgeometry==m_Config.ProjectionInfo.BeamGeometry_Cone)
            {

                std::vector<size_t> CBCT_roi(4,0UL);
                CBCT_roi[0] = m_Config.ProjectionInfo.roi[0];
                CBCT_roi[2] = m_Config.ProjectionInfo.roi[2];

                nProcessedProjections=0;
                m_Config.ProjectionInfo.roi[3]=roi[3];
                CBroi = m_Config.ProjectionInfo.roi;

                if (m_Config.ProjectionInfo.fpPoint[1]>=static_cast<float>(m_Config.ProjectionInfo.roi[1]) && m_Config.ProjectionInfo.fpPoint[1]>=static_cast<float>(m_Config.ProjectionInfo.roi[3]))
                {
                    CBCT_roi[3] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[3]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD+radius))/m_Config.ProjectionInfo.fResolution[0]);
                    float value = m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                    if(value<=0)
                        CBCT_roi[1] = 0;
                    else
                        CBCT_roi[1] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0]);
                }

                if (m_Config.ProjectionInfo.fpPoint[1]<static_cast<float>(m_Config.ProjectionInfo.roi[1]) && m_Config.ProjectionInfo.fpPoint[1]<static_cast<float>(m_Config.ProjectionInfo.roi[3]))
                {
                    float value = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[1])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD+radius))/m_Config.ProjectionInfo.fResolution[0];
                     CBCT_roi[1] = static_cast<size_t>(value);
                     float value2 = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[3])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                     if (value2>=m_Config.ProjectionInfo.projection_roi[3])
                         CBCT_roi[3] = m_Config.ProjectionInfo.projection_roi[3];
                     else
                         CBCT_roi[3] = static_cast<size_t>(value2);
                }

               if (m_Config.ProjectionInfo.fpPoint[1]>=static_cast<float>(m_Config.ProjectionInfo.roi[1]) && m_Config.ProjectionInfo.fpPoint[1]<static_cast<float>(m_Config.ProjectionInfo.roi[3]))
               {
                   float value = m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                   if(value<=0)
                       CBCT_roi[1] = 0;
                   else
                       CBCT_roi[1] = static_cast<size_t>(m_Config.ProjectionInfo.fpPoint[1]-((m_Config.ProjectionInfo.fpPoint[1]-static_cast<float>(m_Config.ProjectionInfo.roi[1]))*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0]);

                   float value2 = m_Config.ProjectionInfo.fpPoint[1]+((static_cast<float>(m_Config.ProjectionInfo.roi[3])-m_Config.ProjectionInfo.fpPoint[1])*m_Config.MatrixInfo.fVoxelSize[0]*m_Config.ProjectionInfo.fSDD/(m_Config.ProjectionInfo.fSOD-radius))/m_Config.ProjectionInfo.fResolution[0];
                   if (value2>=m_Config.ProjectionInfo.projection_roi[3])
                       CBCT_roi[3] = m_Config.ProjectionInfo.projection_roi[3];
                   else
                       CBCT_roi[3] = static_cast<size_t>(value2);
               }

               if (CBCT_roi[1]>=8 && CBCT_roi[1]!=0)
                   CBCT_roi[1] -=8;
               if (CBCT_roi[3]+8<=m_Config.ProjectionInfo.projection_roi[3])
                   CBCT_roi[3] +=8;


                msg.str("");
                msg<<": Processing block "<<nProcessedBlocks<<" ["
                    <<m_Config.ProjectionInfo.roi[1]<<", "
                    <<m_Config.ProjectionInfo.roi[3]<<"]";
                logger(kipl::logging::Logger::LogMessage,msg.str());

                result=Process3D(CBCT_roi);
            }
            else
            {
                nProcessedProjections=0;
                m_Config.ProjectionInfo.roi[3]=roi[3];
                msg.str("");
                msg<<__FUNCTION__<<" Processing block "<<nProcessedBlocks<<" ["
                    <<m_Config.ProjectionInfo.roi[0]<<", "
                    <<m_Config.ProjectionInfo.roi[1]<<", "
                    <<m_Config.ProjectionInfo.roi[2]<<", "
                    <<m_Config.ProjectionInfo.roi[3]<<"]";

                logger(kipl::logging::Logger::LogMessage,msg.str());
                result=Process3D(m_Config.ProjectionInfo.roi);
            }
		}
	}
	catch (ReconException &e) {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (kipl::base::KiplException &e) {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (std::exception &e) {
		msg.str("");
		msg<<"The reconstruction failed with "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (...) {
		msg.str("");
		msg<<"The reconstruction failed with an unknown error";
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

    if (!UpdateProgress(static_cast<float>(nProcessedBlocks)/nTotalBlocks, "Finished"))
    {
		totalTimer.Toc();
		msg.str("");
        msg<<": Totals for "<<totalSlices<<" slices"<<std::endl
            <<totalTimer.elapsedTime(kipl::profile::Timer::seconds)<<" ("<<totalTimer.elapsedTime(kipl::profile::Timer::seconds)/static_cast<double>(totalSlices)<<" s/slice)";

		logger(kipl::logging::Logger::LogMessage,msg.str());

		msg.str("");
		msg<<"\nModule process time:\n";

		logger(kipl::logging::Logger::LogMessage,msg.str());

        logger(kipl::logging::Logger::LogMessage,"Run 3D done.");
		Done();
	}
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Run 3D was canceled by the user.");
        m_ProjectionBlocks.clear();
	}

	return result;
}

int ReconEngine::Run3DBackProjOnly()
{
    logger(kipl::logging::Logger::LogMessage,"Running Back-projection only");
    m_BackProjector->GetModule()->Configure(m_Config,m_Config.backprojector.parameters);

    m_Volume=0.0f;
    int result=ProcessExistingProjections3D({});

    Done();
    return result;
}

kipl::base::TImage<float,3> ReconEngine::RunPreproc(const std::vector<size_t> & roi, std::string sLastModule)
{
	std::stringstream msg;
	m_bCancel=false;

	std::list<ModuleItem *>::iterator it_Module;
	// Initialize the plug-ins with the current ROI
    std::string moduleName;
    // float moduleCnt=0.0f;
    float fNumberOfModules=static_cast<float>(m_PreprocList.size())+1;
    try
    {
        for (auto &module : m_PreprocList)
        {
            // ++moduleCnt;

            moduleName = module->GetModule()->ModuleName();

			msg.str("");
            msg<<"Setting ROI for module "<< moduleName;
			logger(kipl::logging::Logger::LogVerbose,msg.str());

            module->GetModule()->SetROI(roi);
		}
	}
    catch (ReconException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a ReconException for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a KiplException for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg.str("");
        msg<<"SetROI failed with an STL-exception for "<<moduleName<<"\n"<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
	catch (...) {
		msg.str("");
        msg<<"SetROI failed with an unknown exception for "<<moduleName;
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	std::map<std::string, std::string> parameters;

	// Start processing
	kipl::profile::Timer timer;

	msg<<"Allocated preprocessors "<<m_PreprocList.size()<<" using "<<m_BackProjector->GetModule()->Name()<<"\n"
		<<"Arc=["<<m_Config.ProjectionInfo.fScanArc[0]<<", "<<m_Config.ProjectionInfo.fScanArc[1]<<"]"<<"\n"
		<<"Target matrix "<<m_BackProjector->GetModule()->GetVolume();

	logger(kipl::logging::Logger::LogMessage,msg.str());
	msg.str("");

	timer.Tic();

	kipl::base::TImage<float,3> projections;

	msg.str("");

    try
    {
		projections=m_ProjectionReader.Read(m_Config,roi,parameters);
        validateImage(projections.GetDataPtr(), projections.Size(),"post read RunPreproc");
	}
    catch (ReconException &e)
    {
		msg<<"Reading projections failed with a recon exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
		msg<<"Reading projections failed with a kipl exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg<<"Reading projections failed with a STL exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	logger(kipl::logging::Logger::LogMessage,"Starting preprocessing");

    // TODO: Check the progress bar
    // float progress = CurrentOverallProgress();
    float progressIncrement = 1.0f/(fNumberOfModules*nTotalBlocks);

    try
    {
        for (auto &module : m_PreprocList)
        {
            // ++moduleCnt;

            if (module->GetModule()->ModuleName()==sLastModule)
                break;

			msg.str("");
            msg<<"Processing: "<<module->GetModule()->ModuleName();
            // progress += progressIncrement;
			logger(kipl::logging::Logger::LogMessage,msg.str());
            if (!(m_bCancel=UpdateProgress(progressIncrement, msg.str())))
            {
                module->GetModule()->Process(projections,parameters);
            }
			else
				break;

            validateImage(projections.GetDataPtr(), projections.Size(),module->GetModule()->ModuleName());
		}
	}
    catch (ReconException &e)
    {
		msg<<"Preprocessing failed with a recon exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
		msg<<"Preprocessing failed with a kipl exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg<<"Preprocessing failed with an STL exception: "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

	return projections;
}

int ReconEngine::Process3D(const std::vector<size_t> &roi)
{
	std::stringstream msg;
	m_bCancel=false;

    msg<<": Processing ROI in 3D mode ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
	logger(kipl::logging::Logger::LogMessage,msg.str());
    auto extroi=roi;

    if (m_Config.ProjectionInfo.beamgeometry!=m_Config.ProjectionInfo.BeamGeometry_Cone)
    {
        if (m_ProjectionMargin<=roi[1])
            extroi[1]-=m_ProjectionMargin;

        extroi[3]  = m_ProjectionMargin+extroi[3] < m_Config.ProjectionInfo.nDims[1] ? m_ProjectionMargin+extroi[3] : extroi[3];
    }

    msg.str("");
    msg<<": Processing ext ROI ["<<extroi[0]<<", "<<extroi[1]<<", "<<extroi[2]<<", "<<extroi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

	// Initialize the plug-ins with the current ROI
    std::string moduleName;


    try
    {
		msg.str("");
        msg<<": Number of pre proc modules:"<<m_PreprocList.size();
		logger(kipl::logging::Logger::LogMessage,msg.str());
        for (auto &module : m_PreprocList)
		{
            moduleName = module->GetModule()->ModuleName();
			msg.str("");
            msg<<": Setting ROI for module "<< moduleName;

			logger(kipl::logging::Logger::LogMessage,msg.str());

            module->GetModule()->SetROI(extroi);
			logger(kipl::logging::Logger::LogMessage,"ROI set");
		}
	}
    catch (ReconException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a ReconException for "<<moduleName<<"\n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (ModuleException &e)
    {
			msg.str("");
            msg<<"SetROI failed with a ModuleException for "<<moduleName<<"\n"<<e.what();
			logger(kipl::logging::Logger::LogError,msg.str());
			throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
		msg.str("");
        msg<<"SetROI failed with a KiplException for "<<moduleName<<"\n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
		msg.str("");
        msg<<"SetROI failed with an STL-exception for "<<moduleName<<"\n"<<e.what();
		logger(kipl::logging::Logger::LogError,msg.str());
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
		msg.str("");
        msg<<"SetROI failed with an unknown exception for "<<moduleName;
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}

    try
    {
        // add switch on beam geometry
        switch (m_Config.ProjectionInfo.beamgeometry)
        {
            case ReconConfig::cProjections::BeamGeometry_Parallel:
                m_BackProjector->GetModule()->SetROI(roi);
                break;
            case ReconConfig::cProjections::BeamGeometry_Cone:
                m_BackProjector->GetModule()->SetROI(CBroi);
                break;
            case ReconConfig::cProjections::BeamGeometry_Helix:
                logger(logger.LogError,"Helix is not supported by the engine.");
                throw ReconException("Helix is not supported by the engine",__FILE__,__LINE__);
                break;
            default:
                logger(logger.LogError,"Unsupported geometry type.");
                throw ReconException("Unsupported geometry type.",__FILE__,__LINE__);
                break;
        }

    }
    catch (ReconException &e)
    {
        msg.str("");
        msg<<"SetROI failed with a ReconException for "<<m_BackProjector->GetModule()->Name()<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (ModuleException &e)
    {
            msg.str("");
            msg<<"SetROI failed with a ModuleException for "<<m_BackProjector->GetModule()->Name()<<"\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
        msg.str("");
        msg<<"SetROI failed with a KiplException for "<<m_BackProjector->GetModule()->Name()<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg.str("");
        msg<<"SetROI failed with an STL-exception for "<<m_BackProjector->GetModule()->Name()<<"\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (...)
    {
        msg.str("");
        msg<<"SetROI failed with an unknown exception for "<<m_BackProjector->GetModule()->Name();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }


	std::map<std::string, std::string> parameters;

	// Start processing
	kipl::profile::Timer timer;
    msg.str("");
    msg<<": Allocated preprocessors "<<m_PreprocList.size()<<" using "<<m_BackProjector->GetModule()->Name()<<"\n"
		<<"Arc=["<<m_Config.ProjectionInfo.fScanArc[0]<<", "<<m_Config.ProjectionInfo.fScanArc[1]<<"]"<<"\n"
		<<"Target matrix "<<m_BackProjector->GetModule()->GetVolume();

	logger(kipl::logging::Logger::LogMessage,msg.str());

	timer.Tic();

    kipl::base::TImage<float,3> ext_projections;
    msg.str("");
    msg<<": Reading Projections. ROI=["<<extroi[0]<<", "<<extroi[1]<<", "<<extroi[2]<<", "<<extroi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

	msg.str("");


    try
    {
        ext_projections=m_ProjectionReader.Read(m_Config,extroi,parameters);
        validateImage(ext_projections.GetDataPtr(),ext_projections.Size(),"post reader");
	}
    catch (ReconException &e)
    {
        msg<<"Reading projections failed with a recon exception: "<<std::endl<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
        msg<<"Reading projections failed with a kipl exception: "<<std::endl<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
        msg<<"Reading projections failed with a STL exception: "<<std::endl<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
        msg<<"Reading projections failed with an unsupported exception: ";
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

    msg.str("");
    msg<<": Size of read projections using ext roi: "<<ext_projections;
    logger(logger.LogMessage,msg.str());

	logger(kipl::logging::Logger::LogMessage,"Starting preprocessing");

    float progress = CurrentOverallProgress();
    float fNumberOfModules=static_cast<float>(m_PreprocList.size())+1;
    float progressIncrement = 1/(fNumberOfModules*nTotalBlocks);
    try
    {
        for (auto &module : m_PreprocList)
        {
            moduleName = module->GetModule()->ModuleName();

			msg.str("");
            msg<<"Processing: "<< moduleName;
            progress += progressIncrement;
			logger(kipl::logging::Logger::LogMessage,msg.str());
            if (!(m_bCancel=UpdateProgress(progress, msg.str())))
                module->GetModule()->Process(ext_projections,parameters);
			else
				break;
            validateImage(ext_projections.GetDataPtr(),ext_projections.Size(),moduleName);
		}
	}
    catch (ReconException &e)
    {
        msg<<"Preprocessing failed with a recon exception in "<<moduleName<<": "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (kipl::base::KiplException &e)
    {
        msg<<"Preprocessing failed with a kipl exception in "<<moduleName<<": "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (std::exception &e)
    {
        msg<<"Preprocessing failed with an STL exception in "<<moduleName<<": "<<e.what();
		throw ReconException(msg.str(),__FILE__,__LINE__);
	}
    catch (...)
    {
        msg<<"Preprocessing failed with an unsupported exception in "<<moduleName<<". ";
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
	
    kipl::base::TImage<float,3> projections;


    if (m_ProjectionMargin!=0)
    { // Remove padding
        std::vector<size_t> dims = {    ext_projections.Size(0),
                                        ext_projections.Size(1)-(roi[1]!=extroi[1] ? m_ProjectionMargin : 0) - (roi[3]!=extroi[3] ? m_ProjectionMargin : 0),
                                        ext_projections.Size(2)};

        projections.resize(dims);

        msg.str("");
        msg<<"ext: "<<ext_projections<<", proj: "<<projections;
        logger(logger.LogMessage,msg.str());
        for (size_t i=0; i<projections.Size(2); i++)
            memcpy(projections.GetLinePtr(0,i),ext_projections.GetLinePtr((roi[1]!=extroi[1] ? m_ProjectionMargin : 0),i), projections.Size(0) * projections.Size(1) * sizeof(float));
    }
    else
    {
        projections=ext_projections;
    }

    if (m_Config.MatrixInfo.bAutomaticSerialize==false) // Don't store the projections for the reconstruction to disk case
    {
//        m_ProjectionBlocks.push_back(ProjectionBlock(projections,roi,parameters));
        switch (m_Config.ProjectionInfo.beamgeometry)
        {
            case ReconConfig::cProjections::BeamGeometry_Parallel:
                m_ProjectionBlocks.push_back(ProjectionBlock(projections,roi,parameters));
                break;
            case ReconConfig::cProjections::BeamGeometry_Cone:

                m_ProjectionBlocks.push_back(ProjectionBlock(projections,CBroi,parameters));
                break;
            case ReconConfig::cProjections::BeamGeometry_Helix:
                logger(logger.LogError,"Helix is not supported by the engine.");
                throw ReconException("Helix is not supported by the engine",__FILE__,__LINE__);
            default:
                logger(logger.LogError,"Unsupported geometry type.");
                throw ReconException("Unsupported geometry type.",__FILE__,__LINE__);
        }
    }

    int res=0;

    try
    {
        switch (m_Config.ProjectionInfo.beamgeometry)
        {
            case ReconConfig::cProjections::BeamGeometry_Parallel:
                BackProject3D(projections,roi,parameters);
                break;
            case ReconConfig::cProjections::BeamGeometry_Cone:
                BackProject3D(projections,CBroi,parameters);
                break;
            case ReconConfig::cProjections::BeamGeometry_Helix:
                logger(logger.LogError,"Helix is not supported by the engine.");
                throw ReconException("Helix is not supported by the engine",__FILE__,__LINE__);
            default:
                logger(logger.LogError,"Unsupported geometry type.");
                throw ReconException("Unsupported geometry type.",__FILE__,__LINE__);
        }
    }
    catch (ReconException &e)
    {
        msg<<"BackProject3D failed with a recon exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"BackProject3D failed with a kipl exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg<<"BackProject3D failed with an STL exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }

	logger(kipl::logging::Logger::LogVerbose,"Done process 3D.");

    return res;
}

int ReconEngine::ProcessExistingProjections3D(const std::vector<size_t> &roi)
{
    std::ignore = roi;
    std::stringstream msg;
    std::list<ProjectionBlock>::iterator it;
    int i=0;
    int res=0;

    try
    {
        for (it=m_ProjectionBlocks.begin(); it!=m_ProjectionBlocks.end(); ++it, ++i)
        {
            msg.str("");
            msg<<"Back-projecting projection block "<<i+1;
            logger(kipl::logging::Logger::LogMessage,msg.str());
            m_BackProjector->GetModule()->SetROI(it->roi);
            UpdateProgress(static_cast<float>(i)/static_cast<float>(m_ProjectionBlocks.size()),"Block start");

            res=BackProject3D(it->projections,it->roi,it->parameters);
            validateImage(it->projections.GetDataPtr(),it->projections.Size(),"Projections post recon block ProcessExistingProjections3D");
        }
    }
    catch (ReconException &e)
    {
        msg<<"BackProjection of preprocessed block "<<i<<" failed with a recon exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"BackProjection of preprocessed block "<<i<<" failed with a kipl exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }
    catch (std::exception &e)
    {
        msg<<"BackProjection of preprocessed block "<<i<<" failed with an STL exception: "<<e.what();
        throw ReconException(msg.str(),__FILE__,__LINE__);
    }


    return res;
}

int ReconEngine::BackProject3D(kipl::base::TImage<float,3> & projections,
                               const std::vector<size_t> &roi,
                               std::map<std::string, std::string> parameters)
{
    std::stringstream msg;

    msg<<"Got projections:"<<projections<<" with ROI=["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    msg.str("");
    m_BackProjector->GetModule()->SetROI(roi);

    if (!UpdateProgress(CurrentOverallProgress(), "Back projection"))
    {
        try {
            logger(kipl::logging::Logger::LogMessage,"Back projection started.");
            m_BackProjector->GetModule()->Process(projections,parameters);
            logger(kipl::logging::Logger::LogMessage,"Back projection done.");
        }
        catch (ReconException &e) {
            msg<<"Back-projection failed with a recon exception: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
        catch (kipl::base::KiplException &e) {
            msg<<"Back-projection failed with a kipl exception: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
        catch (std::exception &e) {
            msg<<"Back-projection failed with an STL exception: "<<e.what();
            throw ReconException(msg.str(),__FILE__,__LINE__);
        }
    }

    validateImage(projections.GetDataPtr(),projections.Size(),"Projections post recon BackProject3D");

    if (UpdateProgress(1.0f, "Finalizing"))
    {
        logger(kipl::logging::Logger::LogMessage,"Reconstruction was canceled by the user.");
        return 1;
    }
    else
    {
        logger(kipl::logging::Logger::LogMessage,"Reconstruction finished");



        if (m_Config.MatrixInfo.bAutomaticSerialize==true)
        {
            std::vector<size_t> dims;
            Serialize(dims);
        }
        else
        {
            TransferMatrix(roi);
        }
    }

    return 0;
}

bool ReconEngine::UpdateProgress(float val, std::string msg)
{
    std::ignore = msg;
    if (m_Interactor!=nullptr)
    {
        return m_Interactor->SetOverallProgress(val);
    }

    return false;
}

float ReconEngine::CurrentOverallProgress()
{
    float progress=0.0f;
    if (m_Interactor!=nullptr)
    {
        progress=m_Interactor->CurrentOverallProgress();
    }
    return progress;
}

size_t ReconEngine::validateImage(float *data, size_t N, const string &description)
{
    size_t cnt=0;

    if (m_Config.System.bValidateData)
    {
        std::ostringstream msg;
        size_t cntInf=0;
        size_t cntNan=0;

        cnt=kipl::algorithms::dataValidator(data,N,cntNan,cntInf);
        if (cnt!=0)
        {
            msg.str(""); msg<<"Encountered "<<cntInf<<" infs and "<<cntNan<<" NaNs in "<<description;
            logger.warning(msg.str());
        }
    }
    return cnt;
}

void ReconEngine::Done()
{
    if (m_Interactor!=nullptr)
        m_Interactor->Done();
}

void ReconEngine::resetTimers()
{
    m_BackProjector->GetModule()->resetTimer();
    for (auto & module : m_PreprocList)
    {
        module->GetModule()->resetTimer();
    }

}

void ReconEngine::MakeExtendedROI(size_t *roi, size_t margin, size_t *extroi, size_t *margins)
{
    std::ignore = roi;
    std::ignore = margin;
    std::ignore = extroi;
    std::ignore = margins;
}

void ReconEngine::UnpadProjections(kipl::base::TImage<float,3> &projections, size_t *roi, size_t *margins)
{
    std::ignore = roi;
    std::ignore = projections;
    std::ignore = margins;
}

//==========================================
// ProjectionBlock

ProjectionBlock::ProjectionBlock()
{

}

ProjectionBlock::ProjectionBlock(kipl::base::TImage<float,3> & proj, const std::vector<size_t> &r, std::map<std::string, std::string> pars) :
    projections(proj),
    roi(r),
    parameters(pars)
{
    projections.Clone();
}

ProjectionBlock::ProjectionBlock(const ProjectionBlock &b):
    projections(b.projections),
    roi(b.roi),
    parameters(b.parameters)
{
    projections.Clone();
}

ProjectionBlock & ProjectionBlock::operator=(const ProjectionBlock &b)
{
    projections=b.projections;
    projections.Clone();

    parameters=b.parameters;

    roi = b.roi;

    return *this;
}


ProjectionBlock::~ProjectionBlock()
{
}


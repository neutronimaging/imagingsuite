//<LICENSE>

#include "configuregeometrydialog.h"
#include "ui_configuregeometrydialog.h"

#include <exception>
#include <numeric>
#include <algorithm>

#include <QMessageBox>
#include <QSignalBlocker>

#include <ProjectionReader.h>
#include <ReconException.h>
#include <ReconHelpers.h>
#include <base/KiplException.h>
#include <strings/filenames.h>
#include <filters/medianfilter.h>
#include <math/mathconstants.h>
#include <math/linfit.h>
#include <base/tsubimage.h>
#include <base/tpermuteimage.h>
#include <morphology/morphfilters.h>

#include <tomocenter.h>

ConfigureGeometryDialog::ConfigureGeometryDialog(QWidget *parent) :
    QDialog(parent),
    logger("ConfigGeometryDialog"),
    ui(new Ui::ConfigureGeometryDialog),
    m_Config(""),
    fraction(0.9f)
{
    ui->setupUi(this);
    on_groupUseTilt_toggled(false);
}

ConfigureGeometryDialog::~ConfigureGeometryDialog()
{
    delete ui;
}

int ConfigureGeometryDialog::exec(ReconConfig &config)
{
    std::ostringstream msg;

    m_Config=config;
    int res=LoadImages();

    if (res<0)
        return QDialog::Rejected;

    ui->viewerProjection->set_image(m_Proj0Deg.GetDataPtr(),m_Proj0Deg.dims());
    ui->viewerProjection->set_levels(kipl::base::quantile99);
    ui->groupBoxSlices->setVisible(false);
    ui->comboROISelection->setCurrentIndex(0);

    QSignalBlocker firstSlice(ui->spinSliceFirst);
    QSignalBlocker lastSlice(ui->spinSliceLast);

    ui->spinSliceFirst->setMinimum(m_Config.ProjectionInfo.projection_roi[1]);
    ui->spinSliceLast->setMaximum(m_Config.ProjectionInfo.projection_roi[3]);

    UpdateDialog();

    return QDialog::exec();


}

void ConfigureGeometryDialog::GetConfig(ReconConfig & config)
{
    UpdateConfig();
    config=m_Config;
}

void ConfigureGeometryDialog::onROIButtonClicked()
{
    QRect rect=ui->viewerProjection->get_marked_roi();

    if (rect.width()*rect.height()!=0)
    {
        ui->spinSliceFirst->blockSignals(true);
        ui->spinSliceLast->blockSignals(true);
        ui->spinSliceFirst->setValue(rect.y());
        ui->spinSliceLast->setValue(rect.y()+rect.height());
        ui->spinSliceFirst->blockSignals(false);
        ui->spinSliceLast->blockSignals(false);
        ROIChanged(-1,-1);
    }
}

void ConfigureGeometryDialog::ROIChanged(int y0, int y1)
{
    QRect rect;

    auto dims=m_Config.ProjectionInfo.roi;
    if (0<=y0)
    {
        rect.setCoords(static_cast<int>(dims[0]),
                       y0,
                       static_cast<int>(dims[2]),
                       y1);
    }
    else
    {
        rect.setCoords( static_cast<int>(dims[0]),
                        static_cast<int>(dims[1]),
                        static_cast<int>(dims[2]),
                        static_cast<int>(dims[3]));
    }

    on_comboROISelection_currentIndexChanged(ui->comboROISelection->currentIndex());
}

kipl::base::TImage<float,2> ConfigureGeometryDialog::ThresholdProjection(const kipl::base::TImage<float,2> img, float level)
{
    kipl::base::TImage<float,2> result(img.dims());

    float const * const pImg=img.GetDataPtr();
    float *pRes=result.GetDataPtr();

    for (size_t i=0; i<img.Size(); i++)
    {
        pRes[i]=static_cast<float>(level<pImg[i]);
    }

    return result;
}

float ConfigureGeometryDialog::CorrelationCenter(	kipl::base::TImage<float,2> proj_0,
                                                    kipl::base::TImage<float,2> proj_180,
                                                    size_t *roi)
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using correlation");

    msg<<"Corr center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

    std::ofstream cogfile("cog_corr.txt");

    kipl::base::TImage<float,2> limg0,limg180;

    std::vector<size_t> start  = {roi[0],roi[1]};
    std::vector<size_t> length = {roi[2]-roi[0],roi[3]-roi[1]};

    kipl::base::TSubImage<float,2> cropper;
    limg0=cropper.Get(proj_0,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(m_Proj180Deg,start,length),kipl::base::ImageAxisX);

    size_t len=limg0.Size(0)/3;

    kipl::base::TImage<float,2> corrimg({len*2,limg0.Size(1)});

    for (size_t y=0; y<limg0.Size(1); y++) {
        float       *       corr = corrimg.GetLinePtr(y);
        const float * const p0   = limg0.GetLinePtr(y)+len;
        const float * const p180 = limg180.GetLinePtr(y);

        for (size_t idx=0; idx<2*len; idx++ )
        {
            corr[idx]=0.0f;
            for (size_t x=0; x<len; x++)
            {
                corr[idx]+=p0[x]*p180[idx+x];
            }
        }
        size_t pos=0;
        for (size_t i=1; i<2*len; i++)
            if (corr[pos]<corr[i]) pos=i;
        float value=(static_cast<float>(pos)-static_cast<float>(len))*0.5f+(static_cast<float>(limg0.Size(0))*0.5f);

        cogfile<<y<<" "<<len<<" "<<limg0.Size(0)<<" "<<pos<<" "<<value<<std::endl;
        m_vCoG.push_back(value);
    }

    return 0.0f;
}

float ConfigureGeometryDialog::LeastSquareCenter(	kipl::base::TImage<float,2> proj_0,
                                                    kipl::base::TImage<float,2> proj_180, size_t *roi)
{
    std::ostringstream msg;
    logger(kipl::logging::Logger::LogMessage,"Center estimation using least squared error");

  //  size_t *roi = m_Config.ProjectionInfo.roi;
    msg<<"LS center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());
    kipl::base::TImage<float,2> limg0,limg180;
    std::vector<size_t> start  = {roi[0],roi[1]};
    std::vector<size_t> length = {roi[2]-roi[0],roi[3]-roi[1]};
    kipl::base::TSubImage<float,2> cropper;

    msg.str("");
    msg<<"x ["<<start[0]<<", "<<length[0]<<"], y ["<<start[1]<<", "<<length[1]<<"]";

    logger(kipl::logging::Logger::LogMessage,msg.str());
    limg0=cropper.Get(proj_0,start,length);
    limg180 = kipl::base::Mirror(cropper.Get(proj_180,start,length),kipl::base::ImageAxisX);

    ofstream cogfile("cog_lsc.txt");

    size_t len=limg0.Size(0)/3;

    kipl::base::TImage<float,2> corrimg({len*2,limg0.Size(1)});
    float diff=0.0f;
    for (size_t y=0; y<limg0.Size(1); y++)
    {
        float *corr=corrimg.GetLinePtr(y);
        const float * const p0   = limg0.GetLinePtr(y)+len;
        const float * const p180 = limg180.GetLinePtr(y);

        for (size_t idx=0; idx<2*len; idx++ )
        {
            corr[idx]=0.0f;
            for (size_t x=0; x<len; x++)
            {
                diff=p0[x]-p180[idx+x];
                corr[idx]+=diff*diff;
            }
        }
        size_t pos=0;
        for (size_t i=1; i<2*len; i++)
            if (corr[i]<corr[pos]) pos=i;
        float value=(static_cast<float>(len)-static_cast<float>(pos))*0.5f+(static_cast<float>(limg0.Size(0))*0.5f);

        cogfile<<y<<" "<<len<<" "<<limg0.Size(0)<<" "<<pos<<" "<<value<<std::endl;

        m_vCoG.push_back(value);
    }

    return 0.0f;
}

float ConfigureGeometryDialog::CenterOfGravity(const kipl::base::TImage<float,2> img, size_t start, size_t end)
{
    m_vCoG.clear();
    auto roi = m_Config.ProjectionInfo.roi;
    for (size_t y=roi[1]; y<roi[3]; y++) {
        double sum=0.0;
        double cogsum=0.0;
        float const * const pLine=img.GetLinePtr(y);
        for (size_t x=start; x<end; x++)
        {
            cogsum+=pLine[x]*(x-start);
            sum+=pLine[x];
        }
        if (sum!=0)
        {
            m_vCoG.push_back(static_cast<float>(start+cogsum/sum));
        }
    }
    return 0.0f;
}

void ConfigureGeometryDialog::CumulateProjection(const kipl::base::TImage<float,2> img, const kipl::base::TImage<float,2> biimg)
{

    float const * const pImg=img.GetDataPtr();
    float *pCumImg=m_grayCumulate.GetDataPtr();

    for (size_t i=0; i<img.Size(); i++) {
        pCumImg[i]+=pImg[i];
    }

    std::vector<float> kernel(9,1.0f);
    std::vector<size_t> dims={3,3};
    kipl::morphology::TErode<float,2> erode(kernel,dims);

    float const * const pBiImg=biimg.GetDataPtr();
    pCumImg=m_biCumulate.GetDataPtr();
    for (size_t i=0; i<img.Size(); i++)
    {
        pCumImg[i]+=pBiImg[i];
    }
}

pair<size_t, size_t> ConfigureGeometryDialog::FindBoundary(const kipl::base::TImage<float,2> img, float level)
{
    float *profile=new float[img.Size(0)];
    memset(profile,0, sizeof(float)*img.Size(0));

    for (size_t y=0; y<img.Size(1); y++)
    {
        float const * const pLine=img.GetLinePtr(y);
        for (size_t x=0; x<img.Size(0); x++) {
            profile[x]+=pLine[x];
        }
    }
    pair<size_t, size_t> b(0,img.Size(0)-1);

    float th=level*(img.Size(1)+1);
    for ( ; b.first<b.second; b.first++)
        if (th<profile[b.first])
            break;

    for ( ; b.first<b.second; b.second--)
        if (th<profile[b.second])
            break;

    delete [] profile;
    return b;
}

pair<size_t, size_t> ConfigureGeometryDialog::FindMaxBoundary()
{
    vector<pair<size_t, size_t> >::iterator it=m_vBoundary.begin();

    pair<size_t, size_t> b=*it;

    for (it=m_vBoundary.begin() ; it!=m_vBoundary.end(); it++) {
        b.first=min(b.first,it->first);
        b.second=max(b.second,it->second);
    }
    return b;
}

kipl::base::TImage<float,2> ConfigureGeometryDialog::LoadImage(const std::string & fileMask, size_t idx, const std::string name)
{
   kipl::base::TImage<float,2> img;

//   try {
//           size_t found = fileMask.find("hdf");

//           if (found==std::string::npos)
//           {
//               m_ProjOB=reader.Read("",
//               fileMask,
//               idx,
//               m_Config.ProjectionInfo.eFlip,
//               m_Config.ProjectionInfo.eRotate,
//               m_Config.ProjectionInfo.fBinning,
//               {});
//           }
//           else
//           {
//               m_ProjOB = reader.ReadNexus(fileMask,
//                                  idx,
//                                  m_Config.ProjectionInfo.eFlip,
//                                  m_Config.ProjectionInfo.eRotate,
//                                  m_Config.ProjectionInfo.fBinning,
//                                  {});
//           }
//       }
//   }
//   catch (ReconException & e)
//   {
//       msg<<"Failed to load the"<< name <<" image (ReconException): "<<e.what();
//       logger(kipl::logging::Logger::LogError,msg.str());
//       loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
//       loaderror_dlg.exec();
//   }
//   catch (kipl::base::KiplException & e)
//   {
//       msg<<"Failed to load the "<<name<<" image (kipl): "<<e.what();
//       logger(kipl::logging::Logger::LogError,msg.str());
//       loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
//       loaderror_dlg.exec();
//   }
//   catch (std::exception & e)
//   {
//       msg<<"Failed to load the "<<name<<" image (STL): "<<e.what();
//       logger(kipl::logging::Logger::LogError,msg.str());
//       loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
//       loaderror_dlg.exec();
//   }
//   catch (...)
//   {
//       msg<<"Failed to load the "<<name<<" image (unknown exception): ";
//       logger(kipl::logging::Logger::LogError,msg.str());
//       loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
//       loaderror_dlg.exec();
//   }

   return img;
}

int ConfigureGeometryDialog::LoadImages()
{
    kipl::base::TImage<float,2> img;
    std::ostringstream msg;

    ProjectionReader reader;

    kipl::filters::TMedianFilter<float,2> medfilt({3,3});

    msg.str("");
    QMessageBox loaderror_dlg;

    loaderror_dlg.setWindowTitle("Error");
    loaderror_dlg.setText("Failed to load images.");

    m_Proj0Deg       = kipl::base::TImage<float,2>();
    m_Proj180Deg     = kipl::base::TImage<float,2>();
    m_ProjOB         = kipl::base::TImage<float,2>();
    m_ProjDC         = kipl::base::TImage<float,2>();

    // Load references

    try {
        if (m_Config.ProjectionInfo.nOBCount!=0)
        {
            size_t found = m_Config.ProjectionInfo.sOBFileMask.find("hdf");

            if (found==std::string::npos)
            {
                m_ProjOB=reader.Read(m_Config.ProjectionInfo.sReferencePath,
                m_Config.ProjectionInfo.sOBFileMask,
                m_Config.ProjectionInfo.nOBFirstIndex,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                {});
            }
            else
            {
                m_ProjOB = reader.ReadNexus(m_Config.ProjectionInfo.sOBFileMask,
                                   m_Config.ProjectionInfo.nOBFirstIndex,
                                   m_Config.ProjectionInfo.eFlip,
                                   m_Config.ProjectionInfo.eRotate,
                                   m_Config.ProjectionInfo.fBinning,
                                   {});
            }
        }
    }
    catch (ReconException & e)
    {
        msg<<"Failed to load the open beam image (ReconException): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (kipl::base::KiplException & e)
    {
        msg<<"Failed to load the open beam image (kipl): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (std::exception & e)
    {
        msg<<"Failed to load the open beam image (STL): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (...)
    {
        msg<<"Failed to load the open beam image (unknown exception): ";
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }

    try {
        if (m_Config.ProjectionInfo.nDCCount!=0)
        {
            size_t found = m_Config.ProjectionInfo.sDCFileMask.find("hdf");

            if (found==std::string::npos)
            {
                m_ProjDC=reader.Read(m_Config.ProjectionInfo.sReferencePath,
                m_Config.ProjectionInfo.sDCFileMask,
                m_Config.ProjectionInfo.nDCFirstIndex,
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                {});
            }
            else
            {
                m_ProjDC = reader.ReadNexus(m_Config.ProjectionInfo.sDCFileMask,
                                   m_Config.ProjectionInfo.nDCFirstIndex,
                                   m_Config.ProjectionInfo.eFlip,
                                   m_Config.ProjectionInfo.eRotate,
                                   m_Config.ProjectionInfo.fBinning,
                                   {});
            }
        }

    }
    catch (ReconException & e)
    {
        msg<<"Failed to load the open beam image (ReconException): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (kipl::base::KiplException & e)
    {
        msg<<"Failed to load the open beam image (kipl): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (std::exception & e)
    {
        msg<<"Failed to load the open beam image (STL): "<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }
    catch (...)
    {
        msg<<"Failed to load the open beam image (unknown exception): ";
        logger(kipl::logging::Logger::LogError,msg.str());
        loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
        loaderror_dlg.exec();
        return -1;
    }

    if ( kipl::base::checkEqualSize(m_ProjDC, m_ProjOB) )
    {
        for (size_t i=0; i<m_ProjOB.Size(); ++i)
        {
            m_ProjOB[i]-=m_ProjDC[i];
            if (m_ProjOB[i]<1)
                m_ProjOB[i]=1;
        }
    }
    else
    {

    }

    size_t found = m_Config.ProjectionInfo.sFileMask.find("hdf");

    if (found==std::string::npos)
    {

        std::map<float,ProjectionInfo> projlist;
        BuildFileList(&m_Config,&projlist);


        std::map<float,ProjectionInfo>::iterator it,marked;

        try
        {
            marked=projlist.begin();
            float diff=abs(marked->first);
            for (it=projlist.begin(); it!=projlist.end(); it++)
            {
                if (abs(it->first)<diff)
                {
                    marked=it;
                    diff=abs(marked->first);
                }
            }
            msg.str("");
            msg<<"Found first projection at "<<marked->second.angle<<", name: "<<marked->second.name<<", weight="<<marked->second.weight;
            logger(kipl::logging::Logger::LogMessage,msg.str());



            m_Proj0Deg=reader.Read(marked->second.
                    name,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    {} );

            if ( !kipl::base::checkEqualSize(m_ProjDC,m_Proj0Deg) )
            {
                m_ProjDC.resize(m_Proj0Deg.dims());
                std::fill_n(m_ProjDC.GetDataPtr(),0.0f,m_ProjDC.Size());
            }

            if (m_Config.ProjectionInfo.nOBCount!=0)
            {
                if (m_Proj0Deg.Size()==m_ProjOB.Size())
                {
                    float *pProj=m_Proj0Deg.GetDataPtr();
                    float *pOB=m_ProjOB.GetDataPtr();
                    float *pDC=m_ProjDC.GetDataPtr();

                    for (size_t i=0; i<m_Proj0Deg.Size(); i++)
                    {
                        pProj[i]=(pProj[i]-pDC[i])/(pOB[i]-pDC[i]);

                        if ( std::isfinite(pProj[i]) && 0.0f<pProj[i] )
                            pProj[i] = -log(pProj[i]);
                        else
                            pProj[i] = 0.0f;
                    }
                }
                else
                {
                    logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection at 0 deg.");
                }
            }
            else
            {
                logger(kipl::logging::Logger::LogWarning,"No open beam images were provided, using raw projection.");
            }
            m_Proj0Deg=medfilt(m_Proj0Deg);

        }
        catch (ReconException & e)
        {
            msg<<"Failed to load the first projection (ReconException): "<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }
        catch (kipl::base::KiplException &e)
        {
            msg.str("");
            msg<<"Failed loading projection at 0 degrees: \n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }
        catch (std::exception & e)
        {
            msg.str("");
            msg<<"Failed loading projection at 0 degrees:\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }

        try {
            marked=projlist.begin();
            float diff=abs(180.0f-marked->first);
            for (it=projlist.begin(); it!=projlist.end(); it++)
            {
                if (abs(180.0f-it->first)<diff)
                {
                    marked=it;
                    diff=abs(180.0f-marked->first);
                }
            }
            msg.str("");
            msg<<"Found opposite projection at "<<marked->second.angle<<", name: "<<marked->second.name<<", weight="<<marked->second.weight;
            logger(kipl::logging::Logger::LogMessage,msg.str());

            m_Proj180Deg=reader.Read(marked->second.name,
                    m_Config.ProjectionInfo.eFlip,
                    m_Config.ProjectionInfo.eRotate,
                    m_Config.ProjectionInfo.fBinning,
                    {});

            if (m_Config.ProjectionInfo.nOBCount!=0)
            {
                if (m_Proj180Deg.Size()==m_ProjOB.Size())
                {
                    float *pProj = m_Proj180Deg.GetDataPtr();
                    float *pOB   = m_ProjOB.GetDataPtr();
                    float *pDC   = m_ProjDC.GetDataPtr();
                    for (size_t i=0; i<m_Proj180Deg.Size(); i++)
                    {
                        pProj[i]=(pProj[i]-pDC[i])/(pOB[i]-pDC[i]);

                        if ( std::isfinite(pProj[i]) && 0.0f<pProj[i] )
                            pProj[i] = -log(pProj[i]);
                        else
                            pProj[i] = 0.0f;
                    }
                }
                else
                {
                    logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection at 180deg");
                }
            }
            else
            {
                logger(kipl::logging::Logger::LogWarning,"No open beam images were provided, using raw projection.");
            }

            m_Proj180Deg=medfilt(m_Proj180Deg);
        }
        catch (ReconException & e)
        {
            msg<<"Failed to load projection 180 degrees (ReconException): "<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }
        catch (kipl::base::KiplException &e)
        {
            msg.str("");
            msg<<"Failed loading projection at 180 degrees:\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }
        catch (std::exception & e)
        {
            msg.str("");
            msg<<"Failed loading projection at 180 degrees:\n"<<e.what();
            logger(kipl::logging::Logger::LogError,msg.str());
            loaderror_dlg.setDetailedText(QString::fromStdString(msg.str()));
            loaderror_dlg.exec();
            return -1;
        }
    }
    else
    { // read the hfd file

        m_Proj0Deg=reader.ReadNexus(m_Config.ProjectionInfo.sFileMask,
                m_Config.ProjectionInfo.nFirstIndex, // to be automatized
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                {});
        if (m_Config.ProjectionInfo.nOBCount!=0)
        {
            if (m_Proj0Deg.Size()==m_ProjOB.Size())
            {
                float *pProj=m_Proj0Deg.GetDataPtr();
                float *pOB=m_ProjOB.GetDataPtr();

                for (size_t i=0; i<m_Proj0Deg.Size(); i++)
                {
                    pProj[i]=pProj[i]/pOB[i];

                    if ( std::isfinite(pProj[i]) && (0.0f<pProj[i]) )
                        pProj[i] = -log(pProj[i]);
                    else
                        pProj[i] = 0.0f;
                }
            }
            else
            {
                logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection");
            }
        }
        else
        {
            logger(kipl::logging::Logger::LogWarning,"No open beam images were provided, using raw projection.");
        }
        m_Proj0Deg=medfilt(m_Proj0Deg);

        int proj_180 = static_cast<int>(180.0f/((m_Config.ProjectionInfo.fScanArc[1]-m_Config.ProjectionInfo.fScanArc[0])/(static_cast<float>(m_Config.ProjectionInfo.nLastIndex)-static_cast<float>(m_Config.ProjectionInfo.nFirstIndex))));

        m_Proj180Deg=reader.ReadNexus(m_Config.ProjectionInfo.sFileMask,
                proj_180, // to be automatized ASAP
                m_Config.ProjectionInfo.eFlip,
                m_Config.ProjectionInfo.eRotate,
                m_Config.ProjectionInfo.fBinning,
                {});

        if (m_Config.ProjectionInfo.nOBCount!=0)
        {
            if (m_Proj180Deg.Size()==m_ProjOB.Size())
            {
                float *pProj = m_Proj180Deg.GetDataPtr();
                float *pOB   = m_ProjOB.GetDataPtr();
                for (size_t i=0; i<m_Proj180Deg.Size(); i++)
                {
                    pProj[i]=pProj[i]/pOB[i];

                    if ( std::isfinite(pProj[i]) && (0.0f<pProj[i]) )
                        pProj[i] = -log(pProj[i]);
                    else
                        pProj[i] = 0.0f;

                }
            }
            else
            {
                logger(kipl::logging::Logger::LogWarning,"Open beam image does not have the same size as the projection");
            }
        }
        else
        {
            logger(kipl::logging::Logger::LogWarning,"No open beam images were provided, using raw projection.");
        }

        m_Proj180Deg=medfilt(m_Proj180Deg);

    }

    return 0;
}

void ConfigureGeometryDialog::UpdateConfig()
{
    m_Config.ProjectionInfo.roi[1]       = static_cast<size_t>(ui->spinSliceFirst->value());
    m_Config.ProjectionInfo.roi[3]       = static_cast<size_t>(ui->spinSliceLast->value());
    m_Config.ProjectionInfo.fScanArc[0]  = static_cast<float>(ui->dspinAngleFirst->value());
    m_Config.ProjectionInfo.fScanArc[1]  = static_cast<float>(ui->dspinAngleLast->value());
    m_Config.ProjectionInfo.fCenter      = static_cast<float>(ui->dspinCenterRotation->value());
    m_Config.ProjectionInfo.bCorrectTilt = ui->groupUseTilt->isChecked();
    if (ui->groupUseTilt->isChecked())
    {
        m_Config.ProjectionInfo.fTiltAngle         = static_cast<float>(ui->dspinTiltAngle->value());
        m_Config.ProjectionInfo.fTiltPivotPosition = static_cast<int>(ui->dspinTiltPivot->value());
    }
    fraction = static_cast<float>(ui->spinPercentage->value())/100.0f;
}

void ConfigureGeometryDialog::UpdateDialog()
{
    std::ostringstream msg;

    msg<<"["<<m_Config.ProjectionInfo.projection_roi[0]<<", "
         <<m_Config.ProjectionInfo.projection_roi[1]<<", "
         <<m_Config.ProjectionInfo.projection_roi[2]<<", "
         <<m_Config.ProjectionInfo.projection_roi[3]<<"]";


    ui->label_ProjROI->setText(QString::fromStdString(msg.str()));

    ui->spinSliceFirst       ->blockSignals(true);
    ui->spinSliceLast        ->blockSignals(true);
    ui->groupUseTilt         ->setChecked(m_Config.ProjectionInfo.bCorrectTilt);
    ui->spinSliceFirst       ->setValue(static_cast<int>(m_Config.ProjectionInfo.roi[1]));
    ui->spinSliceLast        ->setMinimum(static_cast<int>(m_Config.ProjectionInfo.roi[1]+1));
    ui->spinSliceLast        ->setValue(static_cast<int>(m_Config.ProjectionInfo.roi[3]));
    ui->spinSliceFirst       ->setMaximum(static_cast<int>(m_Config.ProjectionInfo.roi[3]-1));
    ui->dspinAngleFirst      ->setValue(static_cast<double>(m_Config.ProjectionInfo.fScanArc[0]));
    ui->dspinAngleLast       ->setValue(static_cast<double>(m_Config.ProjectionInfo.fScanArc[1]));
    ui->dspinCenterRotation  ->setValue(static_cast<double>(m_Config.ProjectionInfo.fCenter));
    ui->dspinTiltAngle       ->setValue(static_cast<double>(m_Config.ProjectionInfo.fTiltAngle));
    ui->dspinTiltPivot       ->setValue(static_cast<double>(m_Config.ProjectionInfo.fTiltPivotPosition));
    ui->comboScanType        ->setCurrentIndex(static_cast<int>(m_Config.ProjectionInfo.scantype));

    ROIChanged(-1,-1);
    on_groupUseTilt_toggled(m_Config.ProjectionInfo.bCorrectTilt);
    ui->spinSliceFirst->blockSignals(false);
    ui->spinSliceLast->blockSignals(false);
}

void ConfigureGeometryDialog::on_dspinCenterRotation_valueChanged(double arg1)
{

}

void ConfigureGeometryDialog::on_comboROISelection_currentIndexChanged(int index)
{
    QRect rect;
    QString color;
    switch (index)
    {
       case 0 : rect.setCoords(m_Config.ProjectionInfo.projection_roi[0],
                m_Config.ProjectionInfo.projection_roi[1],
                m_Config.ProjectionInfo.projection_roi[2],
                m_Config.ProjectionInfo.projection_roi[3]);
                ui->groupBoxSlices->setVisible(false);
                color="orange";
                break;
       case 1: rect.setCoords(m_Config.ProjectionInfo.projection_roi[0],
                ui->spinSliceFirst->value(),
                m_Config.ProjectionInfo.projection_roi[2],
                ui->spinSliceLast->value());
                ui->groupBoxSlices->setVisible(true);
                color="deepskyblue";
                break;
    }

    ui->viewerProjection->set_rectangle(rect,QColor(color),1);
}

void ConfigureGeometryDialog::on_groupUseTilt_toggled(bool arg1)
{

}

void ConfigureGeometryDialog::on_spinSliceFirst_valueChanged(int arg1)
{
    ui->spinSliceLast->setMinimum(arg1+1);
    ROIChanged(arg1,ui->spinSliceLast->value());
    UpdateConfig();

}

void ConfigureGeometryDialog::on_spinSliceLast_valueChanged(int arg1)
{
    ui->spinSliceFirst->setMaximum(arg1-1);
    ROIChanged(arg1,ui->spinSliceFirst->value());

    UpdateConfig();
}

void ConfigureGeometryDialog::on_buttonFindCenter_clicked()
{
    ostringstream msg;
    UpdateConfig();

    std::vector<size_t> sub_roi = {  m_Config.ProjectionInfo.projection_roi[0],
                                     static_cast<size_t>(ui->spinSliceFirst->value()),
                                     m_Config.ProjectionInfo.projection_roi[2],
                                     static_cast<size_t>(ui->spinSliceLast->value())};

    auto roi=ui->comboROISelection->currentIndex() == 0 ? m_Config.ProjectionInfo.projection_roi : sub_roi;
    msg.str("");
    msg<<"Find center: Current ROI ["<<roi[0]<<", "<<roi[1]<<", "<<roi[2]<<", "<<roi[3]<<"]";
    logger(kipl::logging::Logger::LogMessage,msg.str());

    float fMin,fMax;
    ui->viewerProjection->get_levels(&fMin,&fMax);
    ui->viewerProjection->set_image(m_Proj0Deg.GetDataPtr(),m_Proj0Deg.dims(),fMin, fMax);



    QVector<QPointF> plot_data;

    ImagingAlgorithms::TomoCenter tomoCenter;

    tomoCenter.setFraction(fraction);

    tomoCenter.estimate(    m_Proj0Deg,
                            m_Proj180Deg,
                            static_cast<ImagingAlgorithms::TomoCenter::eEstimator>(ui->comboEstimatationMethod->currentIndex()),
                            roi,
                            ui->groupUseTilt->isChecked(),
                            m_Config.ProjectionInfo.fCenter,
                            m_Config.ProjectionInfo.fTiltAngle,
                            m_Config.ProjectionInfo.fTiltPivotPosition);


//    msg.str("");
//    msg<<"Estimated center="<<m<<", tilt="<<k<<", N="<<N<<", fraction="<<fraction<<std::endl;
//    logger(kipl::logging::Logger::LogMessage,msg.str());
    plot_data.clear();
    plot_data.append(QPointF(tomoCenter.center(0.0)+roi[0],0.0));
    plot_data.append(QPointF(tomoCenter.center(m_Proj0Deg.Size(1)-1.0)+roi[0],m_Proj0Deg.Size(1)-1.0));
    ui->viewerProjection->set_plot(plot_data,QColor("lightblue"),1);
    ui->labelR2->setText(QString::number(tomoCenter.R2(),'g',3));

    UpdateDialog();

    m_vCoG=tomoCenter.centers();
    plot_data.clear();
    //for (size_t i=0; i<m_vCoG.size(); i++)
    int i=0;
    for (auto &cog : m_vCoG)
    {
        plot_data.append(QPointF(cog+roi[0],static_cast<double>(i+roi[1])));
        ++i;
    }

    ui->viewerProjection->set_plot(plot_data,QColor("red"),0);
}

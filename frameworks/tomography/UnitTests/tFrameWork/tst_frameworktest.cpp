#include <QString>
#include <QtTest>

#include <thread>
#include <chrono>

#include <base/timage.h>
#include <base/trotate.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <ProjectionReader.h>
#include <ReconHelpers.h>
#include <ReconException.h>
#include <processtiminglogger.h>


class FrameWorkTest : public QObject
{
    Q_OBJECT

public:
    FrameWorkTest();
    std::vector<float> goldenAngles(int n, int start, float arc);
    std::vector<float> invGoldenAngles(int n, int start, float arc);

private Q_SLOTS:
    void testConfigConstructor();
    void testConfigCopyConstructor();
    void testConfigAssignment();
    void testProjectionReader();
    void testScanTypeEnum();
    void testBuildFileList_GeneratedSequence();
    void testBuildFileList_GeneratedGolden();
    void testBuildFileList_GeneratedGolden_offset();
    void testBuildFileList_GeneratedGolden_delay();
    void testBuildFileList_GeneratedInvGolden();
    void testBuildFileList_GeneratedInvGolden_delay();
    void testBuildFileList();
    void testProcessTimingLogger();


private:
    kipl::base::TImage<unsigned short,2> m_img;
    kipl::base::TImage<float,2> m_fimg;
};

FrameWorkTest::FrameWorkTest()
{
    std::vector<size_t> dims={15, 20};

    m_img.resize(dims);
    m_fimg.resize(dims);

    for (size_t i=0; i<m_img.Size(); i++) {
        m_img[i]  = static_cast<int>(i);
        m_fimg[i] = static_cast<float>(i);
    }

    kipl::io::WriteFITS(m_img,"proj_0001.fits");
    kipl::io::WriteTIFF(m_img,"proj_0002.tif");
}

std::vector<float> FrameWorkTest::goldenAngles(int n, int start, float arc)
{
   std::vector<float> gv;
   float phi=0.5f*(1.0f+sqrt(5.0f));

   for (int i=0; i<n; ++i)
       gv.push_back(static_cast<float>(fmod((i+start)*phi*arc,180)));

   return gv;
}

std::vector<float> FrameWorkTest::invGoldenAngles(int n, int start, float arc)
{
   std::vector<float> gv;
   float phi=0.5f*(1.0f+sqrt(5.0f))-1;

   for (int i=0; i<n; ++i)
       gv.push_back(static_cast<float>(fmod((i+start)*phi*arc,180)));

   return gv;
}

void FrameWorkTest::testConfigConstructor()
{
    ReconConfig config("");

    QCOMPARE(config.System.nMemory,       1500UL);
    QCOMPARE(config.System.eLogLevel,     kipl::logging::Logger::LogMessage);
    QCOMPARE(config.System.bValidateData, false);

    QCOMPARE(config.ProjectionInfo.nDims.size(),       2UL);
    QCOMPARE(config.ProjectionInfo.nDims[0],           2048UL);
    QCOMPARE(config.ProjectionInfo.nDims[1],           2048UL);

    QCOMPARE(config.ProjectionInfo.beamgeometry,       config.ProjectionInfo.BeamGeometry_Parallel);
    QCOMPARE(config.ProjectionInfo.fResolution.size(), 2UL);
    QCOMPARE(config.ProjectionInfo.fBinning,           1.0f);
    QCOMPARE(config.ProjectionInfo.nMargin,            2UL);
    QCOMPARE(config.ProjectionInfo.nFirstIndex,        1UL);
    QCOMPARE(config.ProjectionInfo.nLastIndex,         625UL);
    QCOMPARE(config.ProjectionInfo.nProjectionStep,    1UL);
    QCOMPARE(config.ProjectionInfo.nlSkipList.size(),  0UL);
    QCOMPARE(config.ProjectionInfo.bRepeatLine,        false);
    QCOMPARE(config.ProjectionInfo.scantype,           config.ProjectionInfo.SequentialScan);
    QCOMPARE(config.ProjectionInfo.nGoldenStartIdx,    0UL);
    QCOMPARE(config.ProjectionInfo.imagetype,          config.ProjectionInfo.ImageType_Projections);
    QCOMPARE(config.ProjectionInfo.fCenter,            1024.0f);
    QCOMPARE(config.ProjectionInfo.fSOD,               100.0f);
    QCOMPARE(config.ProjectionInfo.fSDD,               100.0f);
    QCOMPARE(config.ProjectionInfo.fpPoint.size(),     2UL);

    QCOMPARE(config.ProjectionInfo.bTranslate,         false);
    QCOMPARE(config.ProjectionInfo.fTiltAngle,         0.0f);
    QCOMPARE(config.ProjectionInfo.fTiltPivotPosition, 0.0f);
    QCOMPARE(config.ProjectionInfo.bCorrectTilt,       false);

    QCOMPARE(config.ProjectionInfo.sFileMask,          "proj_####.fits");
    QCOMPARE(config.ProjectionInfo.sPath,              "");

    QCOMPARE(config.ProjectionInfo.sReferencePath,     "");
    QCOMPARE(config.ProjectionInfo.sOBFileMask,        "ob_####.fits");
    QCOMPARE(config.ProjectionInfo.nOBFirstIndex,      1UL);
    QCOMPARE(config.ProjectionInfo.nOBCount,           5UL);
    QCOMPARE(config.ProjectionInfo.sDCFileMask,        "dc_####.fits");
    QCOMPARE(config.ProjectionInfo.nDCFirstIndex,      1UL);
    QCOMPARE(config.ProjectionInfo.nDCCount,           5UL);

    QCOMPARE(config.ProjectionInfo.roi.size(),         4UL);
    QCOMPARE(config.ProjectionInfo.roi[0],             0UL);
    QCOMPARE(config.ProjectionInfo.roi[1],             0UL);
    QCOMPARE(config.ProjectionInfo.roi[2],             2047UL);
    QCOMPARE(config.ProjectionInfo.roi[3],             2047UL);

    QCOMPARE(config.ProjectionInfo.projection_roi.size(), 4UL);
    QCOMPARE(config.ProjectionInfo.projection_roi[0],  0UL);
    QCOMPARE(config.ProjectionInfo.projection_roi[1],  0UL);
    QCOMPARE(config.ProjectionInfo.projection_roi[2],  2047UL);
    QCOMPARE(config.ProjectionInfo.projection_roi[3],  2047UL);

    QCOMPARE(config.ProjectionInfo.dose_roi.size(),    4UL);
    QCOMPARE(config.ProjectionInfo.dose_roi[0],        0UL);
    QCOMPARE(config.ProjectionInfo.dose_roi[1],        0UL);
    QCOMPARE(config.ProjectionInfo.dose_roi[2],        10UL);
    QCOMPARE(config.ProjectionInfo.dose_roi[3],        10UL);

    QCOMPARE(config.ProjectionInfo.fScanArc.size(),    2UL);
    QCOMPARE(config.ProjectionInfo.fScanArc[0],        0.0f);
    QCOMPARE(config.ProjectionInfo.fScanArc[1],        360.0f);

    QCOMPARE(config.ProjectionInfo.eFlip,              kipl::base::ImageFlipNone);
    QCOMPARE(config.ProjectionInfo.eRotate,            kipl::base::ImageRotateNone);

    QCOMPARE(config.ProjectionInfo.eDirection,         kipl::base::RotationDirCW);

}

void FrameWorkTest::testConfigCopyConstructor()
{
    ReconConfig config("");

    config.System.nMemory =       15001UL;
    config.System.eLogLevel =     kipl::logging::Logger::LogError;
    config.System.bValidateData =  false;

    config.ProjectionInfo.nDims[0] =           2047UL;
    config.ProjectionInfo.nDims[1] =           2046UL;

    config.ProjectionInfo.beamgeometry =       config.ProjectionInfo.BeamGeometry_Cone;
    config.ProjectionInfo.fResolution = {0.123f,1.23f};
    config.ProjectionInfo.fBinning =           1.0f;
    config.ProjectionInfo.nMargin =            5UL;
    config.ProjectionInfo.nFirstIndex =         3UL;
    config.ProjectionInfo.nLastIndex =         25UL;
    config.ProjectionInfo.nProjectionStep =     3UL;
    config.ProjectionInfo.nlSkipList = {1,2,3};

    config.ProjectionInfo.bRepeatLine =        true;
    config.ProjectionInfo.scantype =           config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx =    10UL;
    config.ProjectionInfo.imagetype =          config.ProjectionInfo.ImageType_Sinograms;
    config.ProjectionInfo.fCenter =            10.0f;
    config.ProjectionInfo.fSOD =               10.0f;
    config.ProjectionInfo.fSDD =               1000.0f;
    config.ProjectionInfo.fpPoint = {123.4f, 456.6f};

    config.ProjectionInfo.bTranslate =         true;
    config.ProjectionInfo.fTiltAngle =         0.1f;
    config.ProjectionInfo.fTiltPivotPosition= 1.0f;
    config.ProjectionInfo.bCorrectTilt=       true;

    config.ProjectionInfo.sFileMask =          "projs_###.fits";
    config.ProjectionInfo.sPath =              "./";

    config.ProjectionInfo.sReferencePath =     "/home/";
    config.ProjectionInfo.sOBFileMask =        "obs_###.fits";
    config.ProjectionInfo.nOBFirstIndex =      10UL;
    config.ProjectionInfo.nOBCount =           50UL;
    config.ProjectionInfo.sDCFileMask =        "dcs_##.fits";
    config.ProjectionInfo.nDCFirstIndex =      2UL;
    config.ProjectionInfo.nDCCount =           25UL;

    config.ProjectionInfo.roi[0] =             1UL;
    config.ProjectionInfo.roi[1] =             2UL;
    config.ProjectionInfo.roi[2] =             3UL;
    config.ProjectionInfo.roi[3] =             4UL;

    config.ProjectionInfo.projection_roi[0] =  5UL;
    config.ProjectionInfo.projection_roi[1] =  6UL;
    config.ProjectionInfo.projection_roi[2] =  7UL;
    config.ProjectionInfo.projection_roi[3] =  8UL;

    config.ProjectionInfo.dose_roi[0] =        9UL;
    config.ProjectionInfo.dose_roi[1] =        10UL;
    config.ProjectionInfo.dose_roi[2] =        110UL;
    config.ProjectionInfo.dose_roi[3] =        120UL;

    config.ProjectionInfo.fScanArc[0] =        10.0f;
    config.ProjectionInfo.fScanArc[1] =        36.0f;

    config.ProjectionInfo.eFlip =              kipl::base::ImageFlipVertical;
    config.ProjectionInfo.eRotate =            kipl::base::ImageRotate180;

    config.ProjectionInfo.eDirection =         kipl::base::RotationDirCCW;

    ReconConfig newconfig(config);
    QCOMPARE(newconfig.System.nMemory,                    config.System.nMemory);
    QCOMPARE(newconfig.System.eLogLevel,                  config.System.eLogLevel);
    QCOMPARE(newconfig.System.bValidateData,              config.System.bValidateData);

    QCOMPARE(newconfig.ProjectionInfo.nDims.size(),       config.ProjectionInfo.nDims.size());
    QCOMPARE(newconfig.ProjectionInfo.nDims[0],           config.ProjectionInfo.nDims[0]);
    QCOMPARE(newconfig.ProjectionInfo.nDims[1],           config.ProjectionInfo.nDims[1]);

    QCOMPARE(newconfig.ProjectionInfo.beamgeometry,       config.ProjectionInfo.beamgeometry);
    QCOMPARE(newconfig.ProjectionInfo.fResolution.size(), config.ProjectionInfo.fResolution.size());
    QCOMPARE(newconfig.ProjectionInfo.fBinning,           config.ProjectionInfo.fBinning);
    QCOMPARE(newconfig.ProjectionInfo.nMargin,            config.ProjectionInfo.nMargin);
    QCOMPARE(newconfig.ProjectionInfo.nFirstIndex,        config.ProjectionInfo.nFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nLastIndex,         config.ProjectionInfo.nLastIndex);
    QCOMPARE(newconfig.ProjectionInfo.nProjectionStep,    config.ProjectionInfo.nProjectionStep);
    QCOMPARE(newconfig.ProjectionInfo.nlSkipList.size(),  config.ProjectionInfo.nlSkipList.size());
    QCOMPARE(newconfig.ProjectionInfo.bRepeatLine,        config.ProjectionInfo.bRepeatLine);
    QCOMPARE(newconfig.ProjectionInfo.scantype,           config.ProjectionInfo.scantype);
    QCOMPARE(newconfig.ProjectionInfo.nGoldenStartIdx,    config.ProjectionInfo.nGoldenStartIdx);
    QCOMPARE(newconfig.ProjectionInfo.imagetype,          config.ProjectionInfo.imagetype);
    QCOMPARE(newconfig.ProjectionInfo.fCenter,            config.ProjectionInfo.fCenter);
    QCOMPARE(newconfig.ProjectionInfo.fSOD,               config.ProjectionInfo.fSOD);
    QCOMPARE(newconfig.ProjectionInfo.fSDD,               config.ProjectionInfo.fSDD);
    QCOMPARE(newconfig.ProjectionInfo.fpPoint.size(),     config.ProjectionInfo.fpPoint.size());

    QCOMPARE(newconfig.ProjectionInfo.bTranslate,         config.ProjectionInfo.bTranslate);
    QCOMPARE(newconfig.ProjectionInfo.fTiltAngle,         config.ProjectionInfo.fTiltAngle);
    QCOMPARE(newconfig.ProjectionInfo.fTiltPivotPosition, config.ProjectionInfo.fTiltPivotPosition);
    QCOMPARE(newconfig.ProjectionInfo.bCorrectTilt,       config.ProjectionInfo.bCorrectTilt);

    QCOMPARE(newconfig.ProjectionInfo.sFileMask,          config.ProjectionInfo.sFileMask);
    QCOMPARE(newconfig.ProjectionInfo.sPath,              config.ProjectionInfo.sPath);

    QCOMPARE(newconfig.ProjectionInfo.sReferencePath,     config.ProjectionInfo.sReferencePath);
    QCOMPARE(newconfig.ProjectionInfo.sOBFileMask,        config.ProjectionInfo.sOBFileMask);
    QCOMPARE(newconfig.ProjectionInfo.nOBFirstIndex,      config.ProjectionInfo.nOBFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nOBCount,           config.ProjectionInfo.nOBCount);
    QCOMPARE(newconfig.ProjectionInfo.sDCFileMask,        config.ProjectionInfo.sDCFileMask);
    QCOMPARE(newconfig.ProjectionInfo.nDCFirstIndex,      config.ProjectionInfo.nDCFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nDCCount,           config.ProjectionInfo.nDCCount);

    QCOMPARE(newconfig.ProjectionInfo.roi.size(),         config.ProjectionInfo.roi.size());
    QCOMPARE(newconfig.ProjectionInfo.roi[0],             config.ProjectionInfo.roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.roi[1],             config.ProjectionInfo.roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.roi[2],             config.ProjectionInfo.roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.roi[3],             config.ProjectionInfo.roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.projection_roi.size(), config.ProjectionInfo.projection_roi.size());
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[0],  config.ProjectionInfo.projection_roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[1],  config.ProjectionInfo.projection_roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[2],  config.ProjectionInfo.projection_roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[3],  config.ProjectionInfo.projection_roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.dose_roi.size(),    config.ProjectionInfo.dose_roi.size());
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[0],        config.ProjectionInfo.dose_roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[1],        config.ProjectionInfo.dose_roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[2],        config.ProjectionInfo.dose_roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[3],        config.ProjectionInfo.dose_roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.fScanArc.size(),    config.ProjectionInfo.fScanArc.size());
    QCOMPARE(newconfig.ProjectionInfo.fScanArc[0],        config.ProjectionInfo.fScanArc[0]);
    QCOMPARE(newconfig.ProjectionInfo.fScanArc[1],        config.ProjectionInfo.fScanArc[1]);

    QCOMPARE(newconfig.ProjectionInfo.eFlip,              config.ProjectionInfo.eFlip);
    QCOMPARE(newconfig.ProjectionInfo.eRotate,            config.ProjectionInfo.eRotate);

    QCOMPARE(newconfig.ProjectionInfo.eDirection,         config.ProjectionInfo.eDirection);
}

void FrameWorkTest::testConfigAssignment()
{
    ReconConfig config("");

    config.System.nMemory =       15001UL;
    config.System.eLogLevel =     kipl::logging::Logger::LogError;
    config.System.bValidateData =  false;

    config.ProjectionInfo.nDims[0] =           2047UL;
    config.ProjectionInfo.nDims[1] =           2046UL;

    config.ProjectionInfo.beamgeometry =       config.ProjectionInfo.BeamGeometry_Cone;
    config.ProjectionInfo.fResolution = {0.123f,1.23f};
    config.ProjectionInfo.fBinning =           1.0f;
    config.ProjectionInfo.nMargin =            5UL;
    config.ProjectionInfo.nFirstIndex =         3UL;
    config.ProjectionInfo.nLastIndex =         25UL;
    config.ProjectionInfo.nProjectionStep =     3UL;
    config.ProjectionInfo.nlSkipList = {1,2,3};

    config.ProjectionInfo.bRepeatLine =        true;
    config.ProjectionInfo.scantype =           config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx =    10UL;
    config.ProjectionInfo.imagetype =          config.ProjectionInfo.ImageType_Sinograms;
    config.ProjectionInfo.fCenter =            10.0f;
    config.ProjectionInfo.fSOD =               10.0f;
    config.ProjectionInfo.fSDD =               1000.0f;
    config.ProjectionInfo.fpPoint = {123.4f, 456.6f};

    config.ProjectionInfo.bTranslate =         true;
    config.ProjectionInfo.fTiltAngle =         0.1f;
    config.ProjectionInfo.fTiltPivotPosition= 1.0f;
    config.ProjectionInfo.bCorrectTilt=       true;

    config.ProjectionInfo.sFileMask =          "projs_###.fits";
    config.ProjectionInfo.sPath =              "./";

    config.ProjectionInfo.sReferencePath =     "/home/";
    config.ProjectionInfo.sOBFileMask =        "obs_###.fits";
    config.ProjectionInfo.nOBFirstIndex =      10UL;
    config.ProjectionInfo.nOBCount =           50UL;
    config.ProjectionInfo.sDCFileMask =        "dcs_##.fits";
    config.ProjectionInfo.nDCFirstIndex =      2UL;
    config.ProjectionInfo.nDCCount =           25UL;

    config.ProjectionInfo.roi[0] =             1UL;
    config.ProjectionInfo.roi[1] =             2UL;
    config.ProjectionInfo.roi[2] =             3UL;
    config.ProjectionInfo.roi[3] =             4UL;

    config.ProjectionInfo.projection_roi[0] =  5UL;
    config.ProjectionInfo.projection_roi[1] =  6UL;
    config.ProjectionInfo.projection_roi[2] =  7UL;
    config.ProjectionInfo.projection_roi[3] =  8UL;

    config.ProjectionInfo.dose_roi[0] =        9UL;
    config.ProjectionInfo.dose_roi[1] =        10UL;
    config.ProjectionInfo.dose_roi[2] =        110UL;
    config.ProjectionInfo.dose_roi[3] =        120UL;

    config.ProjectionInfo.fScanArc[0] =        10.0f;
    config.ProjectionInfo.fScanArc[1] =        36.0f;

    config.ProjectionInfo.eFlip =              kipl::base::ImageFlipVertical;
    config.ProjectionInfo.eRotate =            kipl::base::ImageRotate180;

    config.ProjectionInfo.eDirection =         kipl::base::RotationDirCCW;

    ReconConfig newconfig(config);
    QCOMPARE(newconfig.System.nMemory,                    config.System.nMemory);
    QCOMPARE(newconfig.System.eLogLevel,                  config.System.eLogLevel);
    QCOMPARE(newconfig.System.bValidateData,              config.System.bValidateData);

    QCOMPARE(newconfig.ProjectionInfo.nDims.size(),       config.ProjectionInfo.nDims.size());
    QCOMPARE(newconfig.ProjectionInfo.nDims[0],           config.ProjectionInfo.nDims[0]);
    QCOMPARE(newconfig.ProjectionInfo.nDims[1],           config.ProjectionInfo.nDims[1]);

    QCOMPARE(newconfig.ProjectionInfo.beamgeometry,       config.ProjectionInfo.beamgeometry);
    QCOMPARE(newconfig.ProjectionInfo.fResolution.size(), config.ProjectionInfo.fResolution.size());
    QCOMPARE(newconfig.ProjectionInfo.fBinning,           config.ProjectionInfo.fBinning);
    QCOMPARE(newconfig.ProjectionInfo.nMargin,            config.ProjectionInfo.nMargin);
    QCOMPARE(newconfig.ProjectionInfo.nFirstIndex,        config.ProjectionInfo.nFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nLastIndex,         config.ProjectionInfo.nLastIndex);
    QCOMPARE(newconfig.ProjectionInfo.nProjectionStep,    config.ProjectionInfo.nProjectionStep);
    QCOMPARE(newconfig.ProjectionInfo.nlSkipList.size(),  config.ProjectionInfo.nlSkipList.size());
    QCOMPARE(newconfig.ProjectionInfo.bRepeatLine,        config.ProjectionInfo.bRepeatLine);
    QCOMPARE(newconfig.ProjectionInfo.scantype,           config.ProjectionInfo.scantype);
    QCOMPARE(newconfig.ProjectionInfo.nGoldenStartIdx,    config.ProjectionInfo.nGoldenStartIdx);
    QCOMPARE(newconfig.ProjectionInfo.imagetype,          config.ProjectionInfo.imagetype);
    QCOMPARE(newconfig.ProjectionInfo.fCenter,            config.ProjectionInfo.fCenter);
    QCOMPARE(newconfig.ProjectionInfo.fSOD,               config.ProjectionInfo.fSOD);
    QCOMPARE(newconfig.ProjectionInfo.fSDD,               config.ProjectionInfo.fSDD);
    QCOMPARE(newconfig.ProjectionInfo.fpPoint.size(),     config.ProjectionInfo.fpPoint.size());

    QCOMPARE(newconfig.ProjectionInfo.bTranslate,         config.ProjectionInfo.bTranslate);
    QCOMPARE(newconfig.ProjectionInfo.fTiltAngle,         config.ProjectionInfo.fTiltAngle);
    QCOMPARE(newconfig.ProjectionInfo.fTiltPivotPosition, config.ProjectionInfo.fTiltPivotPosition);
    QCOMPARE(newconfig.ProjectionInfo.bCorrectTilt,       config.ProjectionInfo.bCorrectTilt);

    QCOMPARE(newconfig.ProjectionInfo.sFileMask,          config.ProjectionInfo.sFileMask);
    QCOMPARE(newconfig.ProjectionInfo.sPath,              config.ProjectionInfo.sPath);

    QCOMPARE(newconfig.ProjectionInfo.sReferencePath,     config.ProjectionInfo.sReferencePath);
    QCOMPARE(newconfig.ProjectionInfo.sOBFileMask,        config.ProjectionInfo.sOBFileMask);
    QCOMPARE(newconfig.ProjectionInfo.nOBFirstIndex,      config.ProjectionInfo.nOBFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nOBCount,           config.ProjectionInfo.nOBCount);
    QCOMPARE(newconfig.ProjectionInfo.sDCFileMask,        config.ProjectionInfo.sDCFileMask);
    QCOMPARE(newconfig.ProjectionInfo.nDCFirstIndex,      config.ProjectionInfo.nDCFirstIndex);
    QCOMPARE(newconfig.ProjectionInfo.nDCCount,           config.ProjectionInfo.nDCCount);

    QCOMPARE(newconfig.ProjectionInfo.roi.size(),         config.ProjectionInfo.roi.size());
    QCOMPARE(newconfig.ProjectionInfo.roi[0],             config.ProjectionInfo.roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.roi[1],             config.ProjectionInfo.roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.roi[2],             config.ProjectionInfo.roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.roi[3],             config.ProjectionInfo.roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.projection_roi.size(), config.ProjectionInfo.projection_roi.size());
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[0],  config.ProjectionInfo.projection_roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[1],  config.ProjectionInfo.projection_roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[2],  config.ProjectionInfo.projection_roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.projection_roi[3],  config.ProjectionInfo.projection_roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.dose_roi.size(),    config.ProjectionInfo.dose_roi.size());
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[0],        config.ProjectionInfo.dose_roi[0]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[1],        config.ProjectionInfo.dose_roi[1]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[2],        config.ProjectionInfo.dose_roi[2]);
    QCOMPARE(newconfig.ProjectionInfo.dose_roi[3],        config.ProjectionInfo.dose_roi[3]);

    QCOMPARE(newconfig.ProjectionInfo.fScanArc.size(),    config.ProjectionInfo.fScanArc.size());
    QCOMPARE(newconfig.ProjectionInfo.fScanArc[0],        config.ProjectionInfo.fScanArc[0]);
    QCOMPARE(newconfig.ProjectionInfo.fScanArc[1],        config.ProjectionInfo.fScanArc[1]);

    QCOMPARE(newconfig.ProjectionInfo.eFlip,              config.ProjectionInfo.eFlip);
    QCOMPARE(newconfig.ProjectionInfo.eRotate,            config.ProjectionInfo.eRotate);

    QCOMPARE(newconfig.ProjectionInfo.eDirection,         config.ProjectionInfo.eDirection);
}
void FrameWorkTest::testProjectionReader()
{
    QString msg;
    ProjectionReader reader;
    std::vector<size_t> dims;
    kipl::base::TImage<float,2> res_tiff;
    kipl::base::TImage<float,2> res_fits;
    kipl::base::TImage<float,2> ref;

    // ----------------------------------------
    // Basic read
    {
        dims=reader.GetImageSize("proj_0001.fits",1.0f);
        QVERIFY2(dims[0]==m_img.Size(0), "Reading fits size 0 error");
        QVERIFY2(dims[1]==m_img.Size(1), "Reading fits size 1 error");

        dims=reader.GetImageSize("proj_0002.tif",1.0f);
        QVERIFY2(dims[0]==m_img.Size(0), "Reading tiff size 0 error");
        QVERIFY2(dims[1]==m_img.Size(1), "Reading tiff size 1 error");

        // Basic read fits
        res_fits=reader.Read("proj_0001.fits");
        for (size_t i=0; i<m_img.Size(); i++) {
            QVERIFY2(res_fits[i]==m_fimg[i],"Data error in while reading");
        }

        // Basic read tiff
        res_tiff=reader.Read("proj_0002.tif");
        for (size_t i=0; i<m_img.Size(); i++) {
            QVERIFY2(res_tiff[i]==m_fimg[i],"Data error in while reading");
        }
    }
    // ----------------------------------------
    // Crop  reference data
    std::vector<size_t> crop={2,3,7,9};
    kipl::base::TImage<float,2> crop_ref;
    kipl::base::TSubImage<float,2> cropper;


    // ----------------------------------------
    // Cropping
    {
        crop_ref=cropper.Get(m_fimg,crop,false);
        // Cropped fits
        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,crop);

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with crop");
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with crop");

        for (size_t i=0; i<res_fits.Size(); ++i) {

            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        // Cropped tiff
        res_tiff=reader.Read("proj_0002.tif",kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,crop);

        QVERIFY2(res_tiff.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with crop");
        QVERIFY2(res_tiff.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with crop");

        for (size_t i=0; i<res_tiff.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_tiff[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_tiff[i]==crop_ref[i]
                     ,msg.toStdString().c_str());
        }
    }

    // Crop works rotation and mirror will only be tested with fits
    // todo: maybe it still makes sense to test with tiff too...

    // -----------------------------------------
    // Full read with mirrors
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorVertical(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);
        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horvert");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horvert");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;
        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate90,1.0f,{});
        rot_ref=rot.Rotate90(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate180,1.0f,{});

        rot_ref=rot.Rotate180(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate270,1.0f,{});

        rot_ref=rot.Rotate270(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with mirror and rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,{});

        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate180,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.Rotate180(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate270,1.0f,{});
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,{});
        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate180,1.0f,{});
        rot_ref=rot.Rotate180(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate270,1.0f,{});
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    std::cout<<"Cropped read w. mirrors"<<std::endl;
    // -----------------------------------------
    // Cropped read with mirrors
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,crop);
        std::cout<<"MirrorHorizontalCrop"<<std::endl;
        rot_ref=rot.MirrorHorizontal(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);
        msg.sprintf("Size mismatch for fits reading with horizontal w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());
        QVERIFY(res_fits.Size()==crop_ref.Size());
        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,crop);
        std::cout<<"MirrorVerticalCrop"<<std::endl;
        rot_ref=rot.MirrorVertical(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.sprintf("Size mismatch for fits reading with vertical w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,crop);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);
        std::cout<<"MirrorHorizontalVerticalCrop"<<std::endl;

        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.sprintf("Size mismatch for fits reading with horizontal/vertical w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        QWARN("The horizontal/vertical test is currently excluded");
//        for (size_t i=0; i<res_fits.Size(); ++i) {
//            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
//            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
//        }
    }

    // -----------------------------------------
    // Cropped read with rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        try {
            std::cout<<"Rotate90"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);

            msg.sprintf("Size mismatch for fits reading with rotate90 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try {
            std::cout<<"Rotate180"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate180,1.0f,crop);

            rot_ref=rot.Rotate180(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);
            msg.sprintf("Size mismatch for fits reading with rotate180 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try {
            std::cout<<"Rotate270"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate270,1.0f,crop);
            rot_ref=rot.Rotate270(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);
            msg.sprintf("Size mismatch for fits reading with rotate270 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }
    }
    std::cout<<"Cropped rotate"<<std::endl;

    // -----------------------------------------
    // Cropped read with mirror and rotate
    std::cout<<"Cropped read w. mirror and rotate"<<std::endl;
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref, mirror_ref;

        // Horizontal + 90
        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        // Vertical + 90
        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }
    }
}

void FrameWorkTest::testScanTypeEnum()
{
    std::map<std::string,ReconConfig::cProjections::eScanType> testData;
    testData["sequential"]=ReconConfig::cProjections::SequentialScan;
    testData["goldensection"]=ReconConfig::cProjections::GoldenSectionScan;
    testData["invgoldensection"]=ReconConfig::cProjections::InvGoldenSectionScan;

    ReconConfig::cProjections::eScanType st;

    for (auto item : testData)
    {
        QCOMPARE(enum2string(item.second),item.first);

        string2enum(item.first,st);
        QCOMPARE(st,item.second);
    }
}

void FrameWorkTest::testBuildFileList_GeneratedSequence()
{
    std::ostringstream msg;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=360.0f;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
    msg.str(""); msg<<"Expected size "<<config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1,msg.str().c_str());
    float sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); it++) {
        std::cout<<(it->first)<<", "<<(*it).second.name<<", "<<(*it).second.angle<<", "<<(*it).second.weight<<std::endl;
        sum+=(*it).second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;

    BuildFileList(&config,&ProjectionList);
    msg.str(""); msg<<"Expected size "<<config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1,msg.str().c_str());
    sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); it++) {
        std::cout<<(it->first)<<", "<<(*it).second.name<<", "<<(*it).second.angle<<", "<<(*it).second.weight<<std::endl;
        sum+=(*it).second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden()
{
    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 0;
    config.ProjectionInfo.nLastIndex      = 18;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.0f      , 291.2461f  , 222.49222f , 153.73837f ,  84.98446f ,
                             16.230604f, 307.47675f , 238.72289f , 169.96892f , 101.21517f ,
                             32.46121f , 323.70724f , 254.95349f , 186.19952f , 117.44578f ,
                             48.691597f, 339.93784f , 271.18408f , 202.43034f};

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(&config,&ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180b={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    std::sort(gv180b.begin(),gv180b.end());
    git=gv180b.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden_offset()
{
    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 10;
    config.ProjectionInfo.nLastIndex      = 28;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 10;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.0f      , 291.2461f  , 222.49222f , 153.73837f ,  84.98446f ,
                             16.230604f, 307.47675f , 238.72289f , 169.96892f , 101.21517f ,
                             32.46121f , 323.70724f , 254.95349f , 186.19952f , 117.44578f ,
                             48.691597f, 339.93784f , 271.18408f , 202.43034f};

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(&config,&ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180b={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    std::sort(gv180b.begin(),gv180b.end());
    git=gv180b.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden_delay()
{
   // Starting at index 10
    std::vector<float> gv180={32.46117974981095f,
                              143.70729772479237f,
                              74.95341569977299f,
                              6.19971f,
                              117.44565164973501f,
                              48.69176962471643f,
                              159.93788759969704f,
                              91.18400557467847f,
                              22.43012354965988f,
                              133.6762415246405f,
                              64.9223594996219f,
                              176.16847747460253f,
                              107.41459544958475f,
                              38.6611f,
                              149.90683139954598f,
                              81.15294937452657f,
                              12.3994f,
                              123.64518532449023f,
                              54.89130329947002f,
                              166.13742127445144f};

    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 10;
    config.ProjectionInfo.nLastIndex      = 29;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        QCOMPARE(it.first,*git);
        ++git;
    }

    //
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 12;
    config.ProjectionInfo.nLastIndex      = 31;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 2;

    ProjectionList.clear();
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    git=gv180.begin();

    for (auto &it: ProjectionList) {
        QCOMPARE(it.first,*git);
        ++git;
    }
}

void FrameWorkTest::testBuildFileList_GeneratedInvGolden()
{
    std::ostringstream msg;
    size_t N=10;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=0;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.scantype=config.ProjectionInfo.InvGoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx=0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={  0.f,        111.24612f ,   42.492245f,  153.73833f,    84.98449f,    16.23059f,
            127.47666f,    58.722794f , 169.96898f ,  101.2151f ,    32.46118f,   143.70726f,
             74.95333f,     6.19958f, 117.44559f,    48.691772f , 159.93796f,    91.18403f,
             22.430218f };


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(&config,&ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.f      ,  111.24612f  , 222.49223f  , 333.7383f  ,   84.98449f ,  196.23059f,
                                  307.47665f  ,  58.722794f , 169.96898f ,  281.2151f  ,   32.46118f  , 143.70726f,
                                  254.95332f ,    6.19958f ,117.44559f ,  228.69177f ,  339.93796f ,   91.18403f,
                                  202.4302f   };

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(&config,&ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180odd={  0.f,        111.24612f ,   42.492245f,  153.73833f,    84.98449f,    16.23059f,
            127.47666f,    58.722794f , 169.96898f ,  101.2151f ,    32.46118f,   143.70726f,
             74.95333f,     6.19958f, 117.44559f,    48.691772f , 159.93796f,    91.18403f,
             22.430218f };
    std::sort(gv180odd.begin(),gv180odd.end());
    git=gv180odd.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedInvGolden_delay()
{

}

void FrameWorkTest::testBuildFileList()
{
    // File list
    std::ofstream listfile("listfile.txt");
    std::ostringstream fname,msg;
    size_t N=10;
    size_t i=0;
    for (i=0; i<N; ++i)
    {
        listfile<<18*i<<",file_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;
    }

    listfile<<18*i<<", file_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;

    ReconConfig config("");

    config.ProjectionInfo.sFileMask="listfile.txt";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=i+1;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);

    QVERIFY(ProjectionList.size()==i+1);

    i=0;
    float sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); ++it,++i)
    {
        fname.str("");
        msg.str("");
        fname<<"file_"<<setfill('0') << setw(5) << i <<".fits";
        msg<<fname.str()<<"!="<<it->second.name;
        QVERIFY2(it->second.name==fname.str(),msg.str().c_str());
        QVERIFY(it->second.angle==static_cast<float>(18*i));
        sum+=it->second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testProcessTimingLogger()
{
    ProcessTimingLogger ptl("test.json");

    std::map<std::string,std::map<std::string,std::string>> entryData = {{"data",{{"proj","500"}}},{"time",{{"bp","10"}}}};


    ptl.addLogEntry(entryData);
    std::this_thread::sleep_for(chrono::seconds(1));
    entryData["hardware"]={{"proj","500"}};
    ptl.addLogEntry(entryData);
}

QTEST_APPLESS_MAIN(FrameWorkTest)

#include "tst_frameworktest.moc"

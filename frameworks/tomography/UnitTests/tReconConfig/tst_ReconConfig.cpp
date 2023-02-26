//<LICENSE>

#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <ReconConfig.h>
#include <base/KiplException.h>
#include <strings/filenames.h>

class ReconConfigTests : public QObject
{
    Q_OBJECT

public:
    ReconConfigTests();
    ~ReconConfigTests();

private slots:
    void testConfigEnums();
    void testCLIArgs();
};


ReconConfigTests::ReconConfigTests()
{

}

ReconConfigTests::~ReconConfigTests()
{

}

void ReconConfigTests::testConfigEnums()
{
    // //    enum2string();
    // QCOMPARE(enum2string(kipl::logging::Logger::LogDebug).c_str(),   "debug");
    // QCOMPARE(enum2string(kipl::logging::Logger::LogVerbose).c_str(), "verbose");
    // QCOMPARE(enum2string(kipl::logging::Logger::LogMessage).c_str(), "message");
    // QCOMPARE(enum2string(kipl::logging::Logger::LogWarning).c_str(), "warning");
    // QCOMPARE(enum2string(kipl::logging::Logger::LogError).c_str(),   "error");

    // //    string2enum();
    // kipl::logging::Logger::LogLevel level;
    // string2enum("debug",level);
    // QCOMPARE(level,kipl::logging::Logger::LogDebug);
    // string2enum("verbose",level);
    // QCOMPARE(level,kipl::logging::Logger::LogVerbose);
    // string2enum("message",level);
    // QCOMPARE(level,kipl::logging::Logger::LogMessage);
    // string2enum("warning",level);
    // QCOMPARE(level,kipl::logging::Logger::LogWarning);
    // string2enum("error",level);
    // QCOMPARE(level,kipl::logging::Logger::LogError);
}


void ReconConfigTests::testCLIArgs()
{
    ReconConfig config("path");

    QCOMPARE(config.appPath(),std::string("path"));

    std::vector<std::string> arguserinfo = {"muhrec","-f","config.xml",
        "userinformation:operator=pelle",      
        "userinformation:instrument=testbeamline",
        "userinformation:projectnumber=Q1234", 
        "userinformation:sample=rubberduck",
        "userinformation:comment=bathtub"
    };

    config.GetCommandLinePars(arguserinfo);
    QCOMPARE(config.UserInformation.sOperator,      std::string("pelle"));
    QCOMPARE(config.UserInformation.sInstrument,    std::string("testbeamline"));
    QCOMPARE(config.UserInformation.sProjectNumber, std::string("Q1234"));
    QCOMPARE(config.UserInformation.sSample,        std::string("rubberduck"));
    QCOMPARE(config.UserInformation.sComment,       std::string("bathtub"));

    std::vector<std::string> argsystem ={"muhrec","-f","config.xml",
        "system:memory=1234",
        "system:loglevel=error",
        "system:validate=false",
        "system:maxthreads=123",
        "system:threadmethod=threadingSTL"
    };

    config.GetCommandLinePars(argsystem);
    QCOMPARE(config.System.nMemory,       1234UL);
    QCOMPARE(config.System.eLogLevel,     kipl::logging::Logger::LogError);
    QCOMPARE(config.System.bValidateData, false);
    QCOMPARE(config.System.nMaxThreads,   123UL);
    QCOMPARE(config.System.eThreadMethod, kipl::base::threadingSTL); 

    std::string filemask = "/home/tester/proj_####.fits";
    kipl::strings::filenames::CheckPathSlashes(filemask,false); 
    std::string path = "/shared/data";
    kipl::strings::filenames::CheckPathSlashes(path,true); 
    std::string refpath = "/shared/ref";
    kipl::strings::filenames::CheckPathSlashes(refpath,true); 
    std::string obmask = "/home/data/OB_####.fits";
    kipl::strings::filenames::CheckPathSlashes(obmask,false); 
    std::string dcmask = "/home/refdata/DC1_####.fits";
    kipl::strings::filenames::CheckPathSlashes(dcmask,false); 

    std::vector<std::string> argproj ={"muhrec","-f","config.xml",
            "projections:dims=10 11",
            "projections:resolution=0.1 0.2",
            "projections:binning=2.5",
            "projections:margin=10",
            "projections:firstindex=7",
            "projections:lastindex=20",
            "projections:projectionstep=3",
            "projections:repeatline=true",
            "projections:scantype=sequential",
            "projections:imagetype=sinograms",
            "projections:center=3.1415",
            "projections:translation=true",
            "projections:tiltangle=0.123",
            "projections:tiltpivot=456.7",
            "projections:correcttilt=true",
            "projections:filemask="+filemask,
            "projections:path="+path, 
            "projections:referencepath="+refpath, 
            "projections:obfilemask="+obmask,
            "projections:obfirstindex=5",
            "projections:obcount=12",
            "projections:dcfilemask="+dcmask,
            "projections:dcfirstindex=13",
            "projections:dccount=42",
            "projections:roi=5 6 7 8",
            "projections:projroi=2 3 4 5",
            "projections:doseroi=10 20 25 30",
            "projections:scanarc=0.0 150.0",
            // "projections:scanarc0=10.0", must be tested separately as they overwrite scanarc
            // "projections:scanarc1=12.0",
            "projections:rotate=imagerotate270",
            "projections:flip=imageflipvertical",
            "projections:direction=RotationDirCCW",
            "projections:sod=12.0",
            "projections:sdd=23.0",
            "projections:pPoint=1000 1020"
        };

        config.GetCommandLinePars(argproj);
        QCOMPARE(config.ProjectionInfo.nDims[0],10UL);
        QCOMPARE(config.ProjectionInfo.nDims[1],11UL);
        QCOMPARE(config.ProjectionInfo.fResolution[0],0.1f);
        QCOMPARE(config.ProjectionInfo.fResolution[1],0.2f);
        QCOMPARE(config.ProjectionInfo.fBinning,2.5f);
        QCOMPARE(config.ProjectionInfo.nMargin,10UL);
        QCOMPARE(config.ProjectionInfo.nFirstIndex,7UL);
        QCOMPARE(config.ProjectionInfo.nLastIndex,20UL);
        QCOMPARE(config.ProjectionInfo.nProjectionStep,3UL);
        QCOMPARE(config.ProjectionInfo.bRepeatLine,true);
        QCOMPARE(config.ProjectionInfo.scantype,ReconConfig::cProjections::SequentialScan);   
        QCOMPARE(config.ProjectionInfo.imagetype,ReconConfig::cProjections::ImageType_Sinograms);       
        QCOMPARE(config.ProjectionInfo.fCenter,3.1415f);   
        QCOMPARE(config.ProjectionInfo.bTranslate,true);
        // QCOMPARE(config.ProjectionInfo.fTiltAngle,0.123);   // Precision problems of float in the conversion
        QCOMPARE(config.ProjectionInfo.fTiltPivotPosition,456.7f);    
        QCOMPARE(config.ProjectionInfo.bCorrectTilt,true);
        QCOMPARE(config.ProjectionInfo.sFileMask,filemask);
        QCOMPARE(config.ProjectionInfo.sPath,path);
        QCOMPARE(config.ProjectionInfo.sReferencePath,refpath);
        QCOMPARE(config.ProjectionInfo.sOBFileMask,obmask);
        QCOMPARE(config.ProjectionInfo.nOBFirstIndex,5);
        QCOMPARE(config.ProjectionInfo.nOBCount,12);  
        QCOMPARE(config.ProjectionInfo.sDCFileMask,dcmask);
        QCOMPARE(config.ProjectionInfo.nDCFirstIndex,13UL);
        QCOMPARE(config.ProjectionInfo.nDCCount,42UL);
        QCOMPARE(config.ProjectionInfo.roi[0],5);
        QCOMPARE(config.ProjectionInfo.roi[1],6);
        QCOMPARE(config.ProjectionInfo.roi[2],7);
        QCOMPARE(config.ProjectionInfo.roi[3],8);
        QCOMPARE(config.ProjectionInfo.projection_roi[0],2);
        QCOMPARE(config.ProjectionInfo.projection_roi[1],3);
        QCOMPARE(config.ProjectionInfo.projection_roi[2],4);
        QCOMPARE(config.ProjectionInfo.projection_roi[3],5);
        QCOMPARE(config.ProjectionInfo.dose_roi[0],10);
        QCOMPARE(config.ProjectionInfo.dose_roi[1],20);
        QCOMPARE(config.ProjectionInfo.dose_roi[2],25);
        QCOMPARE(config.ProjectionInfo.dose_roi[3],30);
        QCOMPARE(config.ProjectionInfo.fScanArc[0],0.0);
        QCOMPARE(config.ProjectionInfo.fScanArc[1],150.0);
        QCOMPARE(config.ProjectionInfo.eRotate,kipl::base::ImageRotate270);
        QCOMPARE(config.ProjectionInfo.eFlip,kipl::base::ImageFlipVertical);
        QCOMPARE(config.ProjectionInfo.eDirection,kipl::base::RotationDirCCW);
        QCOMPARE(config.ProjectionInfo.fSOD,12.0f);
        QCOMPARE(config.ProjectionInfo.fSDD,23.0f);
        QCOMPARE(config.ProjectionInfo.fpPoint[0],1000);
        QCOMPARE(config.ProjectionInfo.fpPoint[1],1020);

        std::vector<std::string> argmat ={"muhrec","-f","config.xml",
            "matrix:dims=11 12 13",
            "matrix:rotation=45.0",
            "matrix:serialize=true",
            "matrix:path="+path,
            "matrix:matrixname=volume_###.tif",
            "matrix:filetype=TIFFfloatMultiFrame",
            "matrix:firstindex=123",
            "matrix:grayinterval=-10.0 12.3",
            "matrix:useroi=true",
            "matrix:roi=10 20 30 40"
         //   "matrix:voi")          not tested
        };

        config.GetCommandLinePars(argmat);
        QCOMPARE(config.MatrixInfo.nDims[0],11);
        QCOMPARE(config.MatrixInfo.nDims[1],12);
        QCOMPARE(config.MatrixInfo.nDims[2],13);
        QCOMPARE(config.MatrixInfo.fRotation,45.0f);
        QCOMPARE(config.MatrixInfo.bAutomaticSerialize,true);
        QCOMPARE(config.MatrixInfo.sDestinationPath,path);
        QCOMPARE(config.MatrixInfo.sFileMask,std::string("volume_###.tif"));
        QCOMPARE(config.MatrixInfo.FileType,kipl::io::TIFFfloatMultiFrame);
        QCOMPARE(config.MatrixInfo.nFirstIndex,123UL);
        QCOMPARE(config.MatrixInfo.fGrayInterval[0],-10.0f);
        QCOMPARE(config.MatrixInfo.fGrayInterval[1],12.3f);
        QCOMPARE(config.MatrixInfo.bUseROI,true);
        QCOMPARE(config.MatrixInfo.roi[0],10UL);
        QCOMPARE(config.MatrixInfo.roi[1],20UL);
        QCOMPARE(config.MatrixInfo.roi[2],30UL);
        QCOMPARE(config.MatrixInfo.roi[3],40UL);

}

QTEST_APPLESS_MAIN(ReconConfigTests)

#include "tst_ReconConfig.moc"

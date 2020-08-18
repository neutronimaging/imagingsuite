//<LICENSE>

#include <algorithm>

#include <QString>
#include <QtTest>

#include <MorphSpotCleanModule.h>
#include <ModuleException.h>
#include <strings/filenames.h>
#include <io/io_tiff.h>
#include <base/timage.h>
#include <StdPreprocModules.h>

class TStdPreprocModulesTest : public QObject
{
    Q_OBJECT

public:
    TStdPreprocModulesTest();

private Q_SLOTS:
    void testGetModuleList();
    void testFullLogNorm();
    void testMorpSpotClean_Initialize();
    void testMorpSpotClean_Process();
};

TStdPreprocModulesTest::TStdPreprocModulesTest()
{

}

void TStdPreprocModulesTest::testGetModuleList()
{
    std::map<std::string, std::map<std::string, std::string> > modulelist;
    GetModuleList("muhrec", reinterpret_cast<void *>(&modulelist));
    qDebug() << modulelist.size();

    QCOMPARE(modulelist.size(),20UL);
}

void TStdPreprocModulesTest::testFullLogNorm()
{

}

void TStdPreprocModulesTest::testMorpSpotClean_Initialize()
{
    MorphSpotCleanModule msm(nullptr);

    size_t roi[4]={10,100,990,110};
    msm.SetROI(roi);
    ReconConfig config("");

#ifndef _DEBUG
    std::string confname = "../imagingsuite/frameworks/tomography/data/ReconConfig_MorphSpotClean.xml";
#else
    std::string confname = "../../imagingsuite/frameworks/tomography/data/ReconConfig_MorphSpotClean.xml";
#endif

    kipl::strings::filenames::CheckPathSlashes(confname,false);
    try {
    config.LoadConfigFile(confname,"reconstructor");

    } catch (ModuleException &e) {
        qDebug() << e.what();
        return;
    }
    qDebug() << config.modules[1].m_sModule.c_str();
    try {
        msm.Configure(config,config.modules[1].parameters); // index 1 is the morphspotclean in the config file.
    }
    catch (ModuleException &e) {
        qDebug() << "Error "<< e.what();
    }
    catch (kipl::base::KiplException &e) {
        qDebug() << "Error "<< e.what();
    }
    catch (std::exception &e){
        qDebug() << "std Error "<< e.what();
    }

    std::map<std::string,std::string> expect = {
    {"clampdata","false"},
    {"cleanmethod","morphcleanreplace"},
    {"connectivity","conn4"},
    {"detectionmethod","morphdetectpeaks"},
    {"edgesmooth","5"},
    {"maxarea","30"},
    {"maxlevel","7"},
    {"minlevel","-0.1"},
    {"removeinfnan","false"},
    {"sigma","0.01 0.01"},
    {"threading","false"},
    {"threshold","0.1 0.1"},
    {"transpose","false"}
    };

    auto pars = msm.GetParameters();

    for (auto & exp : expect)
    {
        auto it = pars.find(exp.first);
        if (it != pars.end())
        {
            QCOMPARE(it->second,exp.second);
        }
        else {
            QFAIL(exp.first.c_str());
        }
    }
}

void TStdPreprocModulesTest::testMorpSpotClean_Process()
{
    MorphSpotCleanModule msm(nullptr);

    size_t roi[4]={10,100,990,110};
    msm.SetROI(roi);
    ReconConfig config("");

#ifndef _DEBUG
    std::string confname = "../imagingsuite/frameworks/tomography/data/ReconConfig_MorphSpotClean.xml";
    std::string imgname  = "../imagingsuite/frameworks/tomography/data/woodsino_0200.tif";
#else
    std::string confname = "../../imagingsuite/frameworks/tomography/data/ReconConfig_MorphSpotClean.xml";
    std::string imgname  = "../../imagingsuite/frameworks/tomography/data/woodsino_0200.tif";
#endif

    kipl::strings::filenames::CheckPathSlashes(confname,false);
    try {
        config.LoadConfigFile(confname,"reconstructor");
        msm.Configure(config,config.modules[1].parameters);
    } catch (ModuleException &e) {
        qDebug() << e.what();
        return;
    }

    kipl::base::TImage<float,2> img;
    kipl::strings::filenames::CheckPathSlashes(imgname,false);
    kipl::io::ReadTIFF(img,imgname.c_str());
    size_t nLines = min(40UL,img.Size(1));
 //   size_t dims[3]={img.Size(0),nLines,8};
    kipl::base::TImage<float,3> img3;

    for (size_t i=0; i<nLines; ++i) {
        std::copy_n(img.GetDataPtr(),nLines*img.Size(0),img.GetLinePtr(0,i));
    }

    msm.Process(img,config.modules[1].parameters);

 //   msm.Process(img3,config.modules[1].parameters);

}

QTEST_APPLESS_MAIN(TStdPreprocModulesTest)

#include "tst_tstdpreprocmodulestest.moc"

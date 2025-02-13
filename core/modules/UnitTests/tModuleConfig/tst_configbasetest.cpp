//<LICENSE>

#include <QString>
#include <QtTest>
#include <QDebug>
#include <QDir>

#include <ModuleException.h>
#include <ReconConfig.h>
#include <modulelibnamemanager.h>
#include <strings/filenames.h>
#include "dummyconfig.h"

class ConfigBaseTest : public QObject
{
    Q_OBJECT

public:
    ConfigBaseTest();

private Q_SLOTS:
    void testConstructor();
    void testCopyConstructor();
    void testAssignment();
    void testConfigChanged();
    void testEvalArg();
    void testGetCommandLinePars();
    void testLibNameManagerMac();
    void testLibNameManagerWindows();
    void testLibNameManagerLinux();
};

ConfigBaseTest::ConfigBaseTest()
{
}

void ConfigBaseTest::testConstructor()
{
    DummyConfig config;

    QCOMPARE(config.m_sName,"DummyConfig");
    QCOMPARE(config.m_sApplicationPath,"");
    QCOMPARE(config.UserInformation.sSample,"Unknown item");
    QCOMPARE(config.UserInformation.sComment,"No comment");
    QCOMPARE(config.UserInformation.sVersion,"0");
    QCOMPARE(config.UserInformation.sOperator,"Anders Kaestner");
    QCOMPARE(config.UserInformation.sInstrument,"ICON");
    QCOMPARE(config.UserInformation.sProjectNumber,"P11001");
}

void ConfigBaseTest::testCopyConstructor()
{
    DummyConfig config;

    config.m_sName                        = "NewConfig";
    config.m_sApplicationPath             = "/home/sweet/home";
    config.UserInformation.sSample        = "Known item";
    config.UserInformation.sComment       = "A good comment";
    config.UserInformation.sVersion       = "1.0";
    config.UserInformation.sOperator      = "Pelle Jons";
    config.UserInformation.sInstrument    = "NEUTRA";
    config.UserInformation.sProjectNumber = "P2020001";

    auto c2(config);

    QCOMPARE(c2.m_sName,"NewConfig");
    QCOMPARE(c2.m_sApplicationPath,"/home/sweet/home");
    QCOMPARE(c2.UserInformation.sSample,"Known item");
    QCOMPARE(c2.UserInformation.sComment,"A good comment");
    QCOMPARE(c2.UserInformation.sVersion,"1.0");
    QCOMPARE(c2.UserInformation.sOperator,"Pelle Jons");
    QCOMPARE(c2.UserInformation.sInstrument,"NEUTRA");
    QCOMPARE(c2.UserInformation.sProjectNumber,"P2020001");
}

void ConfigBaseTest::testAssignment()
{
    DummyConfig config;

    config.m_sName                        = "NewConfig";
    config.m_sApplicationPath             = "/home/sweet/home";
    config.UserInformation.sSample        = "Known item";
    config.UserInformation.sComment       = "A good comment";
    config.UserInformation.sVersion       = "1.0";
    config.UserInformation.sOperator      = "Pelle Jons";
    config.UserInformation.sInstrument    = "NEUTRA";
    config.UserInformation.sProjectNumber = "P2020001";

    DummyConfig c2;

    c2 = config;

    QCOMPARE(c2.m_sName,"NewConfig");
    QCOMPARE(c2.m_sApplicationPath,"/home/sweet/home");
    QCOMPARE(c2.UserInformation.sSample,"Known item");
    QCOMPARE(c2.UserInformation.sComment,"A good comment");
    QCOMPARE(c2.UserInformation.sVersion,"1.0");
    QCOMPARE(c2.UserInformation.sOperator,"Pelle Jons");
    QCOMPARE(c2.UserInformation.sInstrument,"NEUTRA");
    QCOMPARE(c2.UserInformation.sProjectNumber,"P2020001");
}

void ConfigBaseTest::testConfigChanged()
{
        //DummyConfig a(""),b("");
        ReconConfig a(""),b("");
        std::list<std::string> freelist;
        qDebug()<<"start";

        QVERIFY2(a.ConfigChanged(b,freelist) == false, "Same config failed");

        b.ProjectionInfo.fCenter=1234.0f;
        QVERIFY2(a.ConfigChanged(b,freelist) == true , "One parameter changed failed");

        freelist.push_back("center");
        QVERIFY2(a.ConfigChanged(b,freelist) == false, "One parameter in the free list changed");

        freelist.push_back("correcttilt");
        QVERIFY2(a.ConfigChanged(b,freelist) == false, "Two parameters in the free list one changed");

        b.ProjectionInfo.bCorrectTilt=true;
        QVERIFY2(a.ConfigChanged(b,freelist) == false, "Two parameters in the free list both changed");

        b.ProjectionInfo.dose_roi[2]=1234;
        QVERIFY2(a.ConfigChanged(b,freelist) == true , "Two parameters in the free list, three changed");

        ReconConfig c(""),d("");
        // DummyConfig c(""),d("");
        QVERIFY2(c.ConfigChanged(d,freelist) == false , "Two parameters in the free list, same config");
}

void ConfigBaseTest::testEvalArg()
{
    std::string arg;
    std::string group;
    std::string var;
    std::string value;

    arg="userinformation:sample=mouse";
    ReconConfig config("");
    // DummyConfig config("");

    config.EvalArg(arg, group, var, value);

    QVERIFY2(group=="userinformation","Extract group");
    QVERIFY2(var=="sample",var.c_str());
    QVERIFY2(value=="mouse",value.c_str());

    // Provoke no value
    arg="userinformation:sample";
    group.clear(); var.clear(); value.clear();
    QVERIFY_THROWS_EXCEPTION(ModuleException, config.EvalArg(arg,group,var,value));

    arg="userinformationsample";
    group.clear(); var.clear(); value.clear();
    QVERIFY_THROWS_EXCEPTION(ModuleException, config.EvalArg(arg,group,var,value));

    arg="projections:roi=40 40 50 50";
    config.EvalArg(arg, group, var, value);

    QVERIFY2(group=="projections","Extract group");
    QVERIFY2(var=="roi",var.c_str());
    QVERIFY2(value=="40 40 50 50",value.c_str());

    arg="\"projections:roi=10 20 40 60\"";
    config.EvalArg(arg, group, var, value);
    QVERIFY2(group=="projections","Extract group");
    QVERIFY2(var=="roi",var.c_str());
    QVERIFY2(value=="10 20 40 60",value.c_str());

    arg="'projections:roi=1 2 4 6'";
    config.EvalArg(arg, group, var, value);
    QVERIFY2(group=="projections","Extract group");
    QVERIFY2(var=="roi",var.c_str());
    QVERIFY2(value=="1 2 4 6",value.c_str());

}

void ConfigBaseTest::testGetCommandLinePars()
{
    std::vector<std::string> args;

    args.push_back("muhrec3");
    args.push_back("-f");
    args.push_back("/Users/kaestner/.imagingtools/test.xml");
    args.push_back("userinformation:sample=mouse");
    args.push_back("projections:center=100.45");


    ReconConfig config("");

    // DummyConfig config("");

    config.GetCommandLinePars(args);
}

void ConfigBaseTest::testLibNameManagerMac()
{
    std::string appPath     = "/Users/kaestner/git/deployed/MuhRec.app/Contents/MacOS/";
    std::string category    = "BackProjectors";
    std::string modulePath  = "/Users/kaestner/git/deployed/MuhRec.app/Contents/PlugIns/BackProjectors/libStdBackProjectors.dylib";
    std::string modulePath2 = "/Users/kaestner/git/deployed/MuhRec.app/Contents/MacOS/../PlugIns/BackProjectors/libStdBackProjectors.dylib";

    // Tests with plugin path
    ModuleLibNameManger mlnm(appPath,true,category);

    std::string name=mlnm.stripLibName(modulePath,kipl::base::OSMacOS);

    qDebug() << name.c_str();
    QCOMPARE(name,std::string("StdBackProjectors"));

    name=mlnm.stripLibName(modulePath2,kipl::base::OSMacOS);

    QCOMPARE(name,std::string("StdBackProjectors"));

    name = mlnm.generateLibName("StdBackProjectors",kipl::base::OSMacOS);

    QCOMPARE(name,modulePath);

    std::string modulePath3 = "/Users/kaestner/libStdBackProjectors.1.0.0.dylib";

    QCOMPARE(mlnm.stripLibName(modulePath3,kipl::base::OSMacOS),modulePath3);

    QCOMPARE(mlnm.generateLibName(modulePath3,kipl::base::OSMacOS),modulePath3);

    std::string path = mlnm.generateLibPath(kipl::base::OSMacOS);

    QCOMPARE(path,std::string("/Users/kaestner/git/deployed/MuhRec.app/Contents/PlugIns/BackProjectors/"));

    // Tests without plugin path
    ModuleLibNameManger mlnm2(appPath,false,category);
}

void ConfigBaseTest::testLibNameManagerLinux()
{
    std::string appPath     = "/home/kaestner/build-imagingsuite/Release/bin";
    std::string category    = "BackProjectors";
    std::string modulePath  = "/home/kaestner/build-imagingsuite/Release/PlugIns/BackProjectors/libStdBackProjectors.so";
    std::string modulePath3 = "/home/kaestner/build-imagingsuite/Release/PlugIns/BackProjectors/libStdBackProjectors.so";

    // Tests with plugin path
    ModuleLibNameManger mlnm(appPath,true,category);

    qDebug() << mlnm.stripLibName(modulePath3,kipl::base::OSLinux).c_str() <<", "<< std::string("StdBackProjectors").c_str();
    
    QCOMPARE(mlnm.stripLibName(modulePath3,kipl::base::OSLinux),std::string("StdBackProjectors"));
    QCOMPARE(mlnm.generateLibName("StdBackProjectors",kipl::base::OSLinux),modulePath);

    std::string path = mlnm.generateLibPath(kipl::base::OSLinux);

    QCOMPARE(path,std::string("/home/kaestner/build-imagingsuite/Release/PlugIns/BackProjectors/"));
    // This is a repeat of the above test
    // std::string modulePath2 = "../build-imagingsuite/Release/lib/libStdBackProjectors.so";

    // QCOMPARE(mlnm.stripLibName(modulePath2,kipl::base::OSLinux),modulePath2);

    // QCOMPARE(mlnm.generateLibName(modulePath2,kipl::base::OSLinux),modulePath2);


    // Tests without plugin path
    ModuleLibNameManger mlnm2(appPath,false,category);

}

void ConfigBaseTest::testLibNameManagerWindows()
{
    std::string appPath    = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\";
    std::string category   = "Preprocessing";
    std::string modulePath = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\PlugIns\\Preprocessing\\StdPreprocModules.dll";

    // kipl::strings::filenames::CheckPathSlashes(appPath,true);
    // kipl::strings::filenames::CheckPathSlashes(modulePath,false);

    // Tests with plugin path
    ModuleLibNameManger mlnm(appPath,true,category);

    QCOMPARE(mlnm.stripLibName(modulePath,kipl::base::OSWindows),"StdPreprocModules");

    QCOMPARE(mlnm.generateLibName("StdPreprocModules",kipl::base::OSWindows),modulePath);

    std::string modulePath2 = "C:\\Users\\kaestner\\muhrec\\PlugIns\\Preprocprocessing\\StdPreprocModules.dll";


    QCOMPARE(mlnm.stripLibName(modulePath2,kipl::base::OSWindows),modulePath2);

    QCOMPARE(mlnm.generateLibName(modulePath2,kipl::base::OSWindows),modulePath2);

    std::string path = mlnm.generateLibPath(kipl::base::OSWindows);

    QCOMPARE(path,std::string("C:\\Users\\kaestner\\git\\deployed\\muhrec\\PlugIns\\Preprocessing\\"));    

    // Tests without plugin path
    ModuleLibNameManger mlnm2(appPath,false,category);

}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(ConfigBaseTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(ConfigBaseTest)
#endif


#include "tst_configbasetest.moc"

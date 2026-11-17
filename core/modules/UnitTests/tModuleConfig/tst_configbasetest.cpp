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
    std::string modulePathExt = "/Users/kaestner/libStdBackProjectors.1.0.0.dylib";

    //
    // Tests with standard path
    //
    {
    std::string modulePathStd1  = "/Users/kaestner/git/deployed/MuhRec.app/Contents/Frameworks/libStdBackProjectors.dylib";
    std::string modulePathStd2 = "/Users/kaestner/git/deployed/MuhRec.app/Contents/MacOS/../Frameworks/libStdBackProjectors.dylib";

    ModuleLibNameManager mlnm_std(appPath,false,category);

    std::string name=mlnm_std.stripLibName(modulePathStd1,kipl::base::OSMacOS);
    QCOMPARE(name,std::string("StdBackProjectors"));

    name=mlnm_std.stripLibName(modulePathStd2,kipl::base::OSMacOS);
    QCOMPARE(name,std::string("StdBackProjectors"));

    name = mlnm_std.generateLibName("StdBackProjectors", kipl::base::OSMacOS);
    QCOMPARE(name,modulePathStd1);
    

    // Test plugins outside the app bundle
    QCOMPARE(mlnm_std.stripLibName(modulePathExt,kipl::base::OSMacOS),modulePathExt);
    QCOMPARE(mlnm_std.generateLibName(modulePathExt,kipl::base::OSMacOS),modulePathExt);
    }

    //
    // Tests with plugin path
    //
    {
        std::string modulePathPlugins1  = "/Users/kaestner/git/deployed/MuhRec.app/Contents/PlugIns/BackProjectors/libStdBackProjectors.dylib";
        std::string modulePathPlugins2  = "/Users/kaestner/git/deployed/MuhRec.app/Contents/MacOS/../PlugIns/BackProjectors/libStdBackProjectors.dylib";
        ModuleLibNameManager mlnm_plugins(appPath,true,category);

        std::string name=mlnm_plugins.stripLibName(modulePathPlugins1,kipl::base::OSMacOS);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name=mlnm_plugins.stripLibName(modulePathPlugins2,kipl::base::OSMacOS);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name = mlnm_plugins.generateLibName("StdBackProjectors", kipl::base::OSMacOS);
        QCOMPARE(name,modulePathPlugins1);
        

        // Test plugins outside the app bundle
        QCOMPARE(mlnm_plugins.stripLibName(modulePathExt,kipl::base::OSMacOS),modulePathExt);
        QCOMPARE(mlnm_plugins.generateLibName(modulePathExt,kipl::base::OSMacOS),modulePathExt);

        std::string path = mlnm_plugins.generateLibPath(kipl::base::OSMacOS);
        QCOMPARE(path,std::string("/Users/kaestner/git/deployed/MuhRec.app/Contents/PlugIns/BackProjectors/"));
    }
}

void ConfigBaseTest::testLibNameManagerLinux()
{
    std::string appPath       = "/home/kaestner/build-imagingsuite/Release/bin";
    std::string category      = "BackProjectors";
    std::string modulePathExt = "/Users/kaestner/libStdBackProjectors.1.0.0.so";

    //
    // Tests with standard path
    //
    {
        std::string modulePathStd1 = "/home/kaestner/build-imagingsuite/Release/lib/libStdBackProjectors.so";
        std::string modulePathStd2 = "/home/kaestner/build-imagingsuite/Release/bin/../lib/libStdBackProjectors.so";

        ModuleLibNameManager mlnm_std(appPath,false,category);

        std::string name=mlnm_std.stripLibName(modulePathStd1,kipl::base::OSLinux);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name=mlnm_std.stripLibName(modulePathStd2,kipl::base::OSLinux);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name = mlnm_std.generateLibName("StdBackProjectors", kipl::base::OSLinux);
        QCOMPARE(name,modulePathStd1);
        
        // Test plugins outside the app bundle
        QCOMPARE(mlnm_std.stripLibName(modulePathExt,kipl::base::OSLinux),modulePathExt);
        QCOMPARE(mlnm_std.generateLibName(modulePathExt,kipl::base::OSLinux),modulePathExt);

        std::string path = mlnm_std.generateLibPath(kipl::base::OSLinux);
        QCOMPARE(path,std::string("/home/kaestner/build-imagingsuite/Release/lib/"));
    }
    //
    // Tests with plugin path
    //
    {
        std::string modulePathPlugins1  = "/home/kaestner/build-imagingsuite/Release/PlugIns/BackProjectors/libStdBackProjectors.so";
        std::string modulePathPlugins2  = "/home/kaestner/build-imagingsuite/Release/bin/../PlugIns/BackProjectors/libStdBackProjectors.so";
        ModuleLibNameManager mlnm_plugins(appPath,true,category);

        std::string name=mlnm_plugins.stripLibName(modulePathPlugins1,kipl::base::OSLinux);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name=mlnm_plugins.stripLibName(modulePathPlugins2,kipl::base::OSLinux);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name = mlnm_plugins.generateLibName("StdBackProjectors", kipl::base::OSLinux);
        QCOMPARE(name,modulePathPlugins1);
        

        // Test plugins outside the app bundle
        QCOMPARE(mlnm_plugins.stripLibName(modulePathExt,kipl::base::OSLinux),modulePathExt);
        QCOMPARE(mlnm_plugins.generateLibName(modulePathExt,kipl::base::OSLinux),modulePathExt);

        std::string path = mlnm_plugins.generateLibPath(kipl::base::OSLinux);
        QCOMPARE(path,std::string("/home/kaestner/build-imagingsuite/Release/PlugIns/BackProjectors/"));
    }

}

void ConfigBaseTest::testLibNameManagerWindows()
{
    std::string appPath       = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\";
    std::string category      = "BackProjectors";
    std::string modulePathExt = "C:\\Users\\kaestner\\StdBackProjectors.dll";

    //
    // Tests with standard path
    //
    {
        std::string modulePathStd1 = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\StdBackProjectors.dll";

        ModuleLibNameManager mlnm_std(appPath,false,category);

        std::string name=mlnm_std.stripLibName(modulePathStd1,kipl::base::OSWindows);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name = mlnm_std.generateLibName("StdBackProjectors", kipl::base::OSWindows);
        QCOMPARE(name,modulePathStd1);
        
        // Test plugins outside the app bundle
        QCOMPARE(mlnm_std.stripLibName(modulePathExt,kipl::base::OSWindows),modulePathExt);
        QCOMPARE(mlnm_std.generateLibName(modulePathExt,kipl::base::OSWindows),modulePathExt);

        std::string path = mlnm_std.generateLibPath(kipl::base::OSWindows);
        QCOMPARE(path,std::string("C:\\Users\\kaestner\\git\\deployed\\muhrec\\"));
    }
    //
    // Tests with plugin path
    //
    {
        std::string modulePathPlugins1  = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\PlugIns\\BackProjectors\\StdBackProjectors.dll";
        ModuleLibNameManager mlnm_plugins(appPath,true,category);

        std::string name=mlnm_plugins.stripLibName(modulePathPlugins1,kipl::base::OSWindows);
        QCOMPARE(name,std::string("StdBackProjectors"));

        name = mlnm_plugins.generateLibName("StdBackProjectors", kipl::base::OSWindows);
        QCOMPARE(name,modulePathPlugins1);
        

        // Test plugins outside the app bundle
        QCOMPARE(mlnm_plugins.stripLibName(modulePathExt,kipl::base::OSWindows),modulePathExt);
        QCOMPARE(mlnm_plugins.generateLibName(modulePathExt,kipl::base::OSWindows),modulePathExt);

        std::string path = mlnm_plugins.generateLibPath(kipl::base::OSWindows);
        QCOMPARE(path,std::string("C:\\Users\\kaestner\\git\\deployed\\muhrec\\PlugIns\\BackProjectors\\"));
    }


}

// #ifdef __APPLE__
//     #pragma clang diagnostic push
//     #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
//     QTEST_APPLESS_MAIN(ConfigBaseTest)
//     #pragma clang diagnostic pop
// #else
    QTEST_APPLESS_MAIN(ConfigBaseTest)
// #endif


#include "tst_configbasetest.moc"

//<LICENSE>
#include <QString>
#include <QtTest>
#include <QDebug>

#include <ModuleException.h>
#include <ReconConfig.h>
#include <modulelibnamemanger.h>

class ConfigBaseTest : public QObject
{
    Q_OBJECT

public:
    ConfigBaseTest();

private Q_SLOTS:
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

void ConfigBaseTest::testConfigChanged()
{
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

    config.EvalArg(arg, group, var, value);

    QVERIFY2(group=="userinformation","Extract group");
    QVERIFY2(var=="sample",var.c_str());
    QVERIFY2(value=="mouse",value.c_str());

    // Provoke no value
    arg="userinformation:sample";
    group.clear(); var.clear(); value.clear();
    QVERIFY_EXCEPTION_THROWN(config.EvalArg(arg,group,var,value), ModuleException);

    arg="userinformationsample";
    group.clear(); var.clear(); value.clear();
    QVERIFY_EXCEPTION_THROWN(config.EvalArg(arg,group,var,value), ModuleException);

    arg="projections:roi=40 40 50 50";
    config.EvalArg(arg, group, var, value);

    QVERIFY2(group=="projections","Extract group");
    QVERIFY2(var=="roi",var.c_str());
    QVERIFY2(value=="40 40 50 50",value.c_str());

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

    config.GetCommandLinePars(args);
}

void ConfigBaseTest::testLibNameManagerMac()
{
    std::string appPath    = "/Users/kaestner/git/deployed/MuhRec.app/Contents/MacOS/";
    std::string modulePath = "/Users/kaestner/git/deployed/MuhRec.app/Contents/Frameworks/libStdBackProjectors.1.0.0.dylib";

    ModuleLibNameManger mlnm(appPath);

    QCOMPARE(mlnm.stripLibName(modulePath,kipl::base::OSMacOS),"StdBackProjectors");

    QCOMPARE(mlnm.generateLibName("StdBackProjectors",kipl::base::OSMacOS),modulePath);

    std::string modulePath2 = "/Users/kaestner/libStdBackProjectors.1.0.0.dylib";

    QCOMPARE(mlnm.stripLibName(modulePath2,kipl::base::OSMacOS),modulePath2);

    QCOMPARE(mlnm.generateLibName(modulePath2,kipl::base::OSMacOS),modulePath2);


}

void ConfigBaseTest::testLibNameManagerLinux()
{
    std::string appPath    = "/Users/kaestner/git/deployed/muhrec/";
    std::string modulePath = "/Users/kaestner/git/deployed/muhrec/Frameworks/libStdBackProjectors.so.1.0.0";

    ModuleLibNameManger mlnm(appPath);

    QCOMPARE(mlnm.stripLibName(modulePath,kipl::base::OSLinux),"StdBackProjectors");

    QCOMPARE(mlnm.generateLibName("StdBackProjectors",kipl::base::OSLinux),modulePath);

    std::string modulePath2 = "/Users/kaestner/libStdBackProjectors.so.1.0.0";

    QCOMPARE(mlnm.stripLibName(modulePath2,kipl::base::OSLinux),modulePath2);

    QCOMPARE(mlnm.generateLibName(modulePath2,kipl::base::OSLinux),modulePath2);
}

void ConfigBaseTest::testLibNameManagerWindows()
{
    std::string appPath    = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\";
    std::string modulePath = "C:\\Users\\kaestner\\git\\deployed\\muhrec\\StdPreprocModules.dll";

    ModuleLibNameManger mlnm(appPath);

    QCOMPARE(mlnm.stripLibName(modulePath,kipl::base::OSWindows),"StdPreprocModules");

    QCOMPARE(mlnm.generateLibName("StdPreprocModules",kipl::base::OSWindows),modulePath);

    std::string modulePath2 = "C:\\Users\\kaestner\\muhrec\\StdPreprocModules.dll";

    QCOMPARE(mlnm.stripLibName(modulePath2,kipl::base::OSWindows),modulePath2);

    QCOMPARE(mlnm.generateLibName(modulePath2,kipl::base::OSWindows),modulePath2);


}

QTEST_APPLESS_MAIN(ConfigBaseTest)

#include "tst_configbasetest.moc"

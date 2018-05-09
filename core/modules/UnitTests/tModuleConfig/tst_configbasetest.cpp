#include <QString>
#include <QtTest>
#include <QDebug>

#include <ModuleException.h>
#include <ReconConfig.h>
#include <applicationconfig.h>
#include <strings/filenames.h>

class ConfigBaseTest : public QObject
{
    Q_OBJECT

public:
    ConfigBaseTest();

private Q_SLOTS:
    void testConfigChanged();
    void testEvalArg();
    void testGetCommandLinePars();
    void testAppCfgCTor();
    void testAppCfgStream();
    void testAppCfgPaths();
};

ConfigBaseTest::ConfigBaseTest()
{
}

void ConfigBaseTest::testConfigChanged()
{
        ReconConfig a,b;
        std::list<std::string> freelist;
        cout<<"start"<<endl;

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

        ReconConfig c,d;
        QVERIFY2(c.ConfigChanged(d,freelist) == false , "Two parameters in the free list, same config");
}

void ConfigBaseTest::testEvalArg()
{
    std::string arg;
    std::string group;
    std::string var;
    std::string value;

    arg="userinformation:sample=mouse";
    ReconConfig config;

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

    ReconConfig config;

  //  config.GetCommandLinePars(args);
}

void ConfigBaseTest::testAppCfgCTor()
{
    ApplicationConfig c0("muhrec");
    std::string p; p+=kipl::strings::filenames::slash;
    std::string ver=VERSION;

    QCOMPARE(c0.applicationPath,p);
    QCOMPARE(c0.dataPath,p);
    QCOMPARE(c0.memory,8192);
    QCOMPARE(c0.getApplicationVersion(),ver);
    QCOMPARE(c0.getConfigVersion(),ver);
    QCOMPARE(c0.getAppName(),std::string("muhrec"));

    c0.applicationPath="/Users/App/";
    c0.dataPath="/projects/data";
    c0.memory=4096;

    ApplicationConfig c1=c0;

    QCOMPARE(c0.applicationPath,c1.applicationPath);
    QCOMPARE(c0.dataPath,c1.dataPath);
    QCOMPARE(c0.memory,c1.memory);
    QCOMPARE(c0.getAppName(),c1.getAppName());

    ApplicationConfig c2;

    c2=c1;
    QCOMPARE(c2.applicationPath,c1.applicationPath);
    QCOMPARE(c2.dataPath,c1.dataPath);
    QCOMPARE(c2.memory,c1.memory);
    QCOMPARE(c2.getAppName(),c1.getAppName());
}

void ConfigBaseTest::testAppCfgStream()
{
    ApplicationConfig c0;

    std::string ver=VERSION;

    c0.applicationPath="/Users/App/";
    c0.dataPath="/projects/data/";
    c0.memory=4096;

    std::string xml=c0.streamXML();

    ApplicationConfig c1;
    try {
        c1.parseXML(xml);
    }
    catch (ModuleException &e) {
        qDebug() << e.what().c_str();
    }

    QCOMPARE(c1.applicationPath,c0.applicationPath);
    QCOMPARE(c1.dataPath,c0.dataPath);
    QCOMPARE(c1.memory,c0.memory);

}

void ConfigBaseTest::testAppCfgPaths()
{
    ApplicationConfig c0;

    c0.applicationPath="/Users/App/";
    c0.dataPath="/projects/data/";
    c0.memory=4096;
    std::string tmp=c0.applicationPath+"libkipl.dylib";

    QCOMPARE(c0.updatePath("@applicationpath/libkipl.dylib"),c0.applicationPath+"libkipl.dylib");
    QCOMPARE(c0.updatePath("@datapath/p123"),c0.dataPath+"p123");
    QCOMPARE(c0.updatePath("/deeperfolder/@datapath/p123"),std::string("/deeperfolder/")+c0.dataPath+"p123");
}

QTEST_APPLESS_MAIN(ConfigBaseTest)

#include "tst_configbasetest.moc"

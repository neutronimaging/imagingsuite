#include <QString>
#include <QtTest>

#include <ReconConfig.h>

class ConfigBaseTest : public QObject
{
    Q_OBJECT

public:
    ConfigBaseTest();

private Q_SLOTS:
    void testConfigChanged();
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

QTEST_APPLESS_MAIN(ConfigBaseTest)

#include "tst_configbasetest.moc"

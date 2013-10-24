#include <QtCore/QString>
#include <QtTest/QtTest>
#include <MorphSpotClean.h>
#include <base/timage.h>

class TestMorphSpotClean : public QObject
{
    Q_OBJECT
    
public:
    TestMorphSpotClean();
    
private Q_SLOTS:
    void testCase1();
    void testCase1_data();
};

TestMorphSpotClean::TestMorphSpotClean()
{
}

void TestMorphSpotClean::testCase1()
{
    QCOMPARE(true,false);
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void TestMorphSpotClean::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_APPLESS_MAIN(TestMorphSpotClean)

#include "tst_testmorphspotclean.moc"

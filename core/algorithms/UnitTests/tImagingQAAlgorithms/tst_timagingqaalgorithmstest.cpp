#include <QString>
#include <QtTest>

class TImagingQAAlgorithmsTest : public QObject
{
    Q_OBJECT

public:
    TImagingQAAlgorithmsTest();

private Q_SLOTS:
    void testCase1();
};

TImagingQAAlgorithmsTest::TImagingQAAlgorithmsTest()
{
}

void TImagingQAAlgorithmsTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TImagingQAAlgorithmsTest)

#include "tst_timagingqaalgorithmstest.moc"

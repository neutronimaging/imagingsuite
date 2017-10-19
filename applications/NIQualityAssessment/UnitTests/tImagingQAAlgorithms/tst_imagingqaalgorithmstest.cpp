#include <QString>
#include <QtTest>

class ImagingQAAlgorithmsTest : public QObject
{
    Q_OBJECT

public:
    ImagingQAAlgorithmsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1_data();
    void testCase1();
};

ImagingQAAlgorithmsTest::ImagingQAAlgorithmsTest()
{
}

void ImagingQAAlgorithmsTest::initTestCase()
{
}

void ImagingQAAlgorithmsTest::cleanupTestCase()
{
}

void ImagingQAAlgorithmsTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void ImagingQAAlgorithmsTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(ImagingQAAlgorithmsTest)

#include "tst_imagingqaalgorithmstest.moc"

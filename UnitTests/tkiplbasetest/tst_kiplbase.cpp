#include <QString>
#include <QtTest>

#include <base/thistogram.h>
#include <base/timage.h>

class TkiplbasetestTest : public QObject
{
    Q_OBJECT

public:
    TkiplbasetestTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // Tests for TImage
    void testTImageInitialization();

    // Tests for BivariateHistogram
    void testBivariateHistogramInitialize();

    void testBivariateHistogram();
};

TkiplbasetestTest::TkiplbasetestTest()
{
}

void TkiplbasetestTest::initTestCase()
{
}

void TkiplbasetestTest::cleanupTestCase()
{
}

void TkiplbasetestTest::testTImageInitialization()
{
    QVERIFY2(true, "Failure");
}

void TkiplbasetestTest::testBivariateHistogramInitialize()
{
    kipl::base::BivariateHistogram bihi;

    bihi.Initialize(0.0f,1.0f,10,2.0f,3.0f,20);

    auto lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    size_t const *dims=bihi.Dims();
    QCOMPARE(dims[0],10UL);
    QCOMPARE(dims[1],20UL);

    bihi.Initialize(1.0f,0.0f,10,3.0f,2.0f,20);

    lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    size_t const *dims2=bihi.Dims();
    QCOMPARE(dims2[0],10UL);
    QCOMPARE(dims2[1],20UL);
}

void TkiplbasetestTest::testBivariateHistogram()
{
        kipl::base::BivariateHistogram bihi;
        bihi.Initialize(0.0f,10.0f,10,0.0f,20.0f,20);

        kipl::base::TImage<size_t,2> &img=bihi.Bins();

        size_t const * dims=bihi.Dims();
        for (size_t y=0; y<dims[1]; y++)
            for (size_t x=0; x<dims[0]; x++)
                img(x,y)=x+y*dims[0];

        kipl::base::BivariateHistogram::BinInfo info=bihi.GetBin(0.0,0.0);

        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(-1.0,0.0);
        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(1.0,-1.0);
        QCOMPARE(img(1,0),info.count);

        info=bihi.GetBin(10.0,0.0);
        QCOMPARE(img(9,0),info.count);

        info=bihi.GetBin(5.0,10.0);
        QCOMPARE(img(5,10),info.count);

        info=bihi.GetBin(10.0,20.0);
        QCOMPARE(img(9,19),info.count);

}

QTEST_APPLESS_MAIN(TkiplbasetestTest)

#include "tst_kiplbase.moc"

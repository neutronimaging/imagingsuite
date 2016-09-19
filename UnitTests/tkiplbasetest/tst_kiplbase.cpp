#include <QString>
#include <QtTest>

#include <base/thistogram.h>
#include <base/timage.h>
#include <base/imageinfo.h>

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

    // Tests for ImageInformation
    void testImageInfoCtor();
    void testImageInfoResolutions();

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

void TkiplbasetestTest::testImageInfoCtor()
{
    kipl::base::ImageInfo infoA;

    QVERIFY(infoA.sSoftware=="KIPL image processing library");        ///< TIFF tag 305
    QVERIFY(infoA.sArtist=="Anders Kaestner");	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright=="Anders Kaestner");       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription=="");     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample==16);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel==1); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat==0);

    QVERIFY(infoA.GetMetricX()==1.0f);
    QVERIFY(infoA.GetMetricY()==1.0f);

    infoA.sArtist="Pelle";
    infoA.sCopyright="Somebody";
    infoA.sDescription="Hepp";
    infoA.nBitsPerSample=8;
    infoA.nSampleFormat=2;
    infoA.nSamplesPerPixel=3;
    infoA.SetMetricX(10.0f);
    infoA.SetMetricY(20.0f);

    kipl::base::ImageInfo infoB(infoA);

    QVERIFY(infoA.sSoftware         == infoB.sSoftware);        ///< TIFF tag 305
    QVERIFY(infoA.sArtist           == infoB.sArtist);	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright        == infoB.sCopyright);       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription      == infoB.sDescription);     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample    == infoB.nBitsPerSample);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel  == infoB.nSamplesPerPixel); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat     == infoB.nSampleFormat);

    QVERIFY(infoA.GetMetricX()==infoB.GetMetricX());
    QVERIFY(infoA.GetMetricY()==infoB.GetMetricY());

    kipl::base::ImageInfo infoC;
    infoC=infoA;

    QVERIFY(infoA.sSoftware         == infoC.sSoftware);        ///< TIFF tag 305
    QVERIFY(infoA.sArtist           == infoC.sArtist);	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright        == infoC.sCopyright);       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription      == infoC.sDescription);     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample    == infoC.nBitsPerSample);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel  == infoC.nSamplesPerPixel); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat     == infoC.nSampleFormat);

    QVERIFY(infoA.GetMetricX()==infoC.GetMetricX());
    QVERIFY(infoA.GetMetricY()==infoC.GetMetricY());
}

void TkiplbasetestTest::testImageInfoResolutions()
{
    kipl::base::ImageInfo infoA;

    infoA.SetMetricX(10.0f);
    QVERIFY(infoA.GetDPCMX() == 1.0f);
    QVERIFY(infoA.GetDPIX()  == 25.4f/10.0f);

    infoA.SetMetricY(20.0f);
    QVERIFY(infoA.GetDPCMY() == 10.0f/20.0f);
    QVERIFY(infoA.GetDPIY()  == 25.4f/20.0f);

    infoA.SetDPCMX(10.0f);
    QVERIFY(infoA.GetMetricX()  == 1.0f);

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

        kipl::base::BivariateHistogram::BinInfo info=bihi.GetBin(0.0f,0.0f);

        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(-1.0f,0.0f);
        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(1.0f,-1.0f);
        QCOMPARE(img(1,0),info.count);

        info=bihi.GetBin(10.0f,0.0f);
        QCOMPARE(img(9,0),info.count);

        info=bihi.GetBin(5.0f,10.0f);
        QCOMPARE(img(5,10),info.count);

        info=bihi.GetBin(10.0f,20.0f);
        QCOMPARE(img(9,19),info.count);

}

QTEST_APPLESS_MAIN(TkiplbasetestTest)

#include "tst_kiplbase.moc"

#include <QString>
#include <QtTest>

#include <base/TImage.h>
#include <generators/NoiseImage.h>

class TNoiseImageTest : public QObject
{
    Q_OBJECT

public:
    TNoiseImageTest();

private Q_SLOTS:
    void testGauss();
    void testPoisson();
};

TNoiseImageTest::TNoiseImageTest()
{
}

void TNoiseImageTest::testGauss()
{
    QVERIFY2(true, "Failure");

    size_t dims[2]={1000,1000};

    kipl::base::TImage<float,2> img(dims);

    kipl::generators::NoiseGenerator<float,2> noise;

   // noise.Gauss(img,10.0f,1.0f);
}

void TNoiseImageTest::testPoisson()
{

}

QTEST_APPLESS_MAIN(TNoiseImageTest)

#include "tst_tnoiseimagetest.moc"

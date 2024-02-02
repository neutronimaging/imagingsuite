#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <generators/NoiseImage.h>
#include <math/statistics.h>
#include <utilities/compare.h>

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

    std::vector<size_t> dims={1000,1000};

    kipl::base::TImage<float,2> img(dims);
    img = 0.0f;

    kipl::generators::NoiseGenerator<float,2> noise;

    noise.Gauss(img,10.0f,1.0f);

    kipl::math::Statistics stats;
    stats.put(img.GetDataPtr(),img.Size());

    QVERIFY2(fuzzyRelativeCompare(stats.s(),1.0,0.001),  "Standard deviation is not correct ");
    QVERIFY2(fuzzyRelativeCompare(stats.E(),10.0,0.001), "Mean is not correct ");
}

void TNoiseImageTest::testPoisson()
{

}

QTEST_APPLESS_MAIN(TNoiseImageTest)

#include "tst_noiseimage.moc"

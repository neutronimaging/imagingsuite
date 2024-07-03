#include <QString>
#include <QtTest>

#include <string>
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
    std::vector<size_t> dims={1000,1000};

    kipl::base::TImage<float,2> img(dims);
    img = 0.0f;

    kipl::generators::NoiseGenerator<float,2> noise;

    noise.Gauss(img,10.0f,1.0f);

    kipl::math::Statistics stats;
    stats.put(img.GetDataPtr(),img.Size());

    qDebug() << "Mean: " << stats.E();
    qDebug() << "Standard deviation: " << stats.s();

    QVERIFY2(fuzzyRelativeCompare(static_cast<float>(stats.s()),1.0f,0.1f),  "Standard deviation is not correct ");
    QVERIFY2(fuzzyRelativeCompare(stats.E(),10.0,0.001), "Mean is not correct ");
}

void TNoiseImageTest::testPoisson()
{

}

QTEST_APPLESS_MAIN(TNoiseImageTest)

#include "tst_noiseimage.moc"

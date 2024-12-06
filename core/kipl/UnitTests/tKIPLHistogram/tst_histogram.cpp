#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/thistogram.h>
#include <generators/NoiseImage.h>

class HistogramTest : public QObject
{
    Q_OBJECT

public:
    HistogramTest();

private Q_SLOTS:
    void testBaselineVsOpt();
    void benchmarkBaseline();
    void benchmarkOpt();

private:
    void makeTestImage();
    kipl::base::TImage<float,3> img;

};

HistogramTest::HistogramTest()
{
    makeTestImage();
}

void HistogramTest::makeTestImage()
{
    img = kipl::base::TImage<float,3>({1000,1000,10});
    kipl::generators::NoiseGenerator<float,3> noise;
    noise.Gauss(img, 0.0, 1.0);
}

void HistogramTest::testBaselineVsOpt()
{
    const size_t nBins = 500;
    std::vector<size_t> hist(nBins);
    std::vector<float> axis(nBins);
    kipl::base::Histogram(img.GetDataPtr(), img.Size(), nBins, hist, axis);
    
    std::vector<size_t> hist2(nBins);
    std::vector<float> axis2(nBins);
    kipl::base::HistogramOpt(img.GetDataPtr(), img.Size(), nBins, hist2, axis2);

    for (size_t i = 0; i < nBins; ++i)
    {
        QCOMPARE(hist[i],hist2[i]);
        QCOMPARE(axis[i],axis2[i]);
    }
}

void HistogramTest::benchmarkBaseline()
{
    QBENCHMARK
    {
        const size_t nBins = 500;
        std::vector<size_t> hist(nBins);
        std::vector<float> axis(nBins);
        kipl::base::Histogram(img.GetDataPtr(), img.Size(), nBins, hist, axis,-2.0,2.0,true);
    }
}

void HistogramTest::benchmarkOpt()
{
    QBENCHMARK
    {
        const size_t nBins = 500;
        std::vector<size_t> hist(nBins);
        std::vector<float> axis(nBins);
        kipl::base::HistogramOpt(img.GetDataPtr(), img.Size(), nBins, hist, axis,-2.0,2.0,true,4192);
    }
}


#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(HistogramTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(HistogramTest)
#endif



#include "tst_histogram.moc"



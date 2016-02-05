#include <iostream>
#include <vector>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <filters/nonlocalmeans.h>


class TKiplAdvFiltersTest : public QObject
{
    Q_OBJECT

public:
    TKiplAdvFiltersTest();

private Q_SLOTS:
    void HistogramBin_ctor();
    void HistogramBin_container();
    void NLMeans_process();

};

TKiplAdvFiltersTest::TKiplAdvFiltersTest()
{
}

void TKiplAdvFiltersTest::HistogramBin_ctor()
{
    akipl::HistogramBin bin;
    QCOMPARE(bin.cnt,0UL);
    QCOMPARE(bin.sum,0.0);
    QCOMPARE(bin.bin,0.0);
    QCOMPARE(bin.local_avg,0.0);

    bin.cnt=10UL;
    bin.sum=100.0;
    bin.bin=123.0;
    bin.local_avg=11.0;

    akipl::HistogramBin bin2(bin);
    QCOMPARE(bin2.cnt,bin.cnt);
    QCOMPARE(bin2.sum,bin.sum);
    QCOMPARE(bin2.bin,bin.bin);
    QCOMPARE(bin2.local_avg,bin.local_avg);

    akipl::HistogramBin bin3=bin;
    QCOMPARE(bin3.cnt,bin.cnt);
    QCOMPARE(bin3.sum,bin.sum);
    QCOMPARE(bin3.bin,bin.bin);
    QCOMPARE(bin3.local_avg,bin.local_avg);

    akipl::HistogramBin bin4;
    bin4=bin;
    QCOMPARE(bin4.cnt,bin.cnt);
    QCOMPARE(bin4.sum,bin.sum);
    QCOMPARE(bin4.bin,bin.bin);
    QCOMPARE(bin4.local_avg,bin.local_avg);
}

void TKiplAdvFiltersTest::HistogramBin_container()
{
    std::vector<akipl::HistogramBin> vec(3);

    QCOMPARE(vec[0].cnt,0UL);
    QCOMPARE(vec[0].sum,0.0);
    QCOMPARE(vec[0].local_avg,0.0);
    QCOMPARE(vec[0].bin,0.0);

    vec[1].cnt=10UL;
    vec[1].sum=100.0;
    vec[1].bin=123.0;
    vec[1].local_avg=11.0;

    QCOMPARE(vec[1].cnt,10UL);
    QCOMPARE(vec[1].sum,100.0);
    QCOMPARE(vec[1].local_avg,11.0);
    QCOMPARE(vec[1].bin,123.0);

    std::vector<akipl::HistogramBin> vec2=vec;

    QCOMPARE(vec[1].cnt,vec2[1].cnt);
    QCOMPARE(vec[1].sum,vec2[1].sum);
    QCOMPARE(vec[1].local_avg,vec2[1].local_avg);
    QCOMPARE(vec[1].bin,vec2[1].bin);
}

void TKiplAdvFiltersTest::NLMeans_process()
{
    kipl::base::TImage<float,2> img, res;
    kipl::io::ReadTIFF(img,"../data/scroll_256.tif");
    kipl::io::WriteTIFF(img,"orig_scroll.tif");


    akipl::NonLocalMeans nlfilter(11,
                                  5000.0f,
                                  8192,
                                  akipl::NonLocalMeans::NLM_window_gauss,
                                  akipl::NonLocalMeans::NLM_HistogramSumParallel);

    QBENCHMARK {
        nlfilter(img,res);
    }

    kipl::io::WriteTIFF32(res,"nl_scroll.tif");

}

QTEST_APPLESS_MAIN(TKiplAdvFiltersTest)

#include "tst_tkipladvfilterstest.moc"

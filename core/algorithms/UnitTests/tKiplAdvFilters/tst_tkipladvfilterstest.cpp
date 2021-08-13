#include <iostream>
#include <vector>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <nonlocalmeans.h>
#include <base/KiplException.h>


class TAdvancedFiltersTest : public QObject
{
    Q_OBJECT

public:
    TAdvancedFiltersTest();

private Q_SLOTS:
    void HistogramBin_ctor();
    void HistogramBin_container();
    void NLMeans_WindowEnum();
    void NLMeans_AlgorithmEnum();
    void NLMeans_process();

};

TAdvancedFiltersTest::TAdvancedFiltersTest()
{
}

void TAdvancedFiltersTest::HistogramBin_ctor()
{
    advancedfilters::HistogramBin bin;
    QCOMPARE(bin.cnt,0UL);
    QCOMPARE(bin.sum,0.0);
    QCOMPARE(bin.bin,0.0);
    QCOMPARE(bin.local_avg,0.0);

    bin.cnt=10UL;
    bin.sum=100.0;
    bin.bin=123.0;
    bin.local_avg=11.0;

    advancedfilters::HistogramBin bin2(bin);
    QCOMPARE(bin2.cnt,bin.cnt);
    QCOMPARE(bin2.sum,bin.sum);
    QCOMPARE(bin2.bin,bin.bin);
    QCOMPARE(bin2.local_avg,bin.local_avg);

    advancedfilters::HistogramBin bin3=bin;
    QCOMPARE(bin3.cnt,bin.cnt);
    QCOMPARE(bin3.sum,bin.sum);
    QCOMPARE(bin3.bin,bin.bin);
    QCOMPARE(bin3.local_avg,bin.local_avg);

    advancedfilters::HistogramBin bin4;
    bin4=bin;
    QCOMPARE(bin4.cnt,bin.cnt);
    QCOMPARE(bin4.sum,bin.sum);
    QCOMPARE(bin4.bin,bin.bin);
    QCOMPARE(bin4.local_avg,bin.local_avg);
}

void TAdvancedFiltersTest::HistogramBin_container()
{
    std::vector<advancedfilters::HistogramBin> vec(3);

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

    std::vector<advancedfilters::HistogramBin> vec2=vec;

    QCOMPARE(vec[1].cnt,vec2[1].cnt);
    QCOMPARE(vec[1].sum,vec2[1].sum);
    QCOMPARE(vec[1].local_avg,vec2[1].local_avg);
    QCOMPARE(vec[1].bin,vec2[1].bin);
}

void TAdvancedFiltersTest::NLMeans_WindowEnum()
{
    QCOMPARE(enum2string(advancedfilters::NonLocalMeans::NLMwindows::NLM_window_sum),std::string("NLM_window_sum"));
    QCOMPARE(enum2string(advancedfilters::NonLocalMeans::NLMwindows::NLM_window_avg),std::string("NLM_window_avg"));
    QCOMPARE(enum2string(advancedfilters::NonLocalMeans::NLMwindows::NLM_window_gauss),std::string("NLM_window_gauss"));
    QCOMPARE(enum2string(advancedfilters::NonLocalMeans::NLMwindows::NLM_window_buades),std::string("NLM_window_buades"));
//    QVERIFY_EXCEPTION_THROWN(enum2string(static_cast<advancedfilters::NonLocalMeans::NLMwindows>(100000)),kipl::base::KiplException);
}

void TAdvancedFiltersTest::NLMeans_AlgorithmEnum()
{

}

void TAdvancedFiltersTest::NLMeans_process()
{
    QSKIP("Development test");
    kipl::base::TImage<float,2> img, res;
    kipl::io::ReadTIFF(img,"../data/scroll_256.tif");
//    kipl::io::ReadTIFF(img,"../data/lena256x256_8bit.tif");
    kipl::io::WriteTIFF(img,"orig_test.tif");


//    advancedfilters::NonLocalMeans nlfilter(11,
//                                  5000.0f,
//                                  8192,
//                                  advancedfilters::NonLocalMeans::NLMwindows::NLM_window_gauss,
//                                  advancedfilters::NonLocalMeans::NLMalgorithms::NLM_HistogramSumParallel);

    advancedfilters::NonLocalMeans nlfilter(11,
                                  200.0f,
                                  2048,
                                  advancedfilters::NonLocalMeans::NLMwindows::NLM_window_gauss,
                                  advancedfilters::NonLocalMeans::NLMalgorithms::NLM_Bivariate);

    QBENCHMARK {
        nlfilter(img,res);
    }

    kipl::io::WriteTIFF(res,"nl_test.tif",kipl::base::Float32);

}


QTEST_APPLESS_MAIN(TAdvancedFiltersTest);

#include "tst_tkipladvfilterstest.moc"

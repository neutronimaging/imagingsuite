#include <iostream>
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
    void NLMeans_process();
};

TKiplAdvFiltersTest::TKiplAdvFiltersTest()
{
}

void TKiplAdvFiltersTest::NLMeans_process()
{
    kipl::base::TImage<float,2> img, res;
    kipl::io::ReadTIFF(img,"../data/scroll_256.tif");
    kipl::io::WriteTIFF(img,"orig_scroll.tif");


    akipl::NonLocalMeans nlfilter(11,50000.0f,8192,akipl::NonLocalMeans::NLM_HistogramSumParallel);

    QBENCHMARK {
        nlfilter(img,res);
    }

    kipl::io::WriteTIFF32(res,"nl_scroll.tif");

}

QTEST_APPLESS_MAIN(TKiplAdvFiltersTest)

#include "tst_tkipladvfilterstest.moc"

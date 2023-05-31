//<LICENSE>

#include <QtTest>

#include <vector>
#include <string>
#include <filters/savitzkygolayfilter.h>
#include <base/timage.h>
#include <base/kiplenums.h>
#include <io/io_tiff.h>
#include <io/io_fits.h>
#include <strings/filenames.h>

#include <filters/medianfilter.h>

class KiplFilters : public QObject
{
    Q_OBJECT

public:
    KiplFilters();
    ~KiplFilters();

private slots:
    void test_SavGolCoeffs();
    void test_SavGolFilter();
    void test_MedianFilter();
    void benchmark_MedianFilterSingle();
    void benchmark_MedianFilterParallel();

private:
    std::string dataPath;

};

KiplFilters::KiplFilters()
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);


}

KiplFilters::~KiplFilters()
{

}

void KiplFilters::test_SavGolCoeffs()
{
    kipl::filters::SavitzkyGolayFilter<double> sg;
    auto res=sg.coeffs(5,2);
    std::vector<double> expres = {-0.0857142857143,  0.342857142857,  0.485714285714,  0.342857142857, -0.0857142857143};

    QCOMPARE(res.size(),expres.size());

    for (auto itA=res.begin(),itB=expres.begin(); itA!=res.end(); ++itA, ++itB)
    {
        QVERIFY(fabs(*itA-*itB)<1e-5);
    }


    res = sg.coeffs(5, 2, 1);
    std::vector<double> expres2 = { -2.00000000e-01,  -1.00000000e-01,  -2.02197100567e-16, 1.00000000e-01, 2.00000000e-01};
    QCOMPARE(res.size(),expres2.size());
    for (auto itA=res.begin(),itB=expres2.begin(); itA!=res.end(); ++itA, ++itB)
    {
       QVERIFY(fabs(*itA-*itB)<1e-5);
    }
}

void KiplFilters::test_SavGolFilter()
{
    std::vector<double> x = {2, 2, 5, 2, 1, 0, 1, 4, 9};
    kipl::filters::SavitzkyGolayFilter<double> sg;
    std::vector<double> res;

    try
    {
        res = sg(x, 5, 2);
    }
    catch (kipl::base::KiplException &e)
    {
        QFAIL(e.what());
    }
    catch (std::exception &e)
    {
        QFAIL(e.what()) ;
    }
    std::vector<double> expres= {1.65714285714, 3.17143, 3.54286, 2.85714, 0.657143, 0.171429, 1.  , 4.  , 9.};

    QCOMPARE(res.size(),expres.size());
    for (auto itA=res.begin(),itB=expres.begin(); itA!=res.end(); ++itA, ++itB)
    {
      QVERIFY(fabs(*itA-*itB)<1e-5);
      // QCOMPARE is too sensitive for the test data
    }
}

void KiplFilters::test_MedianFilter()
{
    kipl::base::TImage<float,2> img;

    std::string fname = dataPath+"2D/tiff/scroll_256.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);

    kipl::filters::TMedianFilter<float,2> med({5,5});

    med.medianAlgorithm = kipl::filters::MedianAlg_HeapSortMedian;
    auto res_single   = med(img,kipl::filters::FilterBase::EdgeZero);

    med.medianAlgorithm = kipl::filters::MedianAlg_HeapSortMedianSTL;
    auto res_parallel = med(img,kipl::filters::FilterBase::EdgeZero);

    for (size_t i =0; i<res_single.Size(); ++i)
        QCOMPARE(res_single[i],res_parallel[i]);

}

void KiplFilters::benchmark_MedianFilterSingle()
{
    // QSKIP("Takes too long");
    kipl::base::TImage<float,2> img;

    std::string fname = dataPath+"2D/tiff/scroll_1024.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);
    // std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    // kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);

    kipl::filters::TMedianFilter<float,2> med({71,1});

    med.medianAlgorithm = kipl::filters::MedianAlg_HeapSortMedian;
    QBENCHMARK
    {
        auto res_single   = med(img,kipl::filters::FilterBase::EdgeZero);
    }
}

void KiplFilters::benchmark_MedianFilterParallel()
{
    kipl::base::TImage<float,2> img;

    std::string fname = dataPath+"2D/tiff/scroll_1024.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);
    // std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    // kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);
    kipl::filters::TMedianFilter<float,2> med({71,1});

    med.medianAlgorithm = kipl::filters::MedianAlg_HeapSortMedianSTL;
    QBENCHMARK
    {
        auto res_parallel = med(img,kipl::filters::FilterBase::EdgeZero);
    }
}

QTEST_APPLESS_MAIN(KiplFilters)

#include "tst_kiplfilters.moc"

//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_csv.h>

#include <StripeFilter.h>
#include <ImagingException.h>

class TestStripeFilter : public QObject
{
    Q_OBJECT
    
public:
    TestStripeFilter();
    
private Q_SLOTS:
    void StripeFilterParameters();
    void StripeFilterProcessing2D();

private:
    void MorphSpotClean_ListAlgorithm();
private:
    std::string dataPath;
};

TestStripeFilter::TestStripeFilter() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    #ifdef __APPLE__
        dataPath = dataPath + "/../../../../../../TestData/";
    #elif defined(__linux__)
        dataPath = dataPath + "/../../../../../../TestData/";
    #else
        dataPath = dataPath + "/../../../../../TestData/";
    #endif
    
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);

    // std::string fname = dataPath+"2D/tiff/spots/balls.tif";
    // kipl::strings::filenames::CheckPathSlashes(fname,false);
    // kipl::io::ReadTIFF(holes,fname);
}

void TestStripeFilter::StripeFilterParameters()
{
   kipl::base::TImage<float,2> sino;
#ifdef DEBUG
   kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#else
   kipl::io::ReadTIFF(sino,dataPath+"2D/tiff/woodsino_0200.tif");
#endif
   ImagingAlgorithms::StripeFilter sf(sino.dims(),"daub17",4,0.21f);
   QCOMPARE(static_cast<size_t>(sf.dims()[0]),sino.Size(0));
   QCOMPARE(static_cast<size_t>(sf.dims()[1]),sino.Size(1));
   QCOMPARE(sf.sigma(),0.21f);
   QCOMPARE(sf.decompositionLevels(),4);
   std::vector<size_t> dims={sino.Size(0),sino.Size(1)};
   QCOMPARE(sf.checkDims(dims),true);
   dims[0]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));
   dims[1]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));
   dims[0]++;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf.checkDims(dims));

   std::vector<int> dims2={static_cast<int>(sino.Size(0)), static_cast<int>(sino.Size(1))};

   ImagingAlgorithms::StripeFilter sf2(dims2,"daub17",4,0.21f);
   QCOMPARE(static_cast<size_t>(sf2.dims()[0]),sino.Size(0));
   QCOMPARE(static_cast<size_t>(sf2.dims()[1]),sino.Size(1));
   QCOMPARE(sf2.sigma(),0.21f);
   QCOMPARE(sf2.decompositionLevels(),4);
   dims[0]=sino.Size(0);
   dims[1]=sino.Size(1);
   QCOMPARE(sf2.checkDims(dims),true);
   dims[0]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));
   dims[1]--;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));
   dims[0]++;
   QVERIFY_THROWS_EXCEPTION(ImagingException,sf2.checkDims(dims));

}

void TestStripeFilter::StripeFilterProcessing2D()
{
    kipl::base::TImage<float,2> sino;

    std::string fname = dataPath+"2D/tiff/woodsino_0200.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadTIFF(sino,fname);

    ImagingAlgorithms::StripeFilter sf(sino.dims(),"daub17",4,0.21f);

    sf.process(sino);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
QTEST_APPLESS_MAIN(TestStripeFilter)
#pragma clang diagnostic pop

#include "tst_StripeFilter.moc"

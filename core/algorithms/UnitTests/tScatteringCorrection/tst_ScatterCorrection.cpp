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

#include <ImagingException.h>
#include <SegmentBB.h>
#include <ScatterEstimation.h>


class TestScatterCorrection : public QObject
{
    Q_OBJECT
    
public:
    TestScatterCorrection();
    
private Q_SLOTS:
    void SegmentBB_initialization();
    void SegmentBB_enums();
    void SegmentBB_segmentation();

    // void ScatterEstimation_initialization();
    void ScatterEstimation_enums();
    void ScatterEstimation_fit();
    void ScatterEstimation_predict();

private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    std::map<size_t,float> points;
    size_t pos1;
    size_t pos2;
};

TestScatterCorrection::TestScatterCorrection() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}


void TestScatterCorrection::SegmentBB_initialization()
{
    QSKIP("Not implemented");
}

void TestScatterCorrection::SegmentBB_enums()
{
    QSKIP("Not implemented");
}

void TestScatterCorrection::SegmentBB_segmentation()
{
    SegmentBB bb_seg;
    kipl::base::TImage<float,2> img;
    
    std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    //std::string fname = dataPath+"2D/fits/BB/bbsample_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);

    kipl::base::TImage<float,2> res(img.dims()); 
    bb_seg.exec(img,res,{200UL,300UL,1900UL,1900UL});

    kipl::io::WriteTIFF(res,"res.tif",kipl::base::Float32);

}

void TestScatterCorrection::ScatterEstimation_fit()
{
    std::ostringstream msg;
    SegmentBB bb_seg;
    kipl::base::TImage<float,2> img;
    
    std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    //std::string fname = dataPath+"2D/fits/BB/bbsample_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);

    kipl::base::TImage<float,2> res(img.dims()); 
    bb_seg.exec(img,res,{200UL,300UL,1900UL,1900UL});

    auto [x,y] = bb_seg.dotCoordinates();
    ScatterEstimator se;  
    se.fit(x,y,img,5,2,2);

    qDebug()<<"Fit error ="<<se.fitError();

}

void TestScatterCorrection::ScatterEstimation_predict()
{

}
// void TestScatterCorrection::ScatterEstimation_initialization()
// {

// }

void TestScatterCorrection::ScatterEstimation_enums()
{
    ScatterEstimator::eAverageMethod mA;
    string2enum("avgmean",mA);
    QCOMPARE(mA, ScatterEstimator::avg_mean);
    string2enum("avgmedian",mA);
    QCOMPARE(mA, ScatterEstimator::avg_median);
    string2enum("avgmin",mA);
    QCOMPARE(mA, ScatterEstimator::avg_min);
    string2enum("avgmax",mA);
    QCOMPARE(mA, ScatterEstimator::avg_max);

    QCOMPARE(enum2string(ScatterEstimator::avg_mean),"avgmean");
    QCOMPARE(enum2string(ScatterEstimator::avg_median),"avgmedian");
    QCOMPARE(enum2string(ScatterEstimator::avg_min),"avgmin");
    QCOMPARE(enum2string(ScatterEstimator::avg_max),"avgmax");

    ScatterEstimator::eFitMethod mF;
    
    string2enum("fitpolynimal",mF);
    QCOMPARE(mF,ScatterEstimator::fitmethod_polynomial);
    string2enum("fitspline",mF);
    QCOMPARE(mF,ScatterEstimator::fitmethod_thinplatesplines);

    QCOMPARE(enum2string(ScatterEstimator::fitmethod_polynomial),"fitpolynimal");
    QCOMPARE(enum2string(ScatterEstimator::fitmethod_thinplatesplines),"fitspline");
}



QTEST_APPLESS_MAIN(TestScatterCorrection)

#include "tst_ScatterCorrection.moc"

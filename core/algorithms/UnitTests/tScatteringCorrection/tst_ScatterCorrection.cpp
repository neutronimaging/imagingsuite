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
#include <ScatteringCorrection.h>

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
    void ScatterEstimation_dose();

    void Normalize_basic();

    void ScatterCorrection_SetRefs();

private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    std::map<size_t,float> points;
    size_t pos1;
    size_t pos2;

    kipl::base::TImage<float,2> sample;
    kipl::base::TImage<float,2> ob;
    kipl::base::TImage<float,2> dc;
    kipl::base::TImage<float,2> bbob;
    kipl::base::TImage<float,2> bbs;

};

TestScatterCorrection::TestScatterCorrection() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);

    std::string fname = dataPath+"2D/fits/BB/sample_0001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadFITS(sample,fname);

    fname = dataPath+"2D/fits/BB/ob_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadFITS(ob,fname);

    fname = dataPath+"2D/fits/BB/dc_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadFITS(bbob,fname);

    fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadFITS(bbob,fname);

    fname = dataPath+"2D/fits/BB/bbsample_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadFITS(bbs,fname);

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

    kipl::base::TImage<float,2> res(img.dims()); 
    bb_seg.exec(bbob,res,{200UL,300UL,1900UL,1900UL});

    kipl::io::WriteTIFF(res,"res.tif",kipl::base::Float32);
}

void TestScatterCorrection::ScatterEstimation_fit()
{
    std::ostringstream msg;
    SegmentBB bb_seg;
    kipl::base::TImage<float,2> img;

    kipl::base::TImage<float,2> res(img.dims()); 
    bb_seg.exec(bbob,res,{200UL,300UL,1900UL,1900UL});
    auto [x,y] = bb_seg.dotCoordinates();
    // std::vector<float> x = { 1806.53, 1435.3, 1062.5, 319.944, 690.995, 
    //                         1808.76, 1064.09, 1436.43, 693.334, 320.784, 
    //                         1809.98, 1437.45, 1065.97, 694.489, 323.123, 
    //                         1811.39, 1439.51, 1068.17, 697.332, 324.943, 
    //                         1813.83, 1441.11, 1069.36, 697.963, 326.948};

    // std::vector<float> y = {  375.315, 376.944, 378.841,  382.358,  381.608, 
    //                           746.751, 750.226, 749.655,  753.382,  753.976, 
    //                          1119.2,  1120.93, 1122.04,  1124.88,  1126.2, 
    //                          1490.89, 1492.34, 1493.66,  1496.03,  1497.88, 
    //                          1861.81, 1864.27, 1864.37,  1868.39,  1868.78};

    // std::vector<float> dots = { 770.259, 708.012, 716.765, 773.111, 745.185, 
    //                             732.518, 893.346, 713.543, 766.802, 1003.36, 
    //                             838.556, 756.444, 831.259, 868.086, 839.148, 
    //                             849.062, 847.518, 1048.54, 901.247, 898.222, 
    //                             1001.42, 1050.63, 928.346, 1106.68, 1054.35};
    msg.str("");
    qDebug()<< x.size();
    qDebug()<< y.size();
    ScatterEstimator se;  
    se.fit(x,y,img,5,2,2);

    qDebug()<<"Fit error ="<<se.fitError();
}

void TestScatterCorrection::ScatterEstimation_predict()
{
    std::vector<float> pars={2.29534e-05f, -0.122711f, 8.18729e-05f, -0.022848f, 1.54186e-05f, 845.537f};

    std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::base::TImage<float,2> img, scatter;
    kipl::io::ReadFITS(img,fname);

    ScatterEstimator se; 

    se.setFitParameters(pars);

    QBENCHMARK 
    {
        scatter = se.scatterImage();
    }

    kipl::io::WriteTIFF(scatter,"scatter.tif", kipl::base::Float32);
}

void TestScatterCorrection::ScatterEstimation_dose()
{
    std::vector<float> pars={2.29534e-05f, -0.122711f, 8.18729e-05f, -0.022848f, 1.54186e-05f, 845.537f};

    kipl::base::TImage<float,2> img, scatter;

    ScatterEstimator se; 

    se.setFitParameters(pars);

    std::vector<size_t> ROI={10,10,100,200};

    scatter = se.scatterImage(ROI);

    QCOMPARE(scatter.Size(0),90);
    QCOMPARE(scatter.Size(1),190);

    float sum=0.0f;
    for (size_t i=0UL; i<scatter.Size(); ++i)
        sum+=scatter[i];

    sum/=scatter.Size();

    float dose = se.scatterDose(ROI);
    QCOMPARE(dose,sum);

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

void TestScatterCorrection::Normalize_basic()
{
}

void TestScatterCorrection::ScatterCorrection_SetRefs()
{
    kipl::base::TImage<float,3> tob({100,100,10});
    kipl::base::TImage<float,3> tdc({100,100,5});
    kipl::base::TImage<float,3> tbbob({100,100,6});
    kipl::base::TImage<float,3> tbbob2({100,10,6});
    kipl::base::TImage<float,3> tbbs({100,100,11});

    ScatteringCorrection sc;

    // Happy path
    sc.setReferences({  {"ob",  tob},
                        {"dc",  tdc},
                        {"bbob",tbbob},
                        {"bbs", tbbs}},
                     {  {"ob",  {1,2,3,4,5,6,7,8,9,10}},
                        {"dc",  {1,2,3,4,5}},
                        {"bbob",{1,2,3,4,5,6}},
                        {"bbs", {1,2,3,4,5,6,7,8,9,10,11}}});

    // Naming mismatch between images and doses
    QVERIFY_EXCEPTION_THROWN(
                            sc.setReferences({  {"ob",tob},
                                                {"ddc",tdc}},
                                             {  {"ob",{1,2,3,4,5,6,7,8,9,10}},
                                                {"dc",{1,2,3,4,5}}});,
                            ImagingException);

    // Number of projections not same as doses
    QVERIFY_EXCEPTION_THROWN(
                            sc.setReferences({  {"ob",tob},
                                                {"dc",tdc}},
                                             {  {"ob",{1,2,3,4,5,6,7,8,9,10}},
                                                {"dc",{2,3,4,5}}});,
                             ImagingException);
    // Image size size mismatch
    QVERIFY_EXCEPTION_THROWN(
                            sc.setReferences({  {"ob",tob},
                                                {"bbob",tbbob2}},
                                             {  {"ob",{1,2,3,4,5,6,7,8,9,10}},
                                                {"bbob",{1,2,3,4,5,6}}});,
                             ImagingException);
}

QTEST_APPLESS_MAIN(TestScatterCorrection)

#include "tst_ScatterCorrection.moc"

#include <sstream>
#include <vector>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <segmentation/gradientguidedthreshold.h>
#include <strings/filenames.h>
#include <io/io_csv.h>

class kiplSegmentationTest : public QObject
{
    Q_OBJECT

public:
    kiplSegmentationTest();

private Q_SLOTS:
//    void testCase1_data();
    void testThreshold();
    void testDoubleThreshold();
    void testMultiThreshold();
    void testGradientGuidedThreshold();
    void testCmpType();
    void testRosin();
    void testOtsu();

private: 
    std::string dataPath;
};

kiplSegmentationTest::kiplSegmentationTest()
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}

void kiplSegmentationTest::testThreshold()
{
    std::vector<size_t> dims = {20,20};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res(dims);

    QVERIFY2(img.Size(0)==res.Size(0),"x-size error");
    QVERIFY2(img.Size(1)==res.Size(1),"y-size error");

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);

    for (float t=0; t<img.Size(); ++t)
    {
        kipl::segmentation::Threshold(img.GetDataPtr(),res.GetDataPtr(),img.Size(),t,kipl::segmentation::cmp_greater);
    
        for (size_t i=0; i<img.Size(); ++i) 
        {
            if (t<i)
                QCOMPARE(res[i],1.0f);
            else
                QCOMPARE(res[i],0.0f);
        }
    }

    for (float t=0; t<img.Size(); ++t)
    {
        kipl::segmentation::Threshold(img.GetDataPtr(),res.GetDataPtr(),img.Size(),t,kipl::segmentation::cmp_less);
    
        for (size_t i=0; i<img.Size(); ++i) 
        {
            if (i<=t)
                QCOMPARE(res[i],1.0f);
            else
                QCOMPARE(res[i],0.0f);
        }
    }

    // todo Implement mask tests
}

void kiplSegmentationTest::testDoubleThreshold()
{
    // QWARN("This test is not implemented.");

}

void kiplSegmentationTest::testMultiThreshold()
{
    std::ostringstream msg;
    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> res;

    std::string fname = dataPath + "2D/tiff/multi_class_reference.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);

    std::vector<float> th={80.0f,150.0f};

    kipl::segmentation::MultiThreshold(img, res, th);

    QVERIFY2(img.Size(0)==res.Size(0),"x-size error");
    QVERIFY2(img.Size(1)==res.Size(1),"y-size error");

    int cnt[3]={0,0,0};
    for (size_t i=0; i<img.Size(); ++i) {
        ++cnt[static_cast<int>(res[i])];
    }

    kipl::io::WriteTIFF(res,"multiseg.tif");
    msg.str(""); msg<<"Back ground count error, got "<<cnt[0];
    QVERIFY2(cnt[0]==49152, msg.str().c_str());
    msg.str(""); msg<<"Back ground count error, got "<<cnt[1];
    QVERIFY2(cnt[1]==12288, msg.str().c_str());
    msg.str(""); msg<<"Back ground count error, got "<<cnt[2];
    QVERIFY2(cnt[2]==4096 , msg.str().c_str());

    // todo-1 Implement class value assignment test
    // todo-2 Implement mask test
}

void kiplSegmentationTest::testGradientGuidedThreshold()
{
//    QFETCH(QString, data);
    std::vector<size_t> dims={100,100};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res(dims);


    std::string fname = dataPath + "2D/tiff/multi_class_smooth.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);

    kipl::segmentation::gradientGuidedThreshold<float,float,2> gs;
    float th[2]={80.0f,150.0f};
    gs.setDataThreshold(th,2);
    //gs.setEdgeThreshold(0.5f);
    gs(img,res);

    QVERIFY2(img.Size(0)==res.Size(0),"x-size error");
    QVERIFY2(img.Size(1)==res.Size(1),"y-size error");

    kipl::io::WriteTIFF(res,"gradientseg.tif");
}

void kiplSegmentationTest::testCmpType()
{
    kipl::segmentation::CmpType c;

    string2enum("cmp_less",c);
    QCOMPARE(c,kipl::segmentation::cmp_less);

    string2enum("cmp_greater",c);
    QCOMPARE(c,kipl::segmentation::cmp_greater);

    string2enum("cmp_less_equal",c);
    QCOMPARE(c,kipl::segmentation::cmp_lesseq);

    string2enum("cmp_greater_equal",c);
    QCOMPARE(c,kipl::segmentation::cmp_greatereq);

    string2enum("cmp_not_equal",c);
    QCOMPARE(c,kipl::segmentation::cmp_noteq);

    string2enum("cmp_equal",c);
    QCOMPARE(c,kipl::segmentation::cmp_eq);

    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, string2enum("cmp",c));

    QCOMPARE(enum2string(kipl::segmentation::cmp_less),std::string("cmp_less"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_greater),std::string("cmp_greater"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_greatereq),std::string("cmp_greater_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_lesseq),std::string("cmp_less_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_noteq),std::string("cmp_not_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_eq),std::string("cmp_equal"));
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException, enum2string(static_cast<kipl::segmentation::CmpType>(20)));

    std::ostringstream s;

    s<<kipl::segmentation::cmp_eq;

    QCOMPARE(s.str().c_str(),"cmp_equal");

}

void kiplSegmentationTest::testRosin()
{
    // const size_t N=100;
    std::vector<size_t> hist;
    std::string fname = dataPath + "1D/histograms/hist_right_tail_03.txt" ;
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    auto data = kipl::io::readCSV(fname,',',false);

    std::copy(data["a"].begin(),data["a"].end(),std::back_inserter(hist));

    auto th=kipl::segmentation::Threshold_Rosin(hist,kipl::segmentation::tail_right,0);
    QCOMPARE(th,21);
}

void kiplSegmentationTest::testOtsu()
{
    size_t N = 256;
    std::vector<size_t> hvec(N,0UL);
    size_t * harr = new size_t[N];
    float s = 64*64;

    for (size_t i=0; i<N; ++i)
    {
        float a = (static_cast<float>(i)-96.0f);
        float b = (static_cast<float>(i)-160.0f);

        hvec[i]=harr[i]=100*exp(-a*a/s)+100*exp(-b*b/s);
    }

    int tha = kipl::segmentation::Threshold_Otsu(harr,N);
    int thv = kipl::segmentation::Threshold_Otsu(hvec);

    QCOMPARE(tha,thv);
}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(kiplSegmentationTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(kiplSegmentationTest)
#endif

#include "tst_kiplsegmentation.moc"

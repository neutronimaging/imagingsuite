#include <sstream>
#include <vector>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <segmentation/thresholds.h>
#include <segmentation/gradientguidedthreshold.h>

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
};

kiplSegmentationTest::kiplSegmentationTest()
{
}

//void kiplSegmentationTest::testCase1_data()
//{
//    QTest::addColumn<kipl::base::TImage<float,2> >("data");
//    QTest::newRow("0") << QString();
//}

void kiplSegmentationTest::testThreshold()
{
    std::vector<size_t> dims = {100,100};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res(dims);

    QVERIFY2(img.Size(0)==res.Size(0),"x-size error");
    QVERIFY2(img.Size(1)==res.Size(1),"y-size error");

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);

    kipl::segmentation::Threshold(img.GetDataPtr(),res.GetDataPtr(),img.Size(),5000.0f,kipl::segmentation::cmp_greater);
    std::stringstream msg;

    for (size_t i=0; i<img.Size(); ++i) {
        msg.str("");
        msg<<"Greater check failed at "<<i;
        if (i<5000)
            QVERIFY2(res[i]==0,msg.str().c_str());
        else
            QVERIFY2(res[i]==1,msg.str().c_str());
    }


    kipl::segmentation::Threshold(img.GetDataPtr(),res.GetDataPtr(),img.Size(),5000.0f,kipl::segmentation::cmp_less);

    for (size_t i=0; i<img.Size(); ++i) {
        msg.str("");
        msg<<"Less check failed at "<<i;
        if (i<5000)
            QVERIFY2(res[i]==1,msg.str().c_str());
        else
            QVERIFY2(res[i]==0,msg.str().c_str());
    }

    // todo Implement mask tests
}

void kiplSegmentationTest::testDoubleThreshold()
{
    QWARN("This test is not implemented.");

}

void kiplSegmentationTest::testMultiThreshold()
{
    std::ostringstream msg;
    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> res;

#ifdef QT_DEBUG
    kipl::io::ReadTIFF(img,"../../TestData/2D/tiff/multi_class_reference.tif");
#else
    kipl::io::ReadTIFF(img,"../TestData/2D/tiff/multi_class_reference.tif");
#endif
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

#ifdef QT_DEBUG
    kipl::io::ReadTIFF(img,"../../TestData/2D/tiff/multi_class_smooth.tif");
#else
    kipl::io::ReadTIFF(img,"../TestData/2D/tiff/multi_class_smooth.tif");
#endif

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

    QVERIFY_EXCEPTION_THROWN (string2enum("cmp",c), kipl::base::KiplException);

    QCOMPARE(enum2string(kipl::segmentation::cmp_less),std::string("cmp_less"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_greater),std::string("cmp_greater"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_greatereq),std::string("cmp_greater_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_lesseq),std::string("cmp_less_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_noteq),std::string("cmp_not_equal"));
    QCOMPARE(enum2string(kipl::segmentation::cmp_eq),std::string("cmp_equal"));
    QVERIFY_EXCEPTION_THROWN(enum2string(static_cast<kipl::segmentation::CmpType>(20)),kipl::base::KiplException);

    std::ostringstream s;

    s<<kipl::segmentation::cmp_eq;

    QCOMPARE(s.str().c_str(),"cmp_equal");

}

QTEST_APPLESS_MAIN(kiplSegmentationTest)

#include "tst_kiplsegmentationtest.moc"

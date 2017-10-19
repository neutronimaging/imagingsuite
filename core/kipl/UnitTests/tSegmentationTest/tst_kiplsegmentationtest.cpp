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
    size_t dims[2]={100,100};
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
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/multi_class_reference.tif");
#else
    kipl::io::ReadTIFF(img,"../imagingsuite/core/kipl/UnitTests/data/multi_class_reference.tif");
#endif
    //kipl::io::ReadTIFF(img,"../kipl/trunk/UnitTests/data/multi_class_smooth.tif");
    float th[2]={80.0f,150.0f};

    kipl::segmentation::MultiThreshold(img, res, th,2);

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
    size_t dims[2]={100,100};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res(dims);

#ifdef QT_DEBUG
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/multi_class_smooth.tif");
#else
    kipl::io::ReadTIFF(img,"../imagingsuite/core/kipl/UnitTests/data/multi_class_smooth.tif");
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

QTEST_APPLESS_MAIN(kiplSegmentationTest)

#include "tst_kiplsegmentationtest.moc"

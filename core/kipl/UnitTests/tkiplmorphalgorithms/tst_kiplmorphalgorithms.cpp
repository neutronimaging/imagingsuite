#include <QtTest>

// add necessary includes here
#include <sstream>

#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <morphology/label.h>
#include <morphology/morphdist.h>

#include <io/io_tiff.h>

class kiplmorphalgorithms : public QObject
{
    Q_OBJECT

public:
    kiplmorphalgorithms();
    ~kiplmorphalgorithms();
    void loadData();

private slots:
    void test_LabelImage();
    void test_LabelImageRealData();
    void test_RemoveConnectedRegion();
    void test_LabelledItemsInfo();
    void test_pixdist();

private:
    void test_EuclideanDistance();
    void test_EuclideanDistance2();

private:
    kipl::base::TImage<float,2> img;

};

kiplmorphalgorithms::kiplmorphalgorithms()
{

}

kiplmorphalgorithms::~kiplmorphalgorithms()
{

}

void kiplmorphalgorithms::loadData()
{
#ifdef _NDEBUG
    kipl::io::ReadTIFF(img,"../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");
#else
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");
#endif
}

void kiplmorphalgorithms::test_LabelImage()
{
    loadData();
    kipl::base::TImage<int,2> result;
    size_t lblCnt=0;
    lblCnt=kipl::morphology::LabelImage(img,result,kipl::base::conn4);

    QCOMPARE(img.Size(),result.Size());
    QCOMPARE(lblCnt,24UL);

    // Special cases
    int data[]={   0,0,0,0,0,1,1,0,
                   1,1,0,0,0,1,1,0,
                   1,1,0,0,0,0,0,0,
                   0,0,0,1,1,0,0,0,
                   0,0,0,1,1,0,0,0,
                   0,0,0,0,0,0,1,1,
                   0,1,1,0,0,0,1,1,
                   0,1,1,0,0,0,0,0
             };

    size_t dims[2]={8,8};
    kipl::base::TImage<int,2> s(dims);
    std::copy_n(data,s.Size(),s.GetDataPtr());
    lblCnt=kipl::morphology::LabelImage(s,result,kipl::base::conn4);
    kipl::io::WriteTIFF(result,"lblresult_4x4blocks_4connect.tif");

    lblCnt=kipl::morphology::LabelImage(s,result,kipl::base::conn8);
    kipl::io::WriteTIFF(result,"lblresult_4x4blocks_8connect.tif");

}

void kiplmorphalgorithms::test_LabelImageRealData()
{

#ifdef _NDEBUG
    std::string fname="../imagingsuite/core/kipl/UnitTests/data/maskOtsuFilled.tif";
#else
    std::string fname="../../imagingsuite/core/kipl/UnitTests/data/maskOtsuFilled.tif";
#endif
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::base::TImage<int,2> a;
    kipl::base::TImage<int,2> result;

    kipl::io::ReadTIFF(a,fname.c_str());

    size_t lblCnt=0;

    lblCnt=kipl::morphology::LabelImage(a,result,kipl::base::conn4);
    qDebug() << lblCnt;
    lblCnt=kipl::morphology::LabelImage(a,result,kipl::base::conn8);
    qDebug() << lblCnt;
    kipl::io::WriteTIFF(result,"lblrealresult_8connect.tif");
}

void kiplmorphalgorithms::test_RemoveConnectedRegion()
{
    loadData();
    kipl::base::TImage<int,2> result;
    size_t lblCnt=0;

    lblCnt=kipl::morphology::LabelImage(img,result,kipl::base::conn4);

    QCOMPARE(img.Size(),result.Size());
    QCOMPARE(lblCnt,24UL);

    std::vector< pair<size_t,size_t>> arealist;
    kipl::morphology::LabelArea(result,lblCnt,arealist);
    QCOMPARE(arealist.size(),lblCnt+1UL);

    int res=kipl::morphology::RemoveConnectedRegion(result, {},kipl::base::conn4);

    QCOMPARE(res,0);

    kipl::morphology::RemoveConnectedRegion(result, {arealist[10].second,arealist[20].second},kipl::base::conn4);

    kipl::io::WriteTIFF(result,"removelbl.tif");
}

void kiplmorphalgorithms::test_LabelledItemsInfo()
{
    QSKIP("Test is not implemented");
}

void kiplmorphalgorithms::test_pixdist()
{
    ptrdiff_t dims[3]={15,20,25};
    ptrdiff_t sx=dims[0];
    ptrdiff_t sxy=dims[0]*dims[1];
    std::ostringstream msg;
    ptrdiff_t a=7;
    ptrdiff_t b=10;
    ptrdiff_t c=12;
    ptrdiff_t p0=a+b*sx+c*sxy;
    ptrdiff_t p1=0;

    for (int z=0; z<dims[2]; ++z )
        for (int y=0; y<dims[1]; ++y)
            for (int x=0; x<dims[0]; ++x)
            {
                p1=x+y*sx+z*sxy;
                ptrdiff_t result = kipl::morphology::pixdist_x(p0,p1,sx,sxy);
                ptrdiff_t expected = std::abs(x-a);
                msg.str(""); msg<<"X: x:"<<x<<", y:"<<y<<", z:"<<z<<" result:"<<result<<", expected:"<<expected;
                QVERIFY2(result==expected,msg.str().c_str());

                result = kipl::morphology::pixdist_y(p0,p1,sx,sxy);
                expected = std::abs(y-b);
                msg.str(""); msg<<"Y: x:"<<x<<", y:"<<y<<", z:"<<z<<" result:"<<result<<", expected:"<<expected;
                QVERIFY2(result==expected,msg.str().c_str());

                result = kipl::morphology::pixdist_z(p0,p1,sx,sxy);
                expected = std::abs(z-c);
                msg.str(""); msg<<"Z: x:"<<x<<", y:"<<y<<", z:"<<z<<" result:"<<result<<", expected:"<<expected;
                QVERIFY2(result==expected,msg.str().c_str());
            }
}

void kiplmorphalgorithms::test_EuclideanDistance()
{
    loadData();

    kipl::base::TImage<float,2> dist_dev,dist_ref;

    kipl::morphology::EuclideanDistance(img,dist_dev,false,kipl::base::conn8);
    loadData();
    kipl::morphology::old::EuclideanDistance(img,dist_ref,false,kipl::morphology::conn8);

    size_t cnt=0;
    for (size_t i =0; i< dist_dev.Size(); i++)
    {
        if (dist_dev[i]!=dist_ref[i])
            cnt++;
    }
    kipl::io::WriteTIFF32(dist_dev,"eucliddist_dev.tif");
    kipl::io::WriteTIFF32(dist_ref,"eucliddist_ref.tif");

    QCOMPARE(cnt,0UL);

}

void kiplmorphalgorithms::test_EuclideanDistance2()
{
    size_t dims[]={2000,2000};
    kipl::base::TImage<float,2> bw(dims);
    bw=1.0f;
    bw[dims[0]*dims[1]/2+dims[0]/2]=0.0f;

    kipl::base::TImage<float,2> dist_dev,dist_ref;

    kipl::morphology::EuclideanDistance(bw,dist_dev,false,kipl::base::conn8);
    bw=1.0f;
    bw[dims[0]*dims[1]/2+dims[0]/2]=0.0f;
    kipl::morphology::old::EuclideanDistance(bw,dist_ref,false,kipl::morphology::conn8);

    size_t cnt=0;
    for (size_t i =0; i< dist_dev.Size(); i++)
    {
        if (dist_dev[i]!=dist_ref[i])
            cnt++;
    }
    kipl::io::WriteTIFF32(bw,"eucliddist2_bw.tif");
    kipl::io::WriteTIFF32(dist_dev,"eucliddist2_dev.tif");
    kipl::io::WriteTIFF32(dist_ref,"eucliddist2_ref.tif");

    QCOMPARE(cnt,0UL);

}

QTEST_APPLESS_MAIN(kiplmorphalgorithms)

#include "tst_kiplmorphalgorithms.moc"

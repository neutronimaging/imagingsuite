#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <sstream>

#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <morphology/label.h>
#include <morphology/morphdist.h>
#include <morphology/morphextrema.h>

#include <io/io_tiff.h>

class kiplmorphalgorithms : public QObject
{
    Q_OBJECT

public:
    kiplmorphalgorithms();
    ~kiplmorphalgorithms();
    void loadData();
    void makeBall(size_t size=100UL, float radius=45.0f);

private slots:
    void test_LabelImage();
    void test_LabelImageRealData();
    void test_RemoveConnectedRegion();
    void test_LabelledItemsInfo();
    void test_pixdist();
    void test_EuclideanDistance2D();
    void test_EuclideanDistance3D();
    void test_DistanceTransform2D();
    void test_DistanceTransform3D();

    void test_FillSpots();

private:
    void test_EuclideanDistance2();

private:
    kipl::base::TImage<float,2> img2;
    kipl::base::TImage<float,3> img3;
    kipl::base::TImage<float,3> d3;

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
    kipl::io::ReadTIFF(img,"../../TestData/2D/tiff/bilevel_ws.tif");
#else
    kipl::io::ReadTIFF(img,"../TestData/2D/tiff/bilevel_ws.tif");
#endif
}

void kiplmorphalgorithms::makeBall(size_t size, float radius)
{
    size_t dims[3]={size,size,size};
    ptrdiff_t mid = size/2;
    float r2 = radius * radius;

    img3.Resize(dims);
    d3.Resize(dims);
    size_t idx=0;
    for (ptrdiff_t z=0; z<size; ++z)
        for (ptrdiff_t y=0; y<size; ++y)
            for (ptrdiff_t x=0; x<size; ++x, ++idx)
            {
                d3[idx]=std::sqrt((z-mid)*(z-mid)+(y-mid)*(y-mid) + (x-mid)*(x-mid));
                if (!(img3[idx] = (d3[idx]<radius)))
                    d3[idx]=0.0f;
                else
                    d3[idx]=radius-d3[idx]+1;
            }

}

void kiplmorphalgorithms::test_LabelImage()
{
    loadData();
    kipl::base::TImage<int,2> result;
    size_t lblCnt=0;
    lblCnt=kipl::morphology::LabelImage(img2,result,kipl::base::conn4);

    QCOMPARE(img2.Size(),result.Size());
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

    std::vector<size_t> dims ={8,8};
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
    std::string fname="../../TestData/2D/tiff/maskOtsuFilled.tif";
#else
    std::string fname="../TestData/2D/tiff/maskOtsuFilled.tif";
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

    lblCnt=kipl::morphology::LabelImage(img2,result,kipl::base::conn4);

    QCOMPARE(img2.Size(),result.Size());
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
    ptrdiff_t p0=a+b*sx;
    ptrdiff_t p1=0;

    for (int y=0; y<dims[1]; ++y)
        for (int x=0; x<dims[0]; ++x)
        {
            p1=x+y*sx;
            ptrdiff_t result = kipl::morphology::pixdist_x(p0,p1,sx);
            ptrdiff_t expected = std::abs(x-a);
            msg.str(""); msg<<"X: x:"<<x<<", y:"<<y<<" result:"<<result<<", expected:"<<expected;
            QVERIFY2(result==expected,msg.str().c_str());

            result = kipl::morphology::pixdist_y(p0,p1,sx);
            expected = std::abs(y-b);
            msg.str(""); msg<<"Y: x:"<<x<<", y:"<<y<<" result:"<<result<<", expected:"<<expected;
            QVERIFY2(result==expected,msg.str().c_str());

            QCOMPARE(kipl::morphology::old::pixdist_x(p0,p1,sx),kipl::morphology::pixdist_x(p0,p1,sx));
            QCOMPARE(kipl::morphology::old::pixdist_y(p0,p1,sx),kipl::morphology::pixdist_y(p0,p1,sx));
        }

    p0=a+b*sx+c*sxy;

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

                QCOMPARE(kipl::morphology::old::pixdist_x(p0,p1,sx,sxy),kipl::morphology::pixdist_x(p0,p1,sx,sxy));
            }
}

void kiplmorphalgorithms::test_FillSpots()
{
    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> res;
    kipl::io::ReadTIFF(img,"../TestData/2D/tiff/spots/balls.tif");
    img = -img;
    QBENCHMARK {
        res = -kipl::morphology::FillSpot(img,5,kipl::base::conn8);
    }
    kipl::io::WriteTIFF(res,"fillspots.tif");
}

void kiplmorphalgorithms::test_EuclideanDistance2D()
{
    loadData();

    kipl::base::TImage<float,2> dist_dev,dist_ref;

    kipl::morphology::EuclideanDistance(img2,dist_dev,false,kipl::base::conn8);
    loadData();
    kipl::morphology::old::EuclideanDistance(img2,dist_ref,false,kipl::morphology::conn8);

    size_t cnt=0;
    for (size_t i =0; i< dist_dev.Size(); i++)
    {
        if (dist_dev[i]!=dist_ref[i])
            cnt++;
    }
    kipl::io::WriteTIFF(dist_dev,"eucliddist_dev.tif",kipl::base::Float32);
    kipl::io::WriteTIFF(dist_ref,"eucliddist_ref.tif",kipl::base::Float32);

    QCOMPARE(cnt,0UL);

}

void kiplmorphalgorithms::test_EuclideanDistance3D()
{
    makeBall();

    kipl::base::TImage<float,3> dist3D_dev;
    QBENCHMARK
    {
        kipl::morphology::EuclideanDistance(img3,dist3D_dev,false,kipl::base::conn6);
    }


    kipl::io::WriteTIFF32(dist3D_dev,"eudist3D_dev.tif");

}

void kiplmorphalgorithms::test_DistanceTransform2D()
{
    loadData();

    kipl::base::TImage<float,2> dist2D_dev,dist2D_ref;
    kipl::morphology::CMetricSvensson sm;
    kipl::morphology::DistanceTransform2D(img2,dist2D_dev,sm);
//    loadData();
//    kipl::morphology::CMetricSvensson smo;
//    kipl::morphology::old::DistanceTransform2D(img,dist_ref,smo);

//    size_t cnt=0;
//    for (size_t i =0; i< dist2D_dev.Size(); i++)
//    {
//        if (dist_dev[i]!=dist_ref[i])
//            cnt++;
//    }
    kipl::io::WriteTIFF32(dist2D_dev,"dist2D_dev.tif");
//    kipl::io::WriteTIFF32(dist_ref,"eucliddist_ref.tif");

//    QCOMPARE(cnt,0UL);
}

void kiplmorphalgorithms::test_DistanceTransform3D()
{
    makeBall();

    kipl::base::TImage<float,3> dist3D_dev;
    kipl::morphology::CMetricSvensson sm;
    QBENCHMARK
    {
        kipl::morphology::DistanceTransform3D(img3,dist3D_dev,sm);
    }

    kipl::io::WriteTIFF32(dist3D_dev,"dist3D_dev.tif");
//    for (size_t i=0; i<d3.Size(); ++i)
//        QCOMPARE(dist3D_dev[i],d3[i]);

}

void kiplmorphalgorithms::test_EuclideanDistance2()
{
    std::vector<size_t> dims ={2000,2000};
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
    kipl::io::WriteTIFF(bw,"eucliddist2_bw.tif",kipl::base::Float32);
    kipl::io::WriteTIFF(dist_dev,"eucliddist2_dev.tif",kipl::base::Float32);
    kipl::io::WriteTIFF(dist_ref,"eucliddist2_ref.tif",kipl::base::Float32);

    QCOMPARE(cnt,0UL);

}

QTEST_APPLESS_MAIN(kiplmorphalgorithms)

#include "tst_kiplmorphalgorithms.moc"

#include <QtTest>
#include <QDebug>

// add necessary includes here
#include <sstream>
#include <iostream>

#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <morphology/label.h>
#include <morphology/morphdist.h>
#include <morphology/morphextrema.h>
#include <morphology/regionproperties.h>
#include <io/io_tiff.h>
#include <strings/filenames.h>

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
    void test_RegionProperties();
    void test_RegPropFilter();
    void test_pixdist();
    void test_FillSpots();

private:
    void test_EuclideanDistance();
    void test_EuclideanDistance2();

private:
    kipl::base::TImage<float,2> img;
    std::string dataPath;

};

kiplmorphalgorithms::kiplmorphalgorithms()
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}

kiplmorphalgorithms::~kiplmorphalgorithms()
{

}

void kiplmorphalgorithms::loadData()
{
    std::string fname = dataPath+"2D/tiff/bilevel_ws.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);
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
    std::string fname=dataPath+"2D/tiff/maskOtsuFilled.tif";
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

void kiplmorphalgorithms::test_RegionProperties()
{
    std::vector<int> data = {  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,0,
                               0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,4,4,4,0,0,0,
                               0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,3,3,3,3,3,0,0,0,0,4,4,4,0,0,0,
                               0,0,1,1,1,1,1,0,2,0,0,0,0,0,3,3,3,3,3,3,3,0,0,0,0,4,4,4,0,0,
                               0,0,1,1,1,1,1,0,2,2,0,0,0,0,3,3,3,3,3,3,3,0,0,0,0,0,4,4,4,0,
                               0,0,1,1,1,1,1,0,2,2,2,0,0,0,3,3,3,3,3,3,3,0,0,0,0,0,0,4,4,4,
                               0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,3,3,3,3,3,0,0,0,0,0,0,0,4,4,4,
                               0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,4,0,
                               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,
                               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0
                 };

    kipl::base::TImage<int,2> lbl({30,10});
    std::copy(data.begin(),data.end(),lbl.GetDataPtr());
    kipl::morphology::RegionProperties<int,int> rp0;
    QCOMPARE(rp0.count(),0UL);

    kipl::morphology::RegionProperties<int,int> rp(lbl,lbl);

    


    QCOMPARE(rp.count(), 5UL);
    auto labels = rp.labels();

    QCOMPARE(labels.size(),5UL);

    int i=0;
    for (const auto &item : labels) 
    {
        QCOMPARE(item, i);
        ++i;
    }

    auto areas = rp.area();
    QCOMPARE(areas.size(),5UL);
    QCOMPARE(areas[0],199UL);
    QCOMPARE(areas[1],25UL);
    QCOMPARE(areas[2],15UL);
    QCOMPARE(areas[3],37UL);
    QCOMPARE(areas[4],24UL);

    auto perimeter = rp.perimeter();
    QCOMPARE(perimeter.size(),5UL);
    QCOMPARE(perimeter[0],73UL);
    QCOMPARE(perimeter[1],16UL);
    QCOMPARE(perimeter[2],12UL);
    QCOMPARE(perimeter[3],16UL);
    QCOMPARE(perimeter[4],19UL);

    auto cog = rp.cog();

    QCOMPARE(cog.size(),5UL);
    QCOMPARE(cog[1][0],4.0f);
    QCOMPARE(cog[1][1],3.0f);

    QCOMPARE(cog[2][0],9.33333f);
    QCOMPARE(cog[2][1],5.66667f);

    QCOMPARE(cog[3][0],17.0f);
    QCOMPARE(cog[3][1],4.0f);

    QCOMPARE(cog[4][0],26.75f);
    QCOMPARE(cog[4][1],3.625f);

    auto spherity = rp.spherity();

    QCOMPARE(spherity.size(),5UL);

    QCOMPARE(spherity[1],static_cast<float>(areas[1])/static_cast<float>(perimeter[1]));
    QCOMPARE(spherity[2],static_cast<float>(areas[2])/static_cast<float>(perimeter[2]));
    QCOMPARE(spherity[3],static_cast<float>(areas[3])/static_cast<float>(perimeter[3]));
    QCOMPARE(spherity[4],static_cast<float>(areas[4])/static_cast<float>(perimeter[4]));

    auto wcog = rp.wcog();

    QCOMPARE(wcog.size(),5UL);
    QCOMPARE(wcog[1][0],cog[1][0]);
    QCOMPARE(wcog[1][1],cog[1][1]);

    QCOMPARE(wcog[2][0],cog[2][0]);
    QCOMPARE(wcog[2][1],cog[2][1]);

    QCOMPARE(wcog[3][0],cog[3][0]);
    QCOMPARE(wcog[3][1],cog[3][1]);

    QCOMPARE(wcog[4][0],cog[4][0]);
    QCOMPARE(wcog[4][1],cog[4][1]);
}

void kiplmorphalgorithms::test_RegPropFilter()
{
    kipl::base::TImage<float,2> dots;
    kipl::base::TImage<float,2> lbl;
    std::string fname = dataPath+"2D/tiff/dots.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(dots,fname);

    fname = dataPath+"2D/tiff/lbldots.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(lbl,fname);

    std::vector<float> areas={ 9.f,   69.f,  305.f,  697.f, 1005.f};
    for (const auto & area : areas) 
    {
        kipl::morphology::RegionProperties<float,float> rp(lbl,dots);
        rp.filter(kipl::morphology::regprop_area,{area-5,area+5});
        QCOMPARE(rp.count(),5);
    }

    kipl::morphology::RegionProperties<float,float> rp(lbl,dots);
    rp.filter(kipl::morphology::regprop_cogx,{41,164});
    rp.filter(kipl::morphology::regprop_cogy,{41,164});
    QCOMPARE(rp.count(),10); // 9 + background

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
    // QSKIP("Test is not implemented");
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

void kiplmorphalgorithms::test_FillSpots()
{
    kipl::base::TImage<float,2> a;
    kipl::base::TImage<float,2> res;
    std::string fname=dataPath+"2D/tiff/spots/balls.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(a,fname);
    a = -a;
    QBENCHMARK {
        res = -kipl::morphology::FillSpot(a,5,kipl::base::conn8);
    }
    kipl::io::WriteTIFF(res,"fillspots.tif");
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
    kipl::io::WriteTIFF(dist_dev,"eucliddist_dev.tif",kipl::base::Float32);
    kipl::io::WriteTIFF(dist_ref,"eucliddist_ref.tif",kipl::base::Float32);

    QCOMPARE(cnt,0UL);

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

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(kiplmorphalgorithms)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(kiplmorphalgorithms)
#endif
#include "tst_kiplmorphalgorithms.moc"

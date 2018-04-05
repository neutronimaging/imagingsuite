#include <sstream>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/index2coord.h>
#include <math/tcenterofgravity.h>
#include <math/circularhoughtransform.h>
#include <math/nonlinfit.h>
#include <filters/filter.h>
#include <drawing/drawing.h>
#include <math/statistics.h>
#include <math/mathfunctions.h>

#include <io/io_tiff.h>

class TKiplMathTest : public QObject
{
    Q_OBJECT

public:
    TKiplMathTest();
    static bool qFuzzyCompare(double p1, double p2, double delta=0.000000000001)
    {
        return (qAbs(p1 - p2) <= delta * qMin(qAbs(p1), qAbs(p2)));
    }

private Q_SLOTS:
    void testCOG();
    void testCircularHoughTransform();
    void testNonLinFit_fitfunctions();
    void testNonLinFit_fitter();
    void testStatistics();
    void testSignFunction();
};

TKiplMathTest::TKiplMathTest()
{
}

void TKiplMathTest::testCOG()
{
    kipl::math::CenterOfGravity cog;

    size_t dims[3]={100,100,100};
    kipl::base::TImage<float,3> img(dims);
    kipl::base::coords3Df center;
    center.x=54;
    center.y=45;
    center.z=60;
    float R2=400;

    size_t i=0UL;
    float *pImg=img.GetDataPtr();
    for (size_t z=0; z<dims[2]; ++z)
    {
        for (size_t y=0; y<dims[1]; ++y)
        {
            for (size_t x=0; x<dims[0]; ++x, ++i)
            {
                pImg[i]=((x-center.x)*(x-center.x)+
                        (y-center.y)*(y-center.y)+
                        (z-center.z)*(z-center.z))<R2;
            }
        }
    }

    kipl::base::coords3Df cogcenter=cog.findCenter(img, false);
    ostringstream msg;
    msg<<center.x<<"!="<<cogcenter.x;
    QVERIFY2(cogcenter.x==center.x,msg.str().c_str());

    msg.str("");
    msg<<center.y<<"!="<<cogcenter.y;
    QVERIFY2(cogcenter.y==center.y,msg.str().c_str());
    msg.str("");
    msg<<center.z<<"!="<<cogcenter.z;
    QVERIFY2(cogcenter.z==center.z,msg.str().c_str());

    float k[30];
    std::fill(k,k+30,1.0f);
    size_t kd[3]={3,3,3};
    kipl::filters::TFilter<float,3> box(k,kd);

    kipl::base::TImage<float,3> fimg=box(img,kipl::filters::FilterBase::EdgeValid);
    cogcenter=cog.findCenter(fimg, true);

    msg.str("");
    msg<<center.x<<"!="<<cogcenter.x;
    float delta=0.01f;
    QVERIFY2(qFuzzyCompare(cogcenter.x,center.x,delta),msg.str().c_str());

    msg.str("");
    msg<<center.y<<"!="<<cogcenter.y;
    QVERIFY2(qFuzzyCompare(cogcenter.y,center.y,delta),msg.str().c_str());
    msg.str("");
    msg<<center.z<<"!="<<cogcenter.z;
    QVERIFY2(qFuzzyCompare(cogcenter.z,center.z,delta),msg.str().c_str());
}

void TKiplMathTest::testCircularHoughTransform()
{
    size_t dims[2]={100,100};

    kipl::base::TImage<float,2> img(dims);

    kipl::drawing::Circle<float> circ1(10.0);
    kipl::drawing::Circle<float> circ2(5.0);

    circ1.Draw(img,50,50,1.1);

    circ1.Draw(img,75,25,0.6);
    circ1.Draw(img,70,80,2.3);
    circ2.Draw(img,25,75,1.5);

    kipl::math::CircularHoughTransform cht;

    kipl::base::TImage<float,2> chm=cht(img,10.0f);

    kipl::io::WriteTIFF32(img,"cht_orig.tif");
    kipl::io::WriteTIFF32(chm,"cht_map.tif");

    chm=cht(img,10.0f,true);

    kipl::io::WriteTIFF32(img,"chtg_orig.tif");
    kipl::io::WriteTIFF32(chm,"chtg_map.tif");
}

void TKiplMathTest::testNonLinFit_fitfunctions()
{
 //   kipl::Nonlinear::SumOfGaussians sog;

}

void TKiplMathTest::testNonLinFit_fitter()
{

}

void TKiplMathTest::testStatistics()
{
    kipl::math::Statistics stats;

    QVERIFY(stats.n()==0UL);
    QVERIFY(stats.E()==0.0f);
    QVERIFY(stats.s()==0.0f);
    QVERIFY(stats.Min()==0.0f);
    QVERIFY(stats.Max()==0.0f);
    QVERIFY(stats.V()==0.0f);
    QVERIFY(stats.Sum()==0.0f);
    QVERIFY(stats.Sum2()==0.0f);

    double data[10]={ 4.49201549,  3.63910658,  1.14245381,  2.42364998,  2.12082273,
                     -0.89899808,  2.27052317,  0.62877943,  5.10328637,  4.0204556};

    for (int i=0; i<10; ++i)
        stats.put(data[i]);
    double eps=0.0001;
    QVERIFY(fabs(stats.Sum()==24.942095070011739)<eps);
    QVERIFY(fabs(stats.Sum2()==93.664900413805682)<eps);
    QVERIFY(stats.n()==10UL);
    QVERIFY(fabs(stats.E()-2.4942095070011741)<eps);
    cout<<stats.s()<<std::endl;
    QVERIFY(fabs(stats.s()-1.7735300889935668)<eps);
    QVERIFY(fabs(stats.Min()==-0.89899808236334788)<eps);
    QVERIFY(fabs(stats.Max()==5.1032863747302493)<eps);
    QVERIFY(fabs(stats.V()==3.1454089765655291)<eps);


}

void TKiplMathTest::testSignFunction()
{
    std::ostringstream msg;
    for (int i=-10; i<10; ++i) {
        msg.str("");
        msg<<"Sign for "<<i<<" is "<<kipl::math::sign(i);
        if (i<0)
            QVERIFY2(kipl::math::sign(i)==int(-1),msg.str().c_str());
        else {
        if (i==0)
            QVERIFY2(kipl::math::sign(i)==0,msg.str().c_str());
        else {
            if (i>0)
                QVERIFY2(kipl::math::sign(i)==1,msg.str().c_str());
            }
        }
    }

    for (float f=-10.0f; f<10.0f; ++f) {
        msg.str("");
        msg<<"Sign for "<<f<<" is "<<kipl::math::sign(f);
        if (f<0.0f)
            QVERIFY2(kipl::math::sign(f)==-1,msg.str().c_str());
        else {
            if (f==0.0f)
                QVERIFY2(kipl::math::sign(f)==0,msg.str().c_str());
            else {
                if (f>0.0f)
                    QVERIFY2(kipl::math::sign(f)==1,msg.str().c_str());
            }
        }

    }

    QVERIFY2(kipl::math::sign(std::numeric_limits<float>::infinity())==1,"Plus inf");
    QVERIFY2(kipl::math::sign(-std::numeric_limits<float>::infinity())==-1,"Minus inf");

}

QTEST_APPLESS_MAIN(TKiplMathTest)

#include "tst_tkiplmathtest.moc"

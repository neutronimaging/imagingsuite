#include <sstream>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/index2coord.h>
#include <math/tcenterofgravity.h>
#include <filters/filter.h>

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

QTEST_APPLESS_MAIN(TKiplMathTest)

#include "tst_tkiplmathtest.moc"

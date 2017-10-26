#include <sstream>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/index2coord.h>
#include <math/tcenterofgravity.h>

class TKiplMathTest : public QObject
{
    Q_OBJECT

public:
    TKiplMathTest();

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

    kipl::base::coords3Df cogcenter=cog.findCenter(img);
    ostringstream msg;
    msg<<center.x<<"!="<<cogcenter.x;
    QVERIFY2(cogcenter.x==center.x,msg.str().c_str());

    msg.str("");
    msg<<center.y<<"!="<<cogcenter.y;
    QVERIFY2(cogcenter.y==center.y,msg.str().c_str());
    msg.str("");
    msg<<center.z<<"!="<<cogcenter.z;
    QVERIFY2(cogcenter.z==center.z,msg.str().c_str());
}

QTEST_APPLESS_MAIN(TKiplMathTest)

#include "tst_tkiplmathtest.moc"

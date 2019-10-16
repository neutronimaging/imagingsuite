#include <sstream>
#include <list>

#include <QString>
#include <QtTest>
#include <QDebug>

#include <base/timage.h>
#include <base/index2coord.h>
#include <math/tcenterofgravity.h>
#include <math/circularhoughtransform.h>
#include <math/nonlinfit.h>
#include <filters/filter.h>
#include <drawing/drawing.h>
#include <math/statistics.h>
#include <math/mathfunctions.h>
#include <math/findpeaks.h>
#include <math/image_statistics.h>
#include <math/covariance.h>
#include <math/mathconstants.h>
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
    void testNonLinFit_enums();
    void testNonLinFit_GaussianFunction();
    void testNonLinFit_fitter();
    void testNonLinFit_EdgeFunction();
    void testFindPeaks();

    void testStatistics();
    void testImageStats();
    void testSignFunction();
    void testCovDims();
    void testCovSymmetry();
    void testCovIntactData();
    void testCovSmallData();
    void testCorrSmallData();



private:
    kipl::base::TImage<float,2> sin2D;
};

TKiplMathTest::TKiplMathTest()
{
    size_t dims[2]={100,6};
    kipl::base::TImage<float,2> img(dims);
    img=0.0f;
    for (int i=0; i<int(dims[1]); i++) {
        float *d=img.GetLinePtr(i);
        float w=2*(i+1)*3.1415926f/float(dims[0]);
 //       std::cout<<"w="<<w<<std::endl;
        for (int j=0; j<int(dims[0]); j++) {
            d[j]=sin(j*w)+i;
        }
    }
    sin2D=img;
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

void TKiplMathTest::testNonLinFit_enums()
{
    std::string val;
    Nonlinear::eProfileFunction e;

    val="GaussProfile";
    string2enum(val,e);
    QCOMPARE(e,Nonlinear::eProfileFunction::fnSumOfGaussians);
    val="LorenzProfile";
    string2enum(val,e);
    QCOMPARE(e,Nonlinear::eProfileFunction::fnLorenzian);
    val="VoightProfile";
    string2enum(val,e);
    QCOMPARE(e,Nonlinear::eProfileFunction::fnVoight);
    val="EdgeProfile";
    string2enum(val,e);
    QCOMPARE(e,Nonlinear::eProfileFunction::fnEdgeFunction);

    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnSumOfGaussians),std::string("GaussProfile"));
    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnLorenzian),std::string("LorenzProfile"));
    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnVoight),std::string("VoightProfile"));
    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnEdgeFunction),std::string("EdgeProfile"));

    ostringstream msg;

    msg.str("");
    msg<<Nonlinear::eProfileFunction::fnSumOfGaussians;
    QCOMPARE(msg.str(),std::string("GaussProfile"));
    msg.str("");
    msg<<Nonlinear::eProfileFunction::fnLorenzian;
    QCOMPARE(msg.str(),std::string("LorenzProfile"));
    msg.str("");
    msg<<Nonlinear::eProfileFunction::fnVoight;
    QCOMPARE(msg.str(),std::string("VoightProfile"));   
    msg.str("");
    msg<<Nonlinear::eProfileFunction::fnEdgeFunction;
    QCOMPARE(msg.str(),std::string("EdgeProfile"));

}
void TKiplMathTest::testNonLinFit_GaussianFunction()
{
    Nonlinear::SumOfGaussians sog(1);
    QCOMPARE(sog.getNpars(),3); // Three parameters as we have position, amplitude and width.
    QCOMPARE(sog.getNpars2fit(),3); // Default all will be fitted

    bool lv[]={false,true,true};
    sog.setLock(lv);
    for (int i=0; i<sog.getNpars(); ++i)
        QCOMPARE(sog.isFree(i),lv[i]);

    QCOMPARE(sog.getNpars2fit(),1);
    sog[0]=1;
    sog[1]=0;
    sog[2]=1;

    QCOMPARE(sog(0.0),(double)1.0);
    QVERIFY(fabs(sog(1)-(double)0.367879441171)<(double)1.0e-7);

    Nonlinear::SumOfGaussians sog2(2);
    sog2[0]=1;
    sog2[1]=0;
    sog2[2]=1;
    sog2[3]=2;
    sog2[4]=0.5;
    sog2[5]=0.5;
//    qDebug() << (double)sog2(0);
//    qDebug() << (double)sog2(1);
    QVERIFY(fabs(sog2(0)-(double)1.73575888234)<(double)1.0e-7);
    QVERIFY(fabs(sog2(1)-(double)1.10363832351)<(double)1.0e-7);

    double x=1;
    double y0=0;
    double y1=0;
    Array1D<double> dyda(3);

    y0=sog(x);
    sog(x,y1,dyda);
    QCOMPARE(y0,y1);
}

void TKiplMathTest::testNonLinFit_fitter()
{
    int N=100;

    Array1D<double> x(N);
    Array1D<double> y(N);
    Array1D<double> sig(N);

    Nonlinear::SumOfGaussians sog0(1),sog(1);
    sog0[0]=2; //A
    sog0[1]=0; //m
    sog0[2]=1; //s

    sog[0]=1; //A
    sog[1]=0.5; //m
    sog[2]=1.5; //s

    for (int i=0; i<N; ++i)
    {
        x[i]=(i-N/2)*0.2;
        y[i]=sog0(x[i]);
      //  qDebug() << "Data: x="<<x[i]<<", y="<<y[i];
        sig[i]=1.0;
    }

    Nonlinear::LevenbergMarquardt mrq(1e-15,15);

    mrq.fit(x,y,sig,sog);

    sog.printPars();

    QCOMPARE(sog[0], sog0[0]);
    QCOMPARE(sog[1], sog0[1]);
    QCOMPARE(sog[2], sog0[2]);

}

void TKiplMathTest::testNonLinFit_EdgeFunction()
{
    int N=1107;
    double eps=0.001;

    Array1D<double> x(N);
    Array1D<double> y(N); // used for the initial estimated of the edge
    Array1D<double> edge(N); // actual edge to be fitted
    Array1D<double> sig(N);
    Array1D<double> initial_edge(N);
    Array1D<double> params(7);
    Array1D<double> dyda(7);
    Array1D<double> dfdsigma(12);
    Array1D<double> dfdx0(12);
    Array1D<double> dfdtau(12);
    Array1D<double> dfda0(12);
    Array1D<double> dfdb0(12);
    Array1D<double> dfda1(12);
    Array1D<double> dfdb1(12);

    // values of the first partial derivatives computed with Mathematica from the analytical function
    dfdsigma[0] = 0.0;
    dfdsigma[1] = 0.0;
    dfdsigma[2] = 0.0;
    dfdsigma[3] = 0.0768592;
    dfdsigma[4] = -5.3928;
    dfdsigma[5] = -1.3984;
    dfdsigma[6] = -0.361873;
    dfdsigma[7] = -0.0934707;
    dfdsigma[8] = -0.0119275;
    dfdsigma[9] = -0.000788495;
    dfdsigma[10] = -0.0000518738;
    dfdsigma[11] = -0.00000339884;


    dfdx0[0] = 0.0;
    dfdx0[1] = 0.0;
    dfdx0[2] = 0.0;
    dfdx0[3] = -0.0170123;
    dfdx0[4] = -80.892;
    dfdx0[5] = -20.976;
    dfdx0[6] = -5.4289;
    dfdx0[7] = -1.40206;
    dfdx0[8] = -0.178912;
    dfdx0[9] = -0.0118274;
    dfdx0[10] = -0.000778108;
    dfdx0[11] = -0.000508926;

    dfdtau[0] = 0.0;
    dfdtau[1] = 0.0;
    dfdtau[2] = 0.0;
    dfdtau[3] = -0.000245961;
    dfdtau[4] = -86.8905;
    dfdtau[5] = -51.1708;
    dfdtau[6] = -20.6529;
    dfdtau[7] = -7.24887;
    dfdtau[8] = -1.29596;
    dfdtau[9] = -0.117969;
    dfdtau[10] = -0.00988577;
    dfdtau[11] = -0.000786945;


    dfda0[0] = -0.232489;
    dfda0[1] = -0.217618;
    dfda0[2] = -0.203699;
    dfda0[3] = -0.190671;
    dfda0[4] = -0.413146;
    dfda0[5] = -0.497202;
    dfda0[6] = -0.514768;
    dfda0[7] = -0.515115;
    dfda0[8] = -0.508423;
    dfda0[9] = -0.497658;
    dfda0[10] = -0.486898;
    dfda0[11] = -0.476355;

    dfdb0[0] = -0.011623;
    dfdb0[1] = -0.0113253;
    dfdb0[2] = -0.0110181;
    dfdb0[3] = -0.0107039;
    dfdb0[4] = -0.0240393;
    dfdb0[5] = -0.0299484;
    dfdb0[6] = -0.0320607;
    dfdb0[7] = -0.0331373;
    dfdb0[8] = -0.034288;
    dfdb0[9] = -0.0356005;
    dfdb0[10] = -0.0368251;
    dfdb0[11] = -0.0379789;

    dfda1[0] = -0.232489;
    dfda1[1] = -0.217618;
    dfda1[2] = -0.203699;
    dfda1[3] = -0.19067;
    dfda1[4] = -0.0608292;
    dfda1[5] = -0.0145361;
    dfda1[6] = -0.00347365;
    dfda1[7] = -0.000830086;
    dfda1[8] = -0.0000944199;
    dfda1[9] = -5.39185e-6;
    dfda1[10] = -3.07902e-7;
    dfda1[11] =-1.75827e-8;

    dfdb1[0] = -0.011623;
    dfdb1[1] = -0.0113253;
    dfdb1[2] = -0.0110181;
    dfdb1[3] = -0.0107038;
    dfdb1[4] = -0.0035394;
    dfdb1[5] = -0.000875568;
    dfdb1[6] = -0.000216346;
    dfdb1[7] = -0.000053399;
    dfdb1[8] = -6.36768e-6;
    dfdb1[9] = -3.85711e-7;
    dfdb1[10] = -2.32872e-8;
    dfdb1[11] = -1.40184e-9;


    Nonlinear::EdgeFunction ef;

    ef[0]=0.056568;
    ef[1]=0.0001;
    ef[2]=0.0015;
    ef[3]=0.315462;
    ef[4]=5.3447841;
    ef[5]=-0.4700811;
    ef[6]=26.929825;

    QCOMPARE(ef.getNpars(),7); // Seven parameters to be fitted
    QCOMPARE(ef.getNpars2fit(),7); // Default all will be fitted   



    short loop=0; //short for loop for input
    string line; //this will contain the data read from the file
    ifstream myfile("/home/carminati_c/git/imagingsuite/core/kipl/UnitTests/data/x.txt"); //opening the file.


    for (double a; myfile>>a;)
    {
        x[loop]=a;
        loop++;
    }

    QCOMPARE(loop, 1107);

    short loop_y=0;
    ifstream myfile_y ("/home/carminati_c/git/imagingsuite/core/kipl/UnitTests/data/initialmodel.txt"); //opening the file. //path should be related to the lib
    ifstream myfile_y2 ("/home/carminati_c/git/imagingsuite/core/kipl/UnitTests/data/y.txt"); //opening the file. //path should be related to the lib
    for (double a; myfile_y>>a;)
    {
        y[loop_y]=a;
        sig[loop_y]=1.0;
        loop_y++;

    }

    loop_y=0;
    for (double a; myfile_y2>>a;)
    {
        edge[loop_y]=a;
        loop_y++;

    }

    QCOMPARE(loop_y, 1107);

    ef.getPars(params);
    QCOMPARE(ef[0],params[0]);
    QCOMPARE(ef[1],params[1]);
    QCOMPARE(ef[2],params[2]);
    QCOMPARE(ef[3],params[3]);
    QCOMPARE(ef[4],params[4]);
    QCOMPARE(ef[5],params[5]);
    QCOMPARE(ef[6],params[6]);




    // Here i test the edge formulation
    for (int i=0; i<loop; ++i)
    {
        initial_edge[i] = ef(x[i]); // I compute the initial edge
        QVERIFY(fabs(initial_edge[i]-y[i])<eps);// it is correct
    }




    // Then I have to test the correctness of the first derivative
    // here I compute the derivatives

    int ind =0;
    for (int i=0; i<loop; i+=100)
//    for (int i=290; i<310; i++)
    {
        ef(x[i],y[i],dyda);
//        qDebug() << ind;
//        qDebug() << x[i];
//        qDebug() << y[i];
//        qDebug() << dyda[0];
//        qDebug() << dfdx0[ind];
//        qDebug() << dyda[1];
//        qDebug() << dyda[2];
//        qDebug() << dyda[3];
//        qDebug() << dyda[4];
//        qDebug() << dyda[5];
//        qDebug() << dyda[6];

        QVERIFY(fabs(dyda[0]-dfdx0[ind])<eps); // passed!
        QVERIFY(fabs(dyda[1]-dfdsigma[ind])<eps); // passed!
        QVERIFY(fabs(dyda[2]-dfdtau[ind])<eps); // passed!
        QVERIFY(fabs(dyda[3]-dfda0[ind])<eps); // passed!
        QVERIFY(fabs(dyda[4]-dfdb0[ind])<eps); // passed!
        QVERIFY(fabs(dyda[5]-dfda1[ind])<eps); // passed!
        QVERIFY(fabs(dyda[6]-dfdb1[ind])<eps); // passed!
        ++ind;

    }


    Nonlinear::LevenbergMarquardt mrq(1e-3,2);

//    bool lock[7]={true, true, true, false, false, false, false};
//    ef.setLock(lock);

//    mrq.fit(x,edge,sig,ef);

//    ef.printPars();

//    ef.getPars(params);

//    qDebug() << params[0];
//    qDebug() << params[1];
//    qDebug() << params[2];
//    qDebug() << params[3];
//    qDebug() << params[4];
//    qDebug() << params[5];
//    qDebug() << params[6];

//    lock[3]= lock[4] = false;
//    lock[5] = lock[6] = true;

//    ef.setLock(lock);
//    mrq.fit(x,edge,sig,ef);

//    ef.getPars(params);

//    qDebug() << params[0];
//    qDebug() << params[1];
//    qDebug() << params[2];
//    qDebug() << params[3];
//    qDebug() << params[4];
//    qDebug() << params[5];
//    qDebug() << params[6];







}

void TKiplMathTest::testFindPeaks()
{
    const size_t N=100;
    float x[N];
    float y[N];

    for (size_t i=0; i<N; ++i) {
        x[i]=float(i)/10.0f;
        y[i]=sin(x[i])+5;
    }

    std::list<size_t> peaks;
    size_t cnt=kipl::math::findPeaks(y,N,5.0f,0.5f,peaks);

    QCOMPARE(cnt,2UL);
    QCOMPARE(cnt,peaks.size());
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

void TKiplMathTest::testImageStats()
{
    double data[10]={ 4.49201549,  3.63910658,  1.14245381,  2.42364998,  2.12082273,
                     -0.89899808,  2.27052317,  0.62877943,  5.10328637,  4.0204556};

    double eps=0.0001;
    std::pair<double,double> stat=kipl::math::statistics(data,10);

    QVERIFY(fabs(stat.first-2.4942095070011741)<eps);

    qDebug() << stat.second ;
    QVERIFY(fabs(stat.second-1.7735300889935668)<eps);

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


void TKiplMathTest::testCovDims()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

    QCOMPARE(C.dim1(),C.dim2());

    QCOMPARE(C.dim1(),int(img.Size(1)));
}

void TKiplMathTest::testCovSymmetry()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

    for (int i=0; i<int(img.Size(1)); i++) {
        for (int j=i; j<int(img.Size(1)); j++) {
            QCOMPARE(C[i][j],C[j][i]);
        }
    }
}

void TKiplMathTest::testCovIntactData()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

    for (int i=0; i<int(img.Size()); i++)
        QCOMPARE(img[i],sin2D[i]);

}

void TKiplMathTest::testCovSmallData()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    cov.setResultMatrixType(kipl::math::CovarianceMatrix);

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

    for (int i=0 ; i<img.Size(1); i++) {
        for (int j=0 ; j<img.Size(1); j++) {
  //          std::cout<<std::setw(12)<<C[i][j];

        }
//        std::cout<<std::endl;
        QVERIFY(fabs(C[i][i]-0.5)<1e-7);
    }
}

void TKiplMathTest::testCorrSmallData()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;
    cov.setResultMatrixType(kipl::math::CorrelationMatrix);

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.Dims(),2);

    for (int i=0 ; i<img.Size(1); i++) {
        for (int j=0 ; j<img.Size(1); j++) {
            std::cout<<std::setw(12)<<C[i][j];
        }
        std::cout<<std::endl;
        QVERIFY(fabs(C[i][i]-1.0)<1e-7);
    }
}

QTEST_APPLESS_MAIN(TKiplMathTest)

#include "tst_tkiplmathtest.moc"

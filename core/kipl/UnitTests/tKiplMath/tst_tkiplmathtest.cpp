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
#include <math/gradient.h>
#include <math/linfit.h>

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
    void testTNTNonLinFit_GaussianFunction();
    void testTNTNonLinFit_fitter();
    void testARMANonLinFit_GaussianFunction();
    void testARMANonLinFit_fitter();
    void testFindPeaks();

    void testStatistics();
    void testImageStats();
    void testSignFunction();
    void testCovDims();
    void testCovSymmetry();
    void testCovIntactData();
    void testCovSmallData();
    void testCorrSmallData();
    void testMinMax();

    void testPolyVal();
    void testPolyFit();
    void testPolyDeriv();




private:
    void TestGradient();
    kipl::base::TImage<float,2> sin2D;
};

TKiplMathTest::TKiplMathTest()
{
    std::vector<size_t> dims={100,6};
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

    std::vector<size_t> dims={100,100,100};
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

//    kipl::base::coords3Df cogcenter=cog.findCenter(img, false);
//    ostringstream msg;
//    msg<<center.x<<"!="<<cogcenter.x;
//    QVERIFY2(cogcenter.x==center.x,msg.str().c_str());

//    msg.str("");
//    msg<<center.y<<"!="<<cogcenter.y;
//    QVERIFY2(cogcenter.y==center.y,msg.str().c_str());
//    msg.str("");
//    msg<<center.z<<"!="<<cogcenter.z;
//    QVERIFY2(cogcenter.z==center.z,msg.str().c_str());

//    std::vector<float> k(30,1.0f);
//    std::vector<size_t> kd={3,3,3};
//    kipl::filters::TFilter<float,3> box(k,kd);

//    kipl::base::TImage<float,3> fimg=box(img,kipl::filters::FilterBase::EdgeValid);
//    cogcenter=cog.findCenter(fimg, true);

//    msg.str("");
//    msg<<center.x<<"!="<<cogcenter.x;
//    float delta=0.01f;
//    QVERIFY2(qFuzzyCompare(cogcenter.x,center.x,delta),msg.str().c_str());

//    msg.str("");
//    msg<<center.y<<"!="<<cogcenter.y;
//    QVERIFY2(qFuzzyCompare(cogcenter.y,center.y,delta),msg.str().c_str());
//    msg.str("");
//    msg<<center.z<<"!="<<cogcenter.z;
//    QVERIFY2(qFuzzyCompare(cogcenter.z,center.z,delta),msg.str().c_str());
}

void TKiplMathTest::testCircularHoughTransform()
{
    std::vector<size_t> dims={100,100};

    kipl::base::TImage<float,2> img(dims);

//    kipl::drawing::Circle<float> circ1(10.0);
//    kipl::drawing::Circle<float> circ2(5.0);

//    circ1.Draw(img,50,50,1.1);

//    circ1.Draw(img,75,25,0.6);
//    circ1.Draw(img,70,80,2.3);
//    circ2.Draw(img,25,75,1.5);

//    kipl::math::CircularHoughTransform cht;

//    kipl::base::TImage<float,2> chm=cht(img,10.0f);

//    kipl::io::WriteTIFF32(img,"cht_orig.tif");
//    kipl::io::WriteTIFF32(chm,"cht_map.tif");

//    chm=cht(img,10.0f,true);

//    kipl::io::WriteTIFF32(img,"chtg_orig.tif");
//    kipl::io::WriteTIFF32(chm,"chtg_map.tif");
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

    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnSumOfGaussians),std::string("GaussProfile"));
    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnLorenzian),std::string("LorenzProfile"));
    QCOMPARE(enum2string(Nonlinear::eProfileFunction::fnVoight),std::string("VoightProfile"));

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

}
void TKiplMathTest::testTNTNonLinFit_GaussianFunction()
{
    Nonlinear::TNTfit::SumOfGaussians sog(1);
    QCOMPARE(sog.getNpars(),3); // Three parameters as we have position, amplitude and width.
    QCOMPARE(sog.getNpars2fit(),3); // Default all will be fitted

    std::vector<bool> lv={false,true,true};
    sog.setLock(lv);
    for (int i=0; i<sog.getNpars(); ++i)
        QCOMPARE(sog.isFree(i),lv[i]);

    QCOMPARE(sog.getNpars2fit(),1);
    sog[0]=1;
    sog[1]=0;
    sog[2]=1;

    QCOMPARE(sog(0.0),(double)1.0);
    QVERIFY(fabs(sog(1)-(double)0.367879441171)<(double)1.0e-7);

    Nonlinear::TNTfit::SumOfGaussians sog2(2);
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

void TKiplMathTest::testTNTNonLinFit_fitter()
{
    int N=100;

    Array1D<double> x(N);
    Array1D<double> y(N);
    Array1D<double> sig(N);

    Nonlinear::TNTfit::SumOfGaussians sog0(1),sog(1);
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

    Nonlinear::TNTfit::LevenbergMarquardt mrq(1e-15);

    mrq.fit(x,y,sig,sog);

    QVERIFY(fabs(sog[0]-sog0[0])<1e-5);
    QVERIFY(fabs(sog[1]-sog0[1])<1e-5);
    QVERIFY(fabs(sog[2]-sog0[2])<1e-5);

}

void TKiplMathTest::testARMANonLinFit_GaussianFunction()
{
    Nonlinear::SumOfGaussians sog(1);
    QCOMPARE(sog.getNpars(),3); // Three parameters as we have position, amplitude and width.
    QCOMPARE(sog.getNpars2fit(),3); // Default all will be fitted

    std::vector<bool> lv={false,true,true};
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

    QVERIFY(fabs(sog2(0)-(double)1.73575888234)<(double)1.0e-7);
    QVERIFY(fabs(sog2(1)-(double)1.10363832351)<(double)1.0e-7);

    double x=1;
    double y0=0;
    double y1=0;
    arma::vec dyda(3);

    y0=sog(x);
    sog(x,y1,dyda);
    QCOMPARE(y0,y1);
}

void TKiplMathTest::testARMANonLinFit_fitter()
{
    int N=100;

    arma::vec x(N);
    arma::vec y(N);
    arma::vec sig(N);

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

    Nonlinear::LevenbergMarquardt mrq(1e-15);

    mrq.fit(x,y,sig,sog);

    QVERIFY(fabs(sog[0]-sog0[0])<1e-5);
    QVERIFY(fabs(sog[1]-sog0[1])<1e-5);
    QVERIFY(fabs(sog[2]-sog0[2])<1e-5);

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

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.dims(),2);

    QCOMPARE(C.dim1(),C.dim2());

    QCOMPARE(C.dim1(),int(img.Size(1)));
}

void TKiplMathTest::testCovSymmetry()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.dims(),2);

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

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.dims(),2);

    for (int i=0; i<int(img.Size()); i++)
        QCOMPARE(img[i],sin2D[i]);

}

void TKiplMathTest::testCovSmallData()
{
    kipl::base::TImage<float,2> img=sin2D;

    img.Clone();

    kipl::math::Covariance<float> cov;

    cov.setResultMatrixType(kipl::math::CovarianceMatrix);

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.dims(),2);

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

    TNT::Array2D<double> C=cov.compute(img.GetDataPtr(),img.dims(),2);

    for (int i=0 ; i<img.Size(1); i++) {
        for (int j=0 ; j<img.Size(1); j++) {
            std::cout<<std::setw(12)<<C[i][j];
        }
        std::cout<<std::endl;
        QVERIFY(fabs(C[i][i]-1.0)<1e-7);
    }
}

void TKiplMathTest::testMinMax()
{
    std::vector<size_t> dims={100UL,100UL};
    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);

    float mi, ma;

    kipl::math::minmax(img.GetDataPtr(),img.Size(),&mi,&ma);
    QCOMPARE(mi,0.0f);
    QCOMPARE(ma,img.Size()-1.0f);
    float zero=0.0f;
    img[1]=sqrt(-1.0f);
    img[2]=-sqrt(-1.0f);
    img[3]=1.0f/zero;
    img[4]=-1.0f/zero;

    kipl::math::minmax(img.GetDataPtr(),img.Size(),&mi,&ma);
    QCOMPARE(mi,0.0f);
    QCOMPARE(ma,img.Size()-1.0f);

    kipl::math::minmax(img.GetDataPtr(),img.Size(),&mi,&ma,false);
    QCOMPARE(mi,-1.0f/zero);
    QCOMPARE(ma,1.0f/zero);

}

void TKiplMathTest::TestGradient()
{
//read data and expected gradient, compute gradient and compare

    double x[12] = {0.0500144, 0.0520624, 0.0541104, 0.0561584, 0.0582064, 0.0602544, 0.0623024,
                    0.0643504, 0.067481,  0.071577,  0.075673,  0.079769};


    double data[12] = {0.232335, 0.217474, 0.203564, 0.190545, 0.414715, 0.497566, 0.51482,  0.515086,
                    0.508319, 0.497549, 0.486791, 0.476251};
    double exp_gradient[12] = {-7.25634766, -7.02416992, -6.57446289, 51.55053711, 74.95629883, 24.43969727,
                               4.27734375, -0.77632507, -2.36423198, -2.62792969, -2.59985352, -2.57324219};

    double *comp_gradient = new double[12];

    kipl::math::num_gradient(data, x, 12, comp_gradient);

    for (int i=0; i<12; ++i)
    {
        qDebug() << comp_gradient[i];
        QVERIFY(fabs(comp_gradient[i]-exp_gradient[i])<1e-7);
//        QCOMPARE(comp_gradient[i], exp_gradient[i]); // does not like negative?
    }

    delete [] comp_gradient;

    // This gives a compilation error
//    // fake arrays to check what happens with int as inputs
//    int intx[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
//    int inty[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
//    int *int_gradient = new int[12];

//    kipl::math::num_gradient(inty, intx, 12, int_gradient);

    float x_f[12] = {0.0500144, 0.0520624, 0.0541104, 0.0561584, 0.0582064, 0.0602544, 0.0623024,
                    0.0643504, 0.067481,  0.071577,  0.075673,  0.079769};


    float data_f[12] = {0.232335, 0.217474, 0.203564, 0.190545, 0.414715, 0.497566, 0.51482,  0.515086,
                    0.508319, 0.497549, 0.486791, 0.476251};
    float exp_gradient_f[12] = {-7.25634766, -7.02416992, -6.57446289, 51.55053711, 74.95629883, 24.43969727,
                               4.27734375, -0.77632507, -2.36423198, -2.62792969, -2.59985352, -2.57324219};

    float *comp_gradient_f = new float[12];

    kipl::math::num_gradient(data_f, x_f, 12, comp_gradient_f);

    for (int i=0; i<12; ++i)
    {
        qDebug() << comp_gradient_f[i];
        QVERIFY(fabs(comp_gradient_f[i]-exp_gradient_f[i])<1e-3);
//        QCOMPARE(comp_gradient[i], exp_gradient[i]); // does not like negative?
    }

    delete [] comp_gradient;

}

void TKiplMathTest::testPolyVal()
{
    std::vector<double> x={-2,-1,0,1,2};
    std::vector<double> y1={-4,-1,2,5,8};
    std::vector<double> coef1 = {2,3};

    auto y=kipl::math::polyVal(x,coef1);

    for (auto itRes=y.begin(), itExp=y1.begin(); itRes!=y.end(); ++itRes, ++itExp)
        QCOMPARE(*itRes,*itExp);
}

void TKiplMathTest::testPolyFit()
{
    std::vector<double> x={-8,-2,-1,0,1,2,15};
    std::vector<double> coef1 = {2,3};

    auto y1=kipl::math::polyVal(x,coef1);

    auto c=kipl::math::polyFit(x,y1,1);

    QCOMPARE(c[0],coef1[0]);
    QCOMPARE(c[1],coef1[1]);

    std::vector<double> coef2 = {2,3,1.5};

    auto y2=kipl::math::polyVal(x,coef2);

    auto c2=kipl::math::polyFit(x,y2,2);

    QCOMPARE(c2[0],coef2[0]);
    QCOMPARE(c2[1],coef2[1]);
    QCOMPARE(c2[2],coef2[2]);

    std::vector<double> coef3 = {2,3,1.5,-0.5};

    auto y3=kipl::math::polyVal(x,coef3);

    auto c3=kipl::math::polyFit(x,y3,3);

    QCOMPARE(c3[0],coef3[0]);
    QCOMPARE(c3[1],coef3[1]);
    QCOMPARE(c3[2],coef3[2]);
    QCOMPARE(c3[3],coef3[3]);


}

void TKiplMathTest::testPolyDeriv()
{
    std::vector<double> coef1 = {2,3};
    std::vector<double> dc1   = kipl::math::polyDeriv(coef1,1);
    QCOMPARE(dc1.size(),1UL);
    QCOMPARE(dc1[0],3.0);
    std::vector<double> ddc1  = kipl::math::polyDeriv(coef1,2);
    QCOMPARE(ddc1.size(),0UL);

    std::vector<double> coef2 = {2,3,1.5};
    std::vector<double> dc2   = kipl::math::polyDeriv(coef2,1);
    QCOMPARE(dc2.size(),2UL);
    QCOMPARE(dc2[0],3.0);
    QCOMPARE(dc2[1],3.0);

    std::vector<double> ddc2  = kipl::math::polyDeriv(coef2,2);
    QCOMPARE(ddc2.size(),1UL);
    QCOMPARE(ddc2[0],3.0);

    std::vector<double> coef3 = {2,3,1.5,-0.5};
    std::vector<double> dc3   = kipl::math::polyDeriv(coef3,1);

    QCOMPARE(dc3.size(),3UL);
    QCOMPARE(dc3[0],3.0);
    QCOMPARE(dc3[1],3.0);
    QCOMPARE(dc3[2],-1.5);

    std::vector<double> ddc3  = kipl::math::polyDeriv(coef3,2);
    QCOMPARE(ddc3.size(),2UL);
    QCOMPARE(ddc3[0],3.0);
    QCOMPARE(ddc3[1],-3.0);

}

QTEST_APPLESS_MAIN(TKiplMathTest)

#include "tst_tkiplmathtest.moc"

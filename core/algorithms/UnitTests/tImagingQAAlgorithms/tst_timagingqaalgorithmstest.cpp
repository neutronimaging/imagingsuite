#include <QString>
#include <QtTest>


#include <base/timage.h>
#include <base/index2coord.h>
#include <math/mathconstants.h>
#include <drawing/drawing.h>
#include <io/io_tiff.h>
#include <io/io_serializecontainers.h>
#include <math/statistics.h>
#include <filters/filter.h>

#include <contrastsampleanalysis.h>
#include <resolutionestimators.h>
#include <profileextractor.h>

class TImagingQAAlgorithmsTest : public QObject
{
    Q_OBJECT

public:
    TImagingQAAlgorithmsTest();
    kipl::base::TImage<float,2> makeEdgeImage(size_t N, float sigma, float angle);

private Q_SLOTS:
    void testContrastSampleAnalysis();
    void testResEstAdmin();
    void testResEstAlg();
    void testProfileExtractor();
};

TImagingQAAlgorithmsTest::TImagingQAAlgorithmsTest()
{
}

kipl::base::TImage<float,2> TImagingQAAlgorithmsTest::makeEdgeImage(size_t N, float sigma, float angle)
{
    size_t dims[2]={N,N};
    kipl::base::TImage<float,2> img(dims);

    float center=dims[0]/2.0f;
    float ycenter=dims[1]/2.0f;
    float k=tan(angle*fPi/180.0f);
    for (size_t y=0; y<dims[1]; ++y)
    {
        for (size_t x=0; x<dims[0]; ++x)
        {
            img(x,y)=1.0f/(1.0f+exp(-(x-center+k*(y-ycenter))/sigma));
        }
    }

    return img;
}

void TImagingQAAlgorithmsTest::testContrastSampleAnalysis()
{
    QSKIP("Skipping due to save time");
    ImagingQAAlgorithms::ContrastSampleAnalysis csa;
    const size_t N=512;
    size_t dims[2]={N,N};

    kipl::base::TImage<float,2> orig(dims);
    const float resolution=0.05f;
    const float radius=3.0f/resolution;
    const float ringRadius=9.0f/resolution;
    kipl::drawing::Circle<float> inset(radius);

    for (int i=0; i<6; ++i) {
        int x=static_cast<int>(N/2.0f+ringRadius*cos(i*fPi/3.0f));
        int y=static_cast<int>(N/2.0f+ringRadius*sin(i*fPi/3.0f));
        float val=1.0f+i*0.1f;
        inset.Draw(orig,x,y,val);
    }
    kipl::io::WriteTIFF32(orig,"csa_test_orig.tif");

    kipl::math::Statistics stats[6];
    kipl::base::coords3Df centers[6];

    csa.saveIntermediateImages=true;

    csa.setImage(orig);

    csa.analyzeContrast(resolution);

}

void TImagingQAAlgorithmsTest::testResEstAdmin()
{
    ImagingQAAlgorithms::ResolutionEstimator re;

    double s=1.0;
    re.setPixelSize(s);
    QCOMPARE(re.pixelSize(),s);
    const int N=21;

    double ddata[N]={0.00000000e+00,   0.00000000e+00,   0.00000000e+00,
                   0.00000000e+00,   0.00000000e+00,   1.93045414e-03,
                   2.02460930e-02,   1.25645318e-01,   4.93524759e-01,
                   1.27232554e+00,   2.27232554e+00,   3.05112632e+00,
                   3.41900577e+00,   3.52440499e+00,   3.54272063e+00,
                   3.54465108e+00,   3.54465108e+00,   3.54465108e+00,
                   3.54465108e+00,   3.54465108e+00,   3.54465108e+00};
    float fdata[N];
\
    re.setProfile(ddata, N);
    QCOMPARE(re.size(),N);
    double rdata[N];
    int NN=0;
    re.profile(rdata,NN);
    QCOMPARE(NN,N);

    for (int i=0; i<N; ++i)
        QCOMPARE(rdata[i],ddata[i]);

    re.clear();
    QCOMPARE(re.size(),0);

    std::copy_n(ddata,N,fdata);
    std::fill_n(rdata,N,0.0);

    re.setProfile(fdata, N);
    QCOMPARE(re.size(),N);
    NN=0;
    re.profile(rdata,NN);

    QCOMPARE(NN,N);

    for (int i=0; i<N; ++i)
        QCOMPARE(rdata[i],fdata[i]);
//    void   setProfile(double *p, int N, double d=1.0);
//    void   setProfile(std::vector<double> &p, double d=1.0);
//    void   setProfile(std::vector<float> &p, double d=1.0);
//    void   setProfile(TNT::Array1D<double> &p, double d=1.0);

}

void TImagingQAAlgorithmsTest::testResEstAlg()
{
    ImagingQAAlgorithms::ResolutionEstimator re;

    double s=1.0;
    re.setPixelSize(s);
    // Test data Gaussian
    double A=1;
    double center=10;
    double sigma=2;

    const int N=21;
    double data[N]={0.00000000e+00,   0.00000000e+00,   0.00000000e+00,
                   0.00000000e+00,   0.00000000e+00,   1.93045414e-03,
                   2.02460930e-02,   1.25645318e-01,   4.93524759e-01,
                   1.27232554e+00,   2.27232554e+00,   3.05112632e+00,
                   3.41900577e+00,   3.52440499e+00,   3.54272063e+00,
                   3.54465108e+00,   3.54465108e+00,   3.54465108e+00,
                   3.54465108e+00,   3.54465108e+00,   3.54465108e+00};

    re.setProfile(data, N);
    QCOMPARE(re.size(),N);

    QVERIFY(fabs(re.FWHM()-sigma*2*sqrt(log(2)))<0.001);
}

void TImagingQAAlgorithmsTest::testProfileExtractor()
{

    kipl::base::TImage<float,2> img=makeEdgeImage(50,2.0f,2.0f);

    kipl::io::WriteTIFF32(img,"slantededge.tif");

    ImagingQAAlgorithms::ProfileExtractor p;

    auto profile=p.getProfile(img);

    kipl::base::TImage<float,2> img2;
    kipl::io::ReadTIFF(img2,"../imagingsuite/core/algorithms/UnitTests/data/raw_edge.tif");

    profile=p.getProfile(img2);
  //  kipl::io::serializeContainer(profile.begin(),profile.end(),"profile.txt");

}

QTEST_APPLESS_MAIN(TImagingQAAlgorithmsTest)

#include "tst_timagingqaalgorithmstest.moc"

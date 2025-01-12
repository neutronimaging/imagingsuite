#include <QString>
#include <QtTest>
#include <QDebug>


#include <base/timage.h>
#include <base/index2coord.h>
#include <math/mathconstants.h>
#include <drawing/drawing.h>
#include <io/io_tiff.h>
#include <io/io_serializecontainers.h>
#include <math/statistics.h>
#include <filters/filter.h>
#include <strings/filenames.h>

#include <contrastsampleanalysis.h>
#include <resolutionestimators.h>
#include <profileextractor.h>


class TImagingQAAlgorithmsTest : public QObject
{
    Q_OBJECT

public:
    TImagingQAAlgorithmsTest();
    kipl::base::TImage<float, 2> makeEdgeImage(size_t N, float sigma, const std::vector<float> &coefs);
    kipl::base::TImage<float, 2> makeEdgeImage(size_t N, float sigma, float angle);

private Q_SLOTS:
    void testContrastSampleAnalysis();
    void testResEstAdmin();
    void testResEstAlg();
    void testProfileExtractor();

private:
    std::string data_path;
};

TImagingQAAlgorithmsTest::TImagingQAAlgorithmsTest()
{
    data_path = QT_TESTCASE_BUILDDIR;
    #ifdef __APPLE__
        data_path = data_path + "/../../../../../../TestData/";
    #elif defined(__linux__)
        data_path = data_path + "/../../../../../../TestData/";
    #else
        data_path = data_path + "/../../../../../TestData/";
    #endif
    kipl::strings::filenames::CheckPathSlashes(data_path,true);
}


kipl::base::TImage<float,2> TImagingQAAlgorithmsTest::makeEdgeImage(size_t N, float sigma, float angle)
{
    std::vector<size_t> dims={N,N};
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

kipl::base::TImage<float,2> TImagingQAAlgorithmsTest::makeEdgeImage(size_t N, float sigma, const std::vector<float> & coefs)
{
    std::vector<size_t> dims={N,N};
    kipl::base::TImage<float,2> img(dims);

    float m=coefs[0];
    float k=coefs[1];
    for (size_t y=0; y<dims[1]; ++y)
    {
        for (size_t x=0; x<dims[0]; ++x)
        {
            img(x,y)=1.0f/(1.0f+exp(-(x-m+k*y)/sigma));
        }
    }

    return img;
}
void TImagingQAAlgorithmsTest::testContrastSampleAnalysis()
{
  //  QSKIP("Skipping due to save time");
    ImagingQAAlgorithms::ContrastSampleAnalysis csa;
    const size_t N=512;
    std::vector<size_t> dims={N,N};

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
    kipl::io::WriteTIFF(orig,"csa_test_orig.tif",kipl::base::Float32);

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
    const size_t N=21;

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

    auto rdata = re.profile();
    QCOMPARE(rdata.size(),N);

    for (size_t i=0; i<N; ++i)
        QCOMPARE(rdata[i],ddata[i]);

    re.clear();
    QCOMPARE(re.size(),0);

    std::copy_n(ddata,N,fdata);
    std::fill_n(rdata.begin(),N,0.0);

    re.setProfile(fdata, N);
    QCOMPARE(re.size(),N);


    auto rdata2=re.profile();

    QCOMPARE(rdata2.size(),N);

    for (size_t i=0; i<N; ++i)
    {
        QCOMPARE(rdata2[i],fdata[i]);
    }
//    void   setProfile(double *p, int N, double d=1.0);
//    void   setProfile(std::vector<double> &p, double d=1.0);
//    void   setProfile(std::vector<float> &p, double d=1.0);

}

void TImagingQAAlgorithmsTest::testResEstAlg()
{
    ImagingQAAlgorithms::ResolutionEstimator re;

    double s=1.0;
    re.setPixelSize(s);
    // Test data Gaussian
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
    size_t N = 50;
    std::vector<float> coefs = {N/2.0f,0.1f};

    kipl::base::TImage<float,2> img=makeEdgeImage(50,2.0f,coefs);

    kipl::io::WriteTIFF(img,"slantededge.tif",kipl::base::Float32);

    ImagingQAAlgorithms::ProfileExtractor p;

    p.setPrecision(1.0f);
    p.saveImages = true;
    auto profile=p.getProfile(img);
    kipl::io::serializeMap(profile,"profile_synth.txt");

    auto dmap=p.distanceMap(img.dims());
    kipl::io::WriteTIFF(dmap,"edgedistmap.tif",kipl::base::Float32);

    QVERIFY(fabs(p.coefficients()[0]/coefs[0]-1)<0.01);
    QVERIFY(fabs(p.coefficients()[1]/coefs[1]-1)<0.01);

    kipl::base::TImage<float,2> img2;
    std::string fname= data_path+"2D/tiff/raw_edge.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img2,fname);

    p.saveImages = false;
    profile=p.getProfile(img2);

    qDebug() << "Profile length = "<<profile.size();
    kipl::io::serializeMap(profile,"profile_raw.txt");

}

#ifdef __APPLE__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    QTEST_APPLESS_MAIN(TImagingQAAlgorithmsTest)
    #pragma clang diagnostic pop
#else
    QTEST_APPLESS_MAIN(TImagingQAAlgorithmsTest)
#endif


#include "tst_ImagingQAAlgorithms.moc"

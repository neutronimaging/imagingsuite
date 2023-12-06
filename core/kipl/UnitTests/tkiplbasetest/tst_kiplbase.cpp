//<LICENSE>
#include <sstream>
#include <string>
#include <numeric>
#include <limits>

#include <QString>
#include <QtTest>
#include <QDebug>

#include <base/thistogram.h>
#include <base/timage.h>
#include <base/imageinfo.h>
#include <base/tsubimage.h>
#include <base/trotate.h>
#include <base/marginsetter.h>
#include <base/tprofile.h>
#include <base/imagecast.h>
#include <base/tpermuteimage.h>
#include <base/kiplenums.h>
#include <logging/logger.h>
#include <io/io_tiff.h>
#include <io/io_serializecontainers.h>

class Tkiplbase : public QObject
{
    Q_OBJECT

public:
    Tkiplbase();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    /// Test memory alignment
    void testMemoryAlignment();

    /// Tests for ImageInformation
    void testImageInfoCtor();
    void testImageInfoResolutions();

    /// Tests for histograms
    void testHighEntropyHistogram();

    /// Tests for BivariateHistogram
    void testBivariateHistogramInitialize();

    void testBivariateHistogram();

    void testHistogram();

    /// Tests cropping
    void testSubImage();

    /// Tests rotations
    void testRotateImage();

    /// Tests margin setter
    void testMarginSetter();

    /// Tests vertical and horizontal profiles
    void testProfiles();

    /// Test image casting
    void testImageCaster();

    void testTranspose();
    /// Test OS specifying enums
    void testOSenums();

    void testRotationDirection();

private:
    std::string data_path;
};

Tkiplbase::Tkiplbase()
{
    data_path = QT_TESTCASE_BUILDDIR;
    data_path = data_path + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(data_path,true);
}

void Tkiplbase::initTestCase()
{
}

void Tkiplbase::cleanupTestCase()
{
}

void Tkiplbase::testMemoryAlignment()
{
    const size_t alignment = 4UL;
    QCOMPARE(__STDCPP_DEFAULT_NEW_ALIGNMENT__ % alignment, 0UL);

    std::vector<size_t> sizes={4,8,32,64,1024};
    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new char[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }

    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new short[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }

    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new float[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }

    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new double[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }

    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new int[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }

    for (const auto size : sizes)
    {
        for (size_t i=0; i<4; ++i) 
        {
            auto p = new size_t[size-i];
            auto pp = reinterpret_cast<size_t>(p);
            QCOMPARE(pp % alignment, 0UL);
            delete [] p;
        }
    }
}

void Tkiplbase::testImageInfoCtor()
{
    kipl::base::ImageInfo infoA;

    QVERIFY(infoA.sSoftware=="KIPL image processing library");        ///< TIFF tag 305
    QVERIFY(infoA.sArtist=="Anders Kaestner");	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright=="Anders Kaestner");       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription=="");     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample==16);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel==1); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat==0);

    QVERIFY(infoA.GetMetricX()==1.0f);
    QVERIFY(infoA.GetMetricY()==1.0f);

    infoA.sArtist="Pelle";
    infoA.sCopyright="Somebody";
    infoA.sDescription="Hepp";
    infoA.nBitsPerSample=8;
    infoA.nSampleFormat=2;
    infoA.nSamplesPerPixel=3;
    infoA.SetMetricX(10.0f);
    infoA.SetMetricY(20.0f);

    kipl::base::ImageInfo infoB(infoA);

    QVERIFY(infoA.sSoftware         == infoB.sSoftware);        ///< TIFF tag 305
    QVERIFY(infoA.sArtist           == infoB.sArtist);	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright        == infoB.sCopyright);       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription      == infoB.sDescription);     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample    == infoB.nBitsPerSample);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel  == infoB.nSamplesPerPixel); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat     == infoB.nSampleFormat);

    QVERIFY(infoA.GetMetricX()==infoB.GetMetricX());
    QVERIFY(infoA.GetMetricY()==infoB.GetMetricY());

    kipl::base::ImageInfo infoC;
    infoC=infoA;

    QVERIFY(infoA.sSoftware         == infoC.sSoftware);        ///< TIFF tag 305
    QVERIFY(infoA.sArtist           == infoC.sArtist);	         ///< TIFF tag 315
    QVERIFY(infoA.sCopyright        == infoC.sCopyright);       ///< TIFF tag 33432
    QVERIFY(infoA.sDescription      == infoC.sDescription);     ///< TIFF tag 270
    QVERIFY(infoA.nBitsPerSample    == infoC.nBitsPerSample);   ///< Number of bits per sample as read from the image file
    QVERIFY(infoA.nSamplesPerPixel  == infoC.nSamplesPerPixel); ///< Number of samples per pixel in the image file. Mainly applies to color images
    QVERIFY(infoA.nSampleFormat     == infoC.nSampleFormat);

    QVERIFY(infoA.GetMetricX()==infoC.GetMetricX());
    QVERIFY(infoA.GetMetricY()==infoC.GetMetricY());
}



void Tkiplbase::testImageInfoResolutions()
{
    kipl::base::ImageInfo infoA;

    infoA.SetMetricX(10.0f);
    QVERIFY(infoA.GetDPCMX() == 1.0f);
    QVERIFY(infoA.GetDPIX()  == 25.4f/10.0f);

    infoA.SetMetricY(20.0f);
    QVERIFY(infoA.GetDPCMY() == 10.0f/20.0f);
    QVERIFY(infoA.GetDPIY()  == 25.4f/20.0f);

    infoA.SetDPCMX(10.0f);
    QVERIFY(infoA.GetMetricX()  == 1.0f);

}

void Tkiplbase::testHistogram()
{
    kipl::logging::Logger::SetLogLevel(kipl::logging::Logger::LogMessage);

    kipl::base::TImage<float,3> img({2101,1234,123});

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);

    std::vector<float> axis;
    std::vector<size_t> hist;
    kipl::base::Histogram(img.GetDataPtr(),img.Size(),1024UL,hist,axis,0.0f,0.0f,false);

    size_t cnt = 0UL;

    for (auto & val : hist)
        cnt += val;

    QCOMPARE(cnt,img.Size());
}

void Tkiplbase::testHighEntropyHistogram()
{
    kipl::base::TImage<float,2> img;
    std::string fname = data_path + "2D/tiff/spots/balls.tif";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::ReadTIFF(img,fname);
    std::vector<float> axis;
    std::vector<size_t> hist;
    kipl::base::Histogram(img.GetDataPtr(),img.Size(),1024UL,hist,axis,0.0f,0.0f,false);
    kipl::io::serializeContainer(axis.begin(),axis.end(),"axis1.txt");
    kipl::io::serializeContainer(hist.begin(),hist.end(),"hist1.txt");

    kipl::base::highEntropyHistogram(img.GetDataPtr(),img.Size(),1024UL,hist,axis,0.0f,0.0f,false);
    kipl::io::serializeContainer(axis.begin(),axis.end(),"axis2.txt");
    kipl::io::serializeContainer(hist.begin(),hist.end(),"hist2.txt");

}

void Tkiplbase::testBivariateHistogramInitialize()
{
    kipl::base::BivariateHistogram bihi;

    bihi.Initialize(0.0f,1.0f,10,2.0f,3.0f,20);

    auto lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    auto dims=bihi.Dims();
    QCOMPARE(dims[0],size_t(10));
    QCOMPARE(dims[1],size_t(20));

    bihi.Initialize(1.0f,0.0f,10,3.0f,2.0f,20);

    lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    auto dims2=bihi.Dims();
    QCOMPARE(dims2[0],size_t(10));
    QCOMPARE(dims2[1],size_t(20));
}

void Tkiplbase::testBivariateHistogram()
{
        kipl::base::BivariateHistogram bihi;
        bihi.Initialize(0.0f,10.0f,10,0.0f,20.0f,20);

        kipl::base::TImage<size_t,2> &img=bihi.Bins();

        auto dims=bihi.Dims();
        for (size_t y=0; y<dims[1]; y++)
            for (size_t x=0; x<dims[0]; x++)
                img(x,y)=x+y*dims[0];

        kipl::base::BivariateHistogram::BinInfo info=bihi.GetBin(0.0f,0.0f);

        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(-1.0f,0.0f);
        QCOMPARE(img(0,0),info.count);

        info=bihi.GetBin(1.0f,-1.0f);
        QCOMPARE(img(1,0),info.count);

        info=bihi.GetBin(10.0f,0.0f);
        QCOMPARE(img(9,0),info.count);

        info=bihi.GetBin(5.0f,10.0f);
        QCOMPARE(img(5,10),info.count);

        info=bihi.GetBin(10.0f,20.0f);
        QCOMPARE(img(9,19),info.count);

}

void Tkiplbase::testSubImage()
{
    std::ostringstream msg;

    std::vector<size_t> dims={10,12};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res;

    kipl::base::TSubImage<float,2> cropper;
    std::vector<size_t> crop={2,3,7,6};
    float val_img;
    float val_res;
    // Test get with crop coordinates
    {
        res=cropper.Get(img,crop,false);

        QVERIFY2(res.Size(0)==crop[2]-crop[0],"Cropped size error x");
        QVERIFY2(res.Size(1)==crop[3]-crop[1],"Cropped size error y");


        for (size_t j=0; j<res.Size(1); ++j) {
            for (size_t i=0; i<res.Size(0); ++i) {
                val_img=img.GetLinePtr(crop[1]+j)[crop[0]+i];
                val_res=res.GetLinePtr(j)[i];
                msg.str(""); msg<<"Pos: "<<i<<", "<<j<<" orig="<<val_img<<", res="<<val_res;
                QVERIFY2(val_img==val_res,msg.str().c_str());
            }
        }

        res=cropper.Get(img,crop,true);

        QVERIFY2(res.Size(0)==crop[2]-crop[0]+1,"Cropped size error x");
        QVERIFY2(res.Size(1)==crop[3]-crop[1]+1,"Cropped size error y");
        for (size_t j=0; j<res.Size(1); ++j) {
            for (size_t i=0; i<res.Size(0); ++i) {
                val_img=img.GetLinePtr(crop[1]+j)[crop[0]+i];
                val_res=res.GetLinePtr(j)[i];
                msg.str(""); msg<<"Pos: "<<i<<", "<<j<<" orig="<<val_img<<", res="<<val_res;
                QVERIFY2(val_img==val_res,msg.str().c_str());
            }
        }
    }

    // Test get pos and sizes
}

void Tkiplbase::testRotateImage()
{
    std::ostringstream msg;

    std::vector<size_t> dims={4,5};
    kipl::base::TImage<float,2> img(dims);
    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);

    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    // 16 17 18 19

    kipl::base::TImage<float,2> res;

    kipl::base::TRotate<float> rot;

    // Test rotate with 90 deg
    {
        res=rot.Rotate90(img);
        float rot90ref[]= {16, 12, 8,  4, 0,
                     17, 13, 9,  5, 1,
                     18, 14, 10, 6, 2,
                     19, 15, 11, 7, 3};
        QVERIFY2(res.Size(0)==img.Size(1),"Dim x error rot 90");
        QVERIFY2(res.Size(1)==img.Size(0),"Dim y error rot 90");

        for (size_t i=0; i<res.Size(); ++i) {
            msg.str("");
            msg<<"i="<<i;
            QVERIFY2(res[i]==rot90ref[i], msg.str().c_str());
        }
    }

    // Test rotate with 180 deg
    {
        res=rot.Rotate180(img);

        // 0  1  2  3
        // 4  5  6  7
        // 8  9  10 11
        // 12 13 14 15
        // 16 17 18 19
        float rot180ref[]= {  19, 18, 17, 16,
                              15, 14, 13, 12,
                              11, 10,  9,  8,
                               7,  6,  5,  4,
                               3,  2,  1,  0};

        QVERIFY2(res.Size(0)==img.Size(0),"Dim x error rot 180");
        QVERIFY2(res.Size(1)==img.Size(1),"Dim y error rot 180");

        for (size_t i=0; i<res.Size(); ++i) {
            msg.str("");
            msg<<"i="<<i;
            QVERIFY2(res[i]==rot180ref[i], msg.str().c_str());
        }
    }

    // Test rotate with 270 deg
    {
        res=rot.Rotate270(img);
        float rot270ref[]= {   3,  7, 11, 15, 19,
                               2,  6, 10, 14, 18,
                               1,  5,  9, 13, 17,
                               0,  4,  8, 12, 16};

        QVERIFY2(res.Size(0)==img.Size(1),"Dim x error rot 270");
        QVERIFY2(res.Size(1)==img.Size(0),"Dim y error rot 270");

        for (size_t i=0; i<res.Size(); ++i) {
            msg.str("");
            msg<<"i="<<i;
            QVERIFY2(res[i]==rot270ref[i], msg.str().c_str());
        }
    }

    // Test vertical mirror
    {
        res=rot.MirrorVertical(img);

        float vertref[]= {   16, 17, 18, 19,
                             12, 13, 14, 15,
                              8,  9, 10, 11,
                              4,  5,  6,  7,
                              0,  1,  2,  3};
        QVERIFY2(res.Size(0)==img.Size(0),"Dim x error vert mirror");
        QVERIFY2(res.Size(1)==img.Size(1),"Dim y error vert mirror");

        for (size_t i=0; i<res.Size(); ++i) {
            msg.str("");
            msg<<"i="<<i;
            QVERIFY2(res[i]==vertref[i], msg.str().c_str());
        }
    }

    // Test horizontal mirror
    {
        res=rot.MirrorHorizontal(img);

        float horref[]={ 3,  2,  1,  0,
                         7,  6,  5,  4,
                        11, 10,  9,  8,
                        15, 14, 13, 12,
                        19, 18, 17, 16};

        QVERIFY2(res.Size(0)==img.Size(0),"Dim x error hor mirror");
        QVERIFY2(res.Size(1)==img.Size(1),"Dim y error hor mirror");

        for (size_t i=0; i<res.Size(); ++i) {
            msg.str("");
            msg<<"i="<<i;
            QVERIFY2(res[i]==horref[i], msg.str().c_str());
        }
    }
}

void Tkiplbase::testMarginSetter()
{
    // 1D
    std::vector<size_t> dims1={10};

    kipl::base::TImage<float,1> img1D_orig(dims1);
    std::iota(img1D_orig.GetDataPtr(),img1D_orig.GetDataPtr()+img1D_orig.Size(),0);
    kipl::base::TImage<float,1> img1D;
    img1D.Clone(img1D_orig);
    float val=-1;
    size_t w=3;
    kipl::base::setMarginValue(img1D,w,val);
    size_t i=0;
    for (i=0; i<w; i++)
        QVERIFY(img1D[i]==val);
    for ( ; i<dims1[0]-w-1; ++i)
        QVERIFY(img1D[i]==img1D_orig[i]);
    for ( ; i<dims1[0]; ++i)
        QVERIFY(img1D[i]==val);

    // 2D
    std::vector<size_t> dims2={10,15};
    kipl::base::TImage<float,2> img2D_orig(dims2);
    std::iota(img2D_orig.GetDataPtr(),img2D_orig.GetDataPtr()+img2D_orig.Size(),0);
    kipl::base::TImage<float,2> img2D;
    img2D.Clone(img2D_orig);
    val=-1;
    w=3;
    kipl::base::setMarginValue(img2D,w,val);

    for (i=0; i<w*dims2[0]; i++)
        QVERIFY(img2D[i]==val);

    for (i=1; i<(w+1)*dims2[0]; i++) {
   //     qDebug() << i <<" "<< img2D[img2D.Size()-i];
        QVERIFY(img2D[img2D.Size()-i]==val);
    }
    size_t j=0;
    for (j=w; j<dims2[1]-w-1; ++j) {
        float *pLine=img2D.GetLinePtr(j);
        float *pOrig=img2D_orig.GetLinePtr(j);
        for (i=0; i<w; i++)
            QVERIFY(pLine[i]==val);
        for ( ; i<dims2[0]-w-1; ++i)
            QVERIFY(pLine[i]==pOrig[i]);
        for ( ; i<dims2[0]; ++i)
            QVERIFY(pLine[i]==val);
    }

    // 3D
    std::vector<size_t> dims3={10,15,20};
    kipl::base::TImage<float,3> img3D_orig(dims3);
    std::iota(img3D_orig.GetDataPtr(),img3D_orig.GetDataPtr()+img3D_orig.Size(),0);
    kipl::base::TImage<float,3> img3D;
    img3D.Clone(img3D_orig);
    val=-1;
    w=3;
    kipl::base::setMarginValue(img3D,w,val);

    for (i=0; i<w*dims3[0]*dims3[1]; i++)
        QVERIFY(img3D[i]==val);

    for (i=1; i<(w+1)*dims3[0]*dims3[1]; i++) {
     //   qDebug() << i <<" "<< img3D[img3D.Size()-i];
        QCOMPARE(img3D[img3D.Size()-i],val);
    }
    size_t k=0;
    for (k=w; k<dims3[2]-w-1; ++k) {

        for (i=0; i<w*dims3[0]; i++)
            QVERIFY(img3D[dims3[0]*dims3[1]*k+i]==val);

        for (i=1; i<(w+1)*dims3[0]; i++) {
        //    qDebug() << i <<" "<< img3D[img3D.Size()-i];
            QVERIFY(img3D[dims3[0]*dims3[1]*(k+1)-i]==val);
        }

        for (j=w; j<dims3[1]-w-1; ++j) {
            float *pLine=img3D.GetLinePtr(j,k);
            float *pOrig=img3D_orig.GetLinePtr(j,k);
            for (i=0; i<w; i++)
                QCOMPARE(pLine[i],val);
            for ( ; i<dims3[0]-w-1; ++i)
                QCOMPARE(pLine[i],pOrig[i]);
            for ( ; i<dims3[0]; ++i)
                QCOMPARE(pLine[i],val);
        }
    }
}

void Tkiplbase::testProfiles()
{
    std::vector<size_t> dims={4,3};
    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);
    float *vp=new float[dims[0]];
    float *hp=new float[dims[1]];
    kipl::base::VerticalProjection2D(img.GetDataPtr(), img.dims(), vp);
    //kipl::base::HorizontalProjection2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProjection=false);
    delete [] vp;
    delete [] hp;
}

void Tkiplbase::testImageCaster()
{
    std::vector<size_t> dims={50,60,1};

    kipl::base::TImage<float,2> imgref2d(dims);



    float scale=1.0f/static_cast<float>(imgref2d.Size());
    for (size_t i=0; i<imgref2d.Size(); ++i)
        imgref2d[i]=static_cast<float>(i);

    kipl::base::TImage<unsigned short,2> casted1=kipl::base::ImageCaster<unsigned short,float,2>::cast(imgref2d);

    QCOMPARE(casted1.Size(0),imgref2d.Size(0));
    QCOMPARE(casted1.Size(1),imgref2d.Size(1));

    for (size_t i=0; i<casted1.Size(); ++i) {
        QCOMPARE(casted1[i],static_cast<unsigned short>(i));
        QCOMPARE(casted1[i],static_cast<unsigned short>(imgref2d[i]));
    }


    for (size_t i=0; i<imgref2d.Size(); ++i)
        imgref2d[i]=static_cast<float>(i)*scale;

    float clampmin=0.1f;
    float clampmax=0.9f;
    kipl::base::TImage<unsigned short,2> casted2=kipl::base::ImageCaster<unsigned short,float,2>::cast(imgref2d,clampmin, clampmax);
    float transformScale=static_cast<float>(std::numeric_limits<unsigned short>::max())/(clampmax-clampmin);
    for (size_t i=0; i<casted1.Size(); ++i) {
        float val=(min(max(imgref2d[i],clampmin),clampmax)-clampmin)*transformScale;

        QCOMPARE(casted2[i],static_cast<unsigned short>(val));
    }

}

void Tkiplbase::testTranspose()
{
    kipl::base::Transpose<float> tr;

    // Test transpose of image with different size and not multiples of the buffer size
    for (size_t p=5; p<12; ++p)
    {

        size_t nx=(1<<p)-1;
        size_t ny=(1<<(p+1))+1;

        qDebug() << p << nx << ny;
        std::vector<size_t> dims={nx,ny};

        kipl::base::TImage<float,2> img(dims);
        kipl::base::TImage<float,2> img2;
        try
        {
            for (size_t i=0; i<img.Size(); ++i)
                img[i]=static_cast<float>(i);
            img2.Clone(img);

            img2=tr(img2);
        }
        catch (kipl::base::KiplException &e)
        {
            QFAIL(e.what());
        }

        QCOMPARE(img2.Size(0),img.Size(1));
        QCOMPARE(img2.Size(1),img.Size(0));

        for (size_t y=0; y<img.Size(1); ++y)
            for (size_t x=0; x<img.Size(0); ++x)
                QCOMPARE(img2(y,x),img(x,y));

        dims={ny,nx};

        img.resize(dims);
        img2.FreeImage();
        try
        {
            for (size_t i=0; i<img.Size(); ++i)
                img[i]=static_cast<float>(i);
            img2.Clone(img);

            img2=tr(img2);
        }
        catch (kipl::base::KiplException &e)
        {
            QFAIL(e.what());
        }

        QCOMPARE(img2.Size(0),img.Size(1));
        QCOMPARE(img2.Size(1),img.Size(0));

        for (size_t y=0; y<img.Size(1); ++y)
            for (size_t x=0; x<img.Size(0); ++x)
                QCOMPARE(img2(y,x),img(x,y));
    }

    // Test transpose of image smaller than the buffer size

    kipl::base::Transpose<float,32> tr2;
    std::vector<size_t> dims={16,1023};

    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> img2;
    try
    {
        for (size_t i=0; i<img.Size(); ++i)
            img[i]=static_cast<float>(i);
        img2.Clone(img);

        img2=tr2(img2);
    }
    catch (kipl::base::KiplException &e)
    {
        QFAIL(e.what());
    }

    QCOMPARE(img2.Size(0),img.Size(1));
    QCOMPARE(img2.Size(1),img.Size(0));

    for (size_t y=0; y<img.Size(1); ++y)
        for (size_t x=0; x<img.Size(0); ++x)
            QCOMPARE(img2(y,x),img(x,y));
}


void Tkiplbase::testOSenums()
{
    QCOMPARE(enum2string(kipl::base::OSLinux),std::string("OSLinux"));
    QCOMPARE(enum2string(kipl::base::OSWindows),std::string("OSWindows"));
    QCOMPARE(enum2string(kipl::base::OSMacOS),std::string("OSMacOS"));
    QCOMPARE(enum2string(kipl::base::OSUnknown),std::string("OSUnknown"));

    kipl::base::eOperatingSystem oe;
    string2enum("OSLinux",oe);
    QCOMPARE(oe,kipl::base::OSLinux);
    string2enum("OSWindows",oe);
    QCOMPARE(oe,kipl::base::OSWindows);
    string2enum("OSMacOS",oe);
    QCOMPARE(oe,kipl::base::OSMacOS);
    string2enum("OSUnknown",oe);
    QCOMPARE(oe,kipl::base::OSUnknown);

    QVERIFY_EXCEPTION_THROWN(string2enum("OSMacos",oe),kipl::base::KiplException);
}
// Test conversion of rotation direction to and from string, all combinations using QTest
void Tkiplbase::testRotationDirection()
{
    QCOMPARE(enum2string(kipl::base::RotationDirCW),std::string("RotationDirCW"));
    QCOMPARE(enum2string(kipl::base::RotationDirCCW),std::string("RotationDirCCW"));

    kipl::base::eRotationDirection oe;
    string2enum("cw",oe);
    QCOMPARE(oe,kipl::base::RotationDirCW);
    string2enum("ccw",oe);
    QCOMPARE(oe,kipl::base::RotationDirCCW);

    string2enum("CW",oe);
    QCOMPARE(oe,kipl::base::RotationDirCW);
    string2enum("CCW",oe);
    QCOMPARE(oe,kipl::base::RotationDirCCW);

    string2enum("RotationDirCW",oe);
    QCOMPARE(oe,kipl::base::RotationDirCW);
    string2enum("RotationDirCCW",oe);
    QCOMPARE(oe,kipl::base::RotationDirCCW);

    QVERIFY_EXCEPTION_THROWN(string2enum("RotationDirectionclockwise",oe),kipl::base::KiplException);
}

QTEST_APPLESS_MAIN(Tkiplbase)

#include "tst_kiplbase.moc"

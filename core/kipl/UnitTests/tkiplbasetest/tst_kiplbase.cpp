//<LICENSE>
#include <sstream>
#include <string>
#include <numeric>
#include <limits>

#include <QString>
#include <QtTest>

#include <base/thistogram.h>
#include <base/timage.h>
#include <base/imageinfo.h>
#include <base/tsubimage.h>
#include <base/trotate.h>
#include <base/marginsetter.h>
#include <base/tprofile.h>
#include <base/imagecast.h>
#include <base/kiplenums.h>

class TkiplbasetestTest : public QObject
{
    Q_OBJECT

public:
    TkiplbasetestTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    /// Tests for ImageInformation
    void testImageInfoCtor();
    void testImageInfoResolutions();

    /// Tests for BivariateHistogram
    void testBivariateHistogramInitialize();

    void testBivariateHistogram();

    /// Tests cropping
    void testSubImage();

    /// Tests rotations
    void testRotateImage();

    /// Tests margin setter
    void testMarginSetter();

    /// Tests vertical and horizsontal profiles
    void testProfiles();

    /// Test image casting
    void testImageCaster();

    /// Test OS specifying enums
    void testOSenums();

};

TkiplbasetestTest::TkiplbasetestTest()
{
}

void TkiplbasetestTest::initTestCase()
{
}

void TkiplbasetestTest::cleanupTestCase()
{
}

void TkiplbasetestTest::testImageInfoCtor()
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



void TkiplbasetestTest::testImageInfoResolutions()
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

void TkiplbasetestTest::testBivariateHistogramInitialize()
{
    kipl::base::BivariateHistogram bihi;

    bihi.Initialize(0.0f,1.0f,10,2.0f,3.0f,20);

    auto lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    size_t const *dims=bihi.Dims();
    QCOMPARE(dims[0],size_t(10));
    QCOMPARE(dims[1],size_t(20));

    bihi.Initialize(1.0f,0.0f,10,3.0f,2.0f,20);

    lim=bihi.GetLimits(0);
    QCOMPARE(lim.first,0.0f);
    QCOMPARE(lim.second,1.0f);

    lim=bihi.GetLimits(1);
    QCOMPARE(lim.first,2.0f);
    QCOMPARE(lim.second,3.0f);

    size_t const *dims2=bihi.Dims();
    QCOMPARE(dims2[0],size_t(10));
    QCOMPARE(dims2[1],size_t(20));
}

void TkiplbasetestTest::testBivariateHistogram()
{
        kipl::base::BivariateHistogram bihi;
        bihi.Initialize(0.0f,10.0f,10,0.0f,20.0f,20);

        kipl::base::TImage<size_t,2> &img=bihi.Bins();

        size_t const * dims=bihi.Dims();
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

void TkiplbasetestTest::testSubImage()
{
    std::ostringstream msg;

    size_t dims[2]={10,12};
    kipl::base::TImage<float,2> img(dims);
    kipl::base::TImage<float,2> res;

    kipl::base::TSubImage<float,2> cropper;
    size_t crop[4]={2,3,7,6};
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

void TkiplbasetestTest::testRotateImage()
{
    std::ostringstream msg;

    size_t dims[2]={4,5};
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

void TkiplbasetestTest::testMarginSetter()
{
    // 1D
    size_t dims[]={10,15,20};

    kipl::base::TImage<float,1> img1D_orig(dims);
    std::iota(img1D_orig.GetDataPtr(),img1D_orig.GetDataPtr()+img1D_orig.Size(),0);
    kipl::base::TImage<float,1> img1D;
    img1D.Clone(img1D_orig);
    float val=-1;
    size_t w=3;
    kipl::base::setMarginValue(img1D,w,val);
    size_t i=0;
    for (i=0; i<w; i++)
        QVERIFY(img1D[i]==val);
    for ( ; i<dims[0]-w-1; ++i)
        QVERIFY(img1D[i]==img1D_orig[i]);
    for ( ; i<dims[0]; ++i)
        QVERIFY(img1D[i]==val);

    // 2D
    kipl::base::TImage<float,2> img2D_orig(dims);
    std::iota(img2D_orig.GetDataPtr(),img2D_orig.GetDataPtr()+img2D_orig.Size(),0);
    kipl::base::TImage<float,2> img2D;
    img2D.Clone(img2D_orig);
    val=-1;
    w=3;
    kipl::base::setMarginValue(img2D,w,val);

    for (i=0; i<w*dims[0]; i++)
        QVERIFY(img2D[i]==val);

    for (i=1; i<(w+1)*dims[0]; i++) {
   //     qDebug() << i <<" "<< img2D[img2D.Size()-i];
        QVERIFY(img2D[img2D.Size()-i]==val);
    }
    size_t j=0;
    for (j=w; j<dims[1]-w-1; ++j) {
        float *pLine=img2D.GetLinePtr(j);
        float *pOrig=img2D_orig.GetLinePtr(j);
        for (i=0; i<w; i++)
            QVERIFY(pLine[i]==val);
        for ( ; i<dims[0]-w-1; ++i)
            QVERIFY(pLine[i]==pOrig[i]);
        for ( ; i<dims[0]; ++i)
            QVERIFY(pLine[i]==val);
    }

    // 3D
    kipl::base::TImage<float,3> img3D_orig(dims);
    std::iota(img3D_orig.GetDataPtr(),img3D_orig.GetDataPtr()+img3D_orig.Size(),0);
    kipl::base::TImage<float,3> img3D;
    img3D.Clone(img3D_orig);
    val=-1;
    w=3;
    kipl::base::setMarginValue(img3D,w,val);

    for (i=0; i<w*dims[0]*dims[1]; i++)
        QVERIFY(img3D[i]==val);

    for (i=1; i<(w+1)*dims[0]*dims[1]; i++) {
     //   qDebug() << i <<" "<< img3D[img3D.Size()-i];
        QCOMPARE(img3D[img3D.Size()-i],val);
    }
    size_t k=0;
    for (k=w; k<dims[2]-w-1; ++k) {

        for (i=0; i<w*dims[0]; i++)
            QVERIFY(img3D[dims[0]*dims[1]*k+i]==val);

        for (i=1; i<(w+1)*dims[0]; i++) {
        //    qDebug() << i <<" "<< img3D[img3D.Size()-i];
            QVERIFY(img3D[dims[0]*dims[1]*(k+1)-i]==val);
        }

        for (j=w; j<dims[1]-w-1; ++j) {
            float *pLine=img3D.GetLinePtr(j,k);
            float *pOrig=img3D_orig.GetLinePtr(j,k);
            for (i=0; i<w; i++)
                QCOMPARE(pLine[i],val);
            for ( ; i<dims[0]-w-1; ++i)
                QCOMPARE(pLine[i],pOrig[i]);
            for ( ; i<dims[0]; ++i)
                QCOMPARE(pLine[i],val);
        }
    }
}

void TkiplbasetestTest::testProfiles()
{
    size_t dims[2]={4,3};
    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=static_cast<float>(i);
    float *vp=new float[dims[0]];
    float *hp=new float[dims[1]];
    kipl::base::VerticalProjection2D(img.GetDataPtr(), img.Dims(), vp);
    //kipl::base::HorizontalProjection2D(const T *pData, const size_t *dims, S *pProfile, bool bMeanProjection=false);
    delete [] vp;
    delete [] hp;
}

void TkiplbasetestTest::testImageCaster()
{
    size_t dims[3]={50,60,1};

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

void TkiplbasetestTest::testOSenums()
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

QTEST_APPLESS_MAIN(TkiplbasetestTest)

#include "tst_kiplbase.moc"

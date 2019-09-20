#include <QtTest>

// add necessary includes here
#include <QDebug>
#include <sstream>

#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <morphology/morphgeo.h>
#include <morphology/morphextrema.h>
#include <io/io_tiff.h>

class morphgeo : public QObject
{
    Q_OBJECT

public:
    morphgeo();
    ~morphgeo();

private:
    void loadData();
    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> bilevelimg;

private slots:
    void test_RecByDilation();
    void test_RecByErosion();
    void testSelfDualReconstruction();

    // Derived algorithms
    void testRemoveEdgeObjects();
    void testRMin();
    void testRMax();
    void testhMax();
    void testExtendedMax();
    void testhMin();
    void testExtendedMin();
    void testMinImpose();
    void testFillHole();
    void testFillPeaks();
    //void testFillExtrema();
};

morphgeo::morphgeo()
{
    loadData();
}

morphgeo::~morphgeo()
{

}

void morphgeo::loadData()
{
#ifdef DEBUG
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/scroll_256.tif");
    kipl::io::ReadTIFF(bilevelimg,"../../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");
#else
    kipl::io::ReadTIFF(img,"../imagingsuite/core/kipl/UnitTests/data/scroll_256.tif");
    kipl::io::ReadTIFF(bilevelimg,"../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");
#endif
    kipl::io::WriteTIFF(img,"scroll_256.tif");
    kipl::io::WriteTIFF(bilevelimg,"bilevel_ws.tif");
}

void morphgeo::test_RecByDilation()
{
    kipl::base::TImage<float,2> img2;
    img2.Clone(img);
    img2+=1.0f;

    kipl::base::TImage<float,2> ref,dev;
    ref=kipl::morphology::old::RecByDilation(img2,img,kipl::morphology::conn8);
    try {
        dev=kipl::morphology::RecByDilation(img2,img,kipl::base::conn8);
    } catch (kipl::base::KiplException &e) {
        qDebug() << e.what();
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    kipl::io::WriteTIFF32(ref,"recdil_ref.tif");
    kipl::io::WriteTIFF32(dev,"recdil_dev.tif");
    QCOMPARE(cnt,0UL);
}

void morphgeo::test_RecByErosion()
{
    kipl::base::TImage<float,2> img2;
    img2.Clone(img);
    img2+=1.0f;

    kipl::base::TImage<float,2> ref,dev;
//    QBENCHMARK
    {
        ref=kipl::morphology::old::RecByErosion(img,img2,kipl::morphology::conn8);
    }
//   QBENCHMARK
    {
        dev=kipl::morphology::RecByErosion(img,img2,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    kipl::io::WriteTIFF32(ref,"recero_ref.tif");
    kipl::io::WriteTIFF32(dev,"recero_dev.tif");
    QCOMPARE(cnt,0UL);
}

void morphgeo::testSelfDualReconstruction()
{
    kipl::base::TImage<float,2> img;
#ifdef DEBUG
    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/scroll_256.tif");
#else
    kipl::io::ReadTIFF(img,"../imagingsuite/core/kipl/UnitTests/data/scroll_256.tif");
#endif

    kipl::base::TImage<float,2> img2;
    img2.Clone(img);
    img2+=1.0f;

    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        ref=kipl::morphology::old::SelfDualReconstruction(img,img2,kipl::morphology::conn8);
    }

    //QBENCHMARK
    {
        dev=kipl::morphology::SelfDualReconstruction(img,img2,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"recself_ref.tif");
    kipl::io::WriteTIFF32(dev,"recselg_dev.tif");
}

void morphgeo::testRemoveEdgeObjects()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        ref=kipl::morphology::old::RemoveEdgeObj(bilevelimg,kipl::morphology::conn8);
    }

    //QBENCHMARK
    {
        dev=kipl::morphology::RemoveEdgeObj(bilevelimg,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"removeedges_ref.tif");
    kipl::io::WriteTIFF32(dev,"removeedges_dev.tif");
}

void morphgeo::testRMin()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::RMin(img,ref,kipl::morphology::conn8,false);
    }

    //QBENCHMARK
    {
        kipl::morphology::RMin(img,dev,kipl::base::conn8,false);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    kipl::io::WriteTIFF32(ref,"rmin_ref.tif");
    kipl::io::WriteTIFF32(dev,"rmin_dev.tif");

    QCOMPARE(cnt,0UL);
}

void morphgeo::testRMax()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::RMax(img,ref,kipl::morphology::conn8);
    }

    //QBENCHMARK
    {
        kipl::morphology::RMax(img,dev,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"rmax_ref.tif");
    kipl::io::WriteTIFF32(dev,"rmax_dev.tif");
}

void morphgeo::testhMax()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::hMax(img,ref,5.0f,kipl::morphology::conn8);
    }

    QBENCHMARK
    {
        kipl::morphology::hMax(img,dev,5.0f,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"hmax_ref.tif");
    kipl::io::WriteTIFF32(dev,"hmax_dev.tif");
}

void morphgeo::testExtendedMax()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::ExtendedMax(img,ref,5.0f,kipl::morphology::conn8);
    }

    //QBENCHMARK
    {
        kipl::morphology::ExtendedMax(img,dev,5.0f,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"emax_ref.tif");
    kipl::io::WriteTIFF32(dev,"emax_dev.tif");
}

void morphgeo::testhMin()
{
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::hMin(img,ref,5.0f,kipl::morphology::conn8);
    }

    //QBENCHMARK
    {
        kipl::morphology::hMin(img,dev,5.0f,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    QCOMPARE(cnt,0UL);

    kipl::io::WriteTIFF32(ref,"hmin_ref.tif");
    kipl::io::WriteTIFF32(dev,"hmin_dev.tif");
}

void morphgeo::testExtendedMin()
{
    loadData();
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        kipl::morphology::old::ExtendedMin(img,ref,5.0f,kipl::morphology::conn4);
    }

    //QBENCHMARK
    {
        kipl::morphology::ExtendedMin(img,dev,5.0f,kipl::base::conn4);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }
    kipl::io::WriteTIFF32(ref,"emin_ref.tif");
    kipl::io::WriteTIFF32(dev,"emin_dev.tif");
    QCOMPARE(cnt,0UL);
}

void morphgeo::testMinImpose()
{
    QSKIP("Not implemented");
}

void morphgeo::testFillHole()
{
    loadData();
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        ref=kipl::morphology::old::FillHole(img,kipl::morphology::conn8);
    }

    QBENCHMARK
    {
        dev=kipl::morphology::FillHole(img,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    kipl::io::WriteTIFF32(ref,"fillhole_ref.tif");
    kipl::io::WriteTIFF32(dev,"fillhole_dev.tif");

    QCOMPARE(cnt,0UL);
}

void morphgeo::testFillPeaks()
{
    loadData();
    kipl::base::TImage<float,2> ref,dev;
    //QBENCHMARK
    {
        ref=kipl::morphology::old::FillPeaks(img,kipl::morphology::conn8);
    }

    QBENCHMARK
    {
        dev=kipl::morphology::FillPeaks(img,kipl::base::conn8);
    }

    QCOMPARE(ref.Size(),dev.Size());
    std::ostringstream msg;
    size_t cnt=0;
    for (size_t i=0; i<ref.Size(); ++i)
    {
        if (ref[i]!=dev[i])
            cnt++;
    }

    kipl::io::WriteTIFF32(ref,"fillpeaks_ref.tif");
    kipl::io::WriteTIFF32(dev,"fillpeaks_dev.tif");

    QCOMPARE(cnt,0UL);
}


QTEST_APPLESS_MAIN(morphgeo)

#include "tst_morphgeo.moc"



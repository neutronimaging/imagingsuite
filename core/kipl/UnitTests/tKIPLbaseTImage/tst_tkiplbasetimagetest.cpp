#include <iostream>
#include <QString>
#include <QtTest>
#include <base/timage.h>
#include <base/core/sharedbuffer.h>
#include <base/KiplException.h>

class TKIPLbaseTImageTest : public QObject
{
    Q_OBJECT

public:
    TKIPLbaseTImageTest();

private Q_SLOTS:
    void testSharedBuffer();
    void testConstructors();
    void testAssignment();
    void testScalarArithmetics();
    void testDataAccess();
    void testClones();
    void testExternalBuffer();
};

TKIPLbaseTImageTest::TKIPLbaseTImageTest()
{
}

void TKIPLbaseTImageTest::testSharedBuffer()
{
    // Constructors
    kipl::base::core::buffer<float> a(10);

    QCOMPARE(a.Size(),10UL);
    QVERIFY(a.References()==1);
    QVERIFY(reinterpret_cast<size_t>(a.GetDataPtr()) % 32 == 0UL); // Memory alignment on 4x4 bytes

    for (size_t i=0; i<a.Size(); ++i)
        a[i]=static_cast<float>(i);

    for (size_t i=0; i<a.Size(); ++i) {
        QCOMPARE(a[i],static_cast<float>(i));
    }

    kipl::base::core::buffer<float> b=a;
    QVERIFY(a.Size()==10);
    QVERIFY(b.Size()==10);

    QVERIFY(a.References()==2);
    QVERIFY(b.References()==2);

    for (size_t i=0; i<a.Size(); ++i)
        a[i]=static_cast<float>(i);

    QCOMPARE(a.GetDataPtr(),b.GetDataPtr());
    b.Clone();
    QVERIFY(a.Size()==10);
    QVERIFY(b.Size()==10);

    QVERIFY(a.References()==1);
    QVERIFY(b.References()==1);

    QVERIFY(a.GetDataPtr()!=b.GetDataPtr());
    QVERIFY((a==b)==false);
    QVERIFY((a!=b)==true);

    for (size_t i=0; i<a.Size(); i++)
        QCOMPARE(a[i],b[i]);

    b=a;
    QVERIFY(a.Size()==10);
    QVERIFY(b.Size()==10);

    QVERIFY(a.References()==2);
    QVERIFY(b.References()==2);

    QVERIFY(a.GetDataPtr()==b.GetDataPtr());
    QVERIFY((a==b)==true);
    QVERIFY((a!=b)==false);

    // Resize
    a.Resize(20);

    QVERIFY(a.References()==1);
    QVERIFY(b.References()==1);

    QVERIFY(a!=b);

    QVERIFY(a.Size()==20);
    QVERIFY(b.Size()==10);

    // Ridiculusly huge allocation
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException,kipl::base::core::buffer<float> c(100000000000000UL));
}

void TKIPLbaseTImageTest::testConstructors()
{
    { // Empty image
    kipl::base::TImage<float,1> img1D;
    QVERIFY(img1D.Size()==0);

    kipl::base::TImage<float,2> img2D;
    QVERIFY(img2D.Size()==0);

    kipl::base::TImage<float,3> img3D;
    QVERIFY(img3D.Size()==0);
    }

    // Set dimensions

    std::vector<size_t> dims1={3};
    kipl::base::TImage<float,1> img1D(dims1);
    QVERIFY(img1D.Size()==dims1[0]);
    QVERIFY(img1D.Size(0)==dims1[0]);
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException,img1D.Size(1));

    std::vector<size_t> dims2={3,4};
    kipl::base::TImage<float,2> img2D(dims2);
    QVERIFY(img2D.Size()==dims2[0]*dims2[1]);
    QVERIFY(img2D.Size(0)==dims2[0]);
    QVERIFY(img2D.Size(1)==dims2[1]);
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException,img2D.Size(2));

    std::vector<size_t> dims3={3,4,5};
    kipl::base::TImage<float,3> img3D(dims3);
    QVERIFY(img3D.Size()==dims3[0]*dims3[1]*dims3[2]);
    QVERIFY(img3D.Size(0)==dims3[0]);
    QVERIFY(img3D.Size(1)==dims3[1]);
    QVERIFY(img3D.Size(2)==dims3[2]);
    QVERIFY_THROWS_EXCEPTION(kipl::base::KiplException,img3D.Size(3));

    // Copy constructor
    kipl::base::TImage<float,2> img2Db(img2D);

    QVERIFY(img2D.Size()==img2Db.Size());
    QVERIFY(img2D.Size(0)==img2Db.Size(0));
    QVERIFY(img2D.Size(1)==img2Db.Size(1));
    QVERIFY(img2D.GetDataPtr()==img2Db.GetDataPtr());

    for (size_t i=0; i<img2D.Size(); ++i)
        img2D[i]=i;

    img2Db.Clone();
    QVERIFY(img2D.Size()==img2Db.Size());
    QVERIFY(img2D.Size(0)==img2Db.Size(0));
    QVERIFY(img2D.Size(1)==img2Db.Size(1));
    QVERIFY(img2D.GetDataPtr()!=img2Db.GetDataPtr());

    for (size_t i=0; i<img2D.Size(); ++i)
        QCOMPARE(img2Db[i],img2D[i]);

    dims3[0]=6; dims3[1]=7; dims3[2]=8;
    img3D.resize(dims3);
    QVERIFY(img3D.Size()==dims3[0]*dims3[1]*dims3[2]);
    QVERIFY(img3D.Size(0)==dims3[0]);
    QVERIFY(img3D.Size(1)==dims3[1]);
    QVERIFY(img3D.Size(2)==dims3[2]);

}

void TKIPLbaseTImageTest::testAssignment()
{
    std::vector<size_t> dims={3,4};
    kipl::base::TImage<float,2> a(dims);

    a=0.0f;
    for (size_t i=0; i<a.Size(); ++i)
        QVERIFY(a[i]==0.0f);

    a=3.1415f;
    for (size_t i=0; i<a.Size(); ++i)
        QCOMPARE(a[i],3.1415f);

    QVERIFY(a.References()==1);
    kipl::base::TImage<float,2> b;
    b=a;
    QVERIFY(a.References()==2);
    QVERIFY(b.References()==2);
    QVERIFY(a.Size()  == b.Size());
    QVERIFY(a.Size(0) == b.Size(0));
    QVERIFY(a.Size(1) == b.Size(1));
    QVERIFY(a.GetDataPtr() == b.GetDataPtr());

}

void TKIPLbaseTImageTest::testScalarArithmetics()
{
    std::vector<size_t> dims={4,4};

    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=i;

    kipl::base::TImage<float,2> res;

    res=img+2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(res[i],i+2.0f);

    res=img-2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(res[i],i-2.0f);

    res=img*2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(res[i],i*2.0f);

    res=img/2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QCOMPARE(res[i],i/2.0f);

}

void TKIPLbaseTImageTest::testClones()
{
    std::vector<size_t> dims={100,110};

    kipl::base::TImage<float,2> a(dims),b,c;
    for (size_t i=0; i<a.Size(); i++)
        a[i]=static_cast<float>(i);

    a=b;

    QVERIFY(a.GetDataPtr()==b.GetDataPtr());
    b.Clone();
    QVERIFY(a.GetDataPtr()!=b.GetDataPtr());

    for (size_t i=0; i<a.Size(); i++)
        QCOMPARE(a[i],b[i]);

    c.Clone(a);

    QVERIFY(a.GetDataPtr()!=c.GetDataPtr());
    QVERIFY(a.Size()==c.Size());
    QVERIFY(a.Size(0)==c.Size(0));
    QVERIFY(a.Size(1)==c.Size(1));

    for (size_t i=0; i<a.Size(); i++)
        QCOMPARE(a[i],c[i]);

}

void TKIPLbaseTImageTest::testExternalBuffer()
{
    std::vector<size_t> dims1={3,4,5};
    std::vector<size_t> dims2={6,7,8};

    kipl::base::TImage<float,3> img1(dims1);
    QCOMPARE(img1.haveExternalBuffer(),false);

    const size_t N=dims2[0]*dims2[1]*dims2[2];
    float *buffer = new float[N];
    kipl::base::TImage<float,3> img2(buffer,dims2);
    QCOMPARE(img2.haveExternalBuffer(),true);
    QCOMPARE(img2.Size(),N);
    QCOMPARE(img2.Size(0),dims2[0]);
    QCOMPARE(img2.Size(1),dims2[1]);
    QCOMPARE(img2.Size(2),dims2[2]);

    for (size_t i=0; i<N; ++i)
    {
        img2[i]=static_cast<float>(i);
        QCOMPARE(img2[i],buffer[i]);

    }

    img2=img1;

    QCOMPARE(img2.haveExternalBuffer(),false);
    QCOMPARE(img2.Size(),img1.Size());

    kipl::base::TImage<float,3> img3(buffer,dims2);
    for (size_t i=0; i<N; ++i)
    {
        QCOMPARE(img3[i],buffer[i]);
    }

    std::vector<size_t> dims3={3,2,1};
    QCOMPARE(img3.haveExternalBuffer(),true);
    QCOMPARE(img3.References(),1);
    kipl::base::TImage<float,3> img4=img3;
    QCOMPARE(img3.References(),2);
    QCOMPARE(img4.References(),2);
    QCOMPARE(img3.Size(),N);
    img3.resize(dims3);
    QCOMPARE(img3.References(),1);
    QCOMPARE(img4.References(),1);

    for (size_t i=0; i<N; ++i)
    {
        QCOMPARE(img4[i],buffer[i]);
    }

    QCOMPARE(img3.Size(),6UL);
    QCOMPARE(img3.haveExternalBuffer(),false);

    kipl::base::TImage<float,3> img5=img4;
    QCOMPARE(img5.References(),2);
    QCOMPARE(img4.References(),2);

    img5.Clone();
    QCOMPARE(img5.References(),1);
    QCOMPARE(img4.References(),1);
    QVERIFY(img4.GetDataPtr()!=img5.GetDataPtr());

    img3.Clone(img4);
    QCOMPARE(img3.Size(),img4.Size());
    QVERIFY(img3.GetDataPtr()!=img4.GetDataPtr());

    for (size_t i=0; i<N; ++i)
    {
        QCOMPARE(img4[i],img3[i]);
    }

    delete [] buffer;
}

void TKIPLbaseTImageTest::testDataAccess()
{
    // Pointers vs indexing

    // indexing outside buffer

}
QTEST_APPLESS_MAIN(TKIPLbaseTImageTest)

#include "tst_tkiplbasetimagetest.moc"

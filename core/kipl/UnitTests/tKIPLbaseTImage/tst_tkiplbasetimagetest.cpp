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
};

TKIPLbaseTImageTest::TKIPLbaseTImageTest()
{
}

void TKIPLbaseTImageTest::testSharedBuffer()
{
    // Constructors
    kipl::base::core::buffer<float> a(10);

    QVERIFY(a.Size()==10);
    QVERIFY(a.References()==1);
    QVERIFY(reinterpret_cast<size_t>(a.GetDataPtr()) % 32 == 0UL); // Memory alignment on 4x4 bytes

    for (size_t i=0; i<a.Size(); ++i)
        a[i]=static_cast<float>(i);

    for (size_t i=0; i<a.Size(); ++i) {
        QVERIFY(a[i]==static_cast<float>(i));
    }

    kipl::base::core::buffer<float> b=a;
    QVERIFY(a.Size()==10);
    QVERIFY(b.Size()==10);

    QVERIFY(a.References()==2);
    QVERIFY(b.References()==2);

    for (size_t i=0; i<a.Size(); ++i)
        a[i]=static_cast<float>(i);

    QVERIFY(a.GetDataPtr()==b.GetDataPtr());
    b.Clone();
    QVERIFY(a.Size()==10);
    QVERIFY(b.Size()==10);

    QVERIFY(a.References()==1);
    QVERIFY(b.References()==1);

    QVERIFY(a.GetDataPtr()!=b.GetDataPtr());
    QVERIFY((a==b)==false);
    QVERIFY((a!=b)==true);

    for (size_t i=0; i<a.Size(); i++)
        QVERIFY(a[i]==b[i]);

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
    QVERIFY_EXCEPTION_THROWN(kipl::base::core::buffer<float> c(100000000000000UL),kipl::base::KiplException);
}

void TKIPLbaseTImageTest::testConstructors()
{
    size_t dims[]={3,4,5,6};
    { // Empty image
    kipl::base::TImage<float,1> img1D;
    QVERIFY(img1D.Size()==0);

    kipl::base::TImage<float,2> img2D;
    QVERIFY(img2D.Size()==0);

    kipl::base::TImage<float,3> img3D;
    QVERIFY(img3D.Size()==0);
    }

    // Set dimensions
    kipl::base::TImage<float,1> img1D(dims);
    QVERIFY(img1D.Size()==dims[0]);
    QVERIFY(img1D.Size(0)==dims[0]);
    QVERIFY_EXCEPTION_THROWN(img1D.Size(1),kipl::base::KiplException);

    kipl::base::TImage<float,2> img2D(dims);
    QVERIFY(img2D.Size()==dims[0]*dims[1]);
    QVERIFY(img2D.Size(0)==dims[0]);
    QVERIFY(img2D.Size(1)==dims[1]);
    QVERIFY_EXCEPTION_THROWN(img2D.Size(2),kipl::base::KiplException);

    kipl::base::TImage<float,3> img3D(dims);
    QVERIFY(img3D.Size()==dims[0]*dims[1]*dims[2]);
    QVERIFY(img3D.Size(0)==dims[0]);
    QVERIFY(img3D.Size(1)==dims[1]);
    QVERIFY(img3D.Size(2)==dims[2]);
    QVERIFY_EXCEPTION_THROWN(img2D.Size(3),kipl::base::KiplException);


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
        QVERIFY(img2Db[i]==img2D[i]);

    dims[0]=6; dims[1]=7; dims[2]=8;
    img3D.Resize(dims);
    QVERIFY(img3D.Size()==dims[0]*dims[1]*dims[2]);
    QVERIFY(img3D.Size(0)==dims[0]);
    QVERIFY(img3D.Size(1)==dims[1]);
    QVERIFY(img3D.Size(2)==dims[2]);

}

void TKIPLbaseTImageTest::testAssignment()
{
    size_t dims[]={3,4};
    kipl::base::TImage<float,2> a(dims);

    a=0.0f;
    for (size_t i=0; i<a.Size(); ++i)
        QVERIFY(a[i]==0.0f);

    a=3.1415f;
    for (size_t i=0; i<a.Size(); ++i)
        QVERIFY(a[i]==3.1415f);

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
    size_t dims[2]={4,4};

    kipl::base::TImage<float,2> img(dims);

    for (size_t i=0; i<img.Size(); ++i)
        img[i]=i;

    kipl::base::TImage<float,2> res;

    res=img+2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QVERIFY(res[i]==i+2.0f);

    res=img-2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QVERIFY(res[i]==i-2.0f);

    res=img*2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QVERIFY(res[i]==i*2.0f);

    res=img/2.0f;

    QVERIFY(res.Size(0)==img.Size(0));
    QVERIFY(res.Size(1)==img.Size(1));
    QVERIFY(res.Size()==img.Size());

    for (size_t i=0; i<img.Size(); ++i)
        QVERIFY(res[i]==i/2.0f);

}

void TKIPLbaseTImageTest::testClones()
{
    size_t dims[]={100,110};

    kipl::base::TImage<float,2> a(dims),b,c;
    for (int i=0; i<a.Size(); i++)
        a[i]=float(i);

    a=b;

    QVERIFY(a.GetDataPtr()==b.GetDataPtr());
    b.Clone();
    QVERIFY(a.GetDataPtr()!=b.GetDataPtr());

    for (int i=0; i<a.Size(); i++)
        QVERIFY(a[i]==b[i]);

    c.Clone(a);

    QVERIFY(a.GetDataPtr()!=c.GetDataPtr());
    QVERIFY(a.Size()==c.Size());
    QVERIFY(a.Size(0)==c.Size(0));
    QVERIFY(a.Size(1)==c.Size(1));

    for (int i=0; i<a.Size(); i++)
        QVERIFY(a[i]==c[i]);

}

void TKIPLbaseTImageTest::testDataAccess()
{
    // Pointers vs indexing

    // indexing outside buffer

}
QTEST_APPLESS_MAIN(TKIPLbaseTImageTest)

#include "tst_tkiplbasetimagetest.moc"

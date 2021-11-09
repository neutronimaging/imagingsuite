#include <QtTest>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>
#include <vector>

#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <ImagingException.h>
#include <ReferenceImageCorrection.h>
// add necessary includes here

class TestReferenceImageCorrection : public QObject
{
    Q_OBJECT

public:
    TestReferenceImageCorrection();
    ~TestReferenceImageCorrection();

private slots:

    void test_Initialization();
    void test_Enums();
    void test_PlainNormalization();

};

TestReferenceImageCorrection::TestReferenceImageCorrection()
{

}

TestReferenceImageCorrection::~TestReferenceImageCorrection()
{

}

void TestReferenceImageCorrection::test_Initialization()
{
    ImagingAlgorithms::ReferenceImageCorrection bb;
}

void TestReferenceImageCorrection::test_Enums()
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod> strmap;
    strmap["otsumask"]            = ImagingAlgorithms::ReferenceImageCorrection::otsuMask;
    strmap["manuallythresholdedmask"] = ImagingAlgorithms::ReferenceImageCorrection::manuallyThresholdedMask;
    strmap["userdefinedmask"]         = ImagingAlgorithms::ReferenceImageCorrection::userDefinedMask;
    strmap["referencefreemask"]       = ImagingAlgorithms::ReferenceImageCorrection::referenceFreeMask;

    ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod em;
    for (auto & item : strmap)
    {
        QCOMPARE(item.first,enum2string(item.second));
        string2enum(item.first,em);
        QCOMPARE(item.second,em);
    }
    em=static_cast<ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod>(9999);
    QVERIFY_EXCEPTION_THROWN(enum2string(em),ImagingException);
    QVERIFY_EXCEPTION_THROWN(string2enum("flipfolp",em),ImagingException);
}

void TestReferenceImageCorrection::test_PlainNormalization()
{
    kipl::base::TImage<float,2> img,dc,ob,result,groundtruth;
    std::string imgname = "";
    std::string obname  = "";
    std::string dcname  = "";
    std::string gtname  = "";


    kipl::io::ReadFITS(img,imgname);
    kipl::io::ReadFITS(ob,obname);
    kipl::io::ReadFITS(dc,dcname);
    kipl::io::ReadTIFF(groundtruth,gtname);

    std::vector<size_t> doseROI = {10,10,100,100};

    ImagingAlgorithms::ReferenceImageCorrection bb;





}

QTEST_APPLESS_MAIN(TestReferenceImageCorrection)

#include "tst_referenceimagecorrection.moc"

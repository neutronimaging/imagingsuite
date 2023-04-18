//<LICENSE>

#include <sstream>
#include <iostream>
#include <map>
#include <cmath>

#include <QtCore/QString>
#include <QtTest/QtTest>


#include <base/timage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>
#include <io/io_csv.h>

#include <ImagingException.h>
#include <ReferenceImageCorrection.h>


class TestScatterCorrection : public QObject
{
    Q_OBJECT
    
public:
    TestScatterCorrection();
    
private Q_SLOTS:
    void RefImgCorrection_Initialization();

    void RefImgCorrection_enums();
    void testSegmentation();

private:
    std::string dataPath;
    kipl::base::TImage<float,2> holes;
    std::map<size_t,float> points;
    size_t pos1;
    size_t pos2;


};

TestScatterCorrection::TestScatterCorrection() 
{
    dataPath = QT_TESTCASE_BUILDDIR;
    dataPath = dataPath + "/../../../../../TestData/";
    kipl::strings::filenames::CheckPathSlashes(dataPath,true);
}


void TestScatterCorrection::RefImgCorrection_Initialization()
{
    ImagingAlgorithms::ReferenceImageCorrection bb;
}

void TestScatterCorrection::RefImgCorrection_enums()
{
    std::map<std::string, ImagingAlgorithms::ReferenceImageCorrection::eMaskCreationMethod> strmap;
    strmap["otsumask"]                = ImagingAlgorithms::ReferenceImageCorrection::otsuMask;
    strmap["rosinmask"]                = ImagingAlgorithms::ReferenceImageCorrection::rosinMask;
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

void TestScatterCorrection::testSegmentation()
{
    ImagingAlgorithms::ReferenceImageCorrection bb;
    kipl::base::TImage<float,2> img;
    kipl::base::TImage<float,2> res; 
    std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);

    bb.SegmentBlackBody(img,res);
    kipl::io::WriteTIFF(res,"res.tif",kipl::base::Float32);
}

QTEST_APPLESS_MAIN(TestScatterCorrection)

#include "tst_ScatterCorrection.moc"

#include <QString>
#include <QtTest>

#include <sstream>
#include <base/KiplException.h>
#include <analyzefileext.h>
#include <datasetbase.h>

class TReaderConfigTest : public QObject
{
    Q_OBJECT

public:
    TReaderConfigTest();

private Q_SLOTS:
    void testAnalyzeFileExt();
    void testDataSetBase();
};

TReaderConfigTest::TReaderConfigTest()
{
}

void TReaderConfigTest::testAnalyzeFileExt()
{
    // Tests functions in analyzefileext.h
    readers::eExtensionTypes et;
    string2enum("txt", et);  QCOMPARE(et,readers::ExtensionTXT);
    string2enum(".txt", et);  QCOMPARE(et,readers::ExtensionTXT);
    string2enum("tXt", et);  QCOMPARE(et,readers::ExtensionTXT);

    string2enum("dmp", et);  QCOMPARE(et,readers::ExtensionDMP);
    string2enum("dat", et);  QCOMPARE(et,readers::ExtensionDAT);
    string2enum("xml", et);  QCOMPARE(et,readers::ExtensionXML);
    string2enum("raw", et);  QCOMPARE(et,readers::ExtensionRAW);
    string2enum("fit", et);  QCOMPARE(et,readers::ExtensionFITS);
    string2enum("fts", et);  QCOMPARE(et,readers::ExtensionFITS);
    string2enum("fits", et); QCOMPARE(et,readers::ExtensionFITS);
    string2enum("png", et);  QCOMPARE(et,readers::ExtensionPNG);
    string2enum("jpg", et);  QCOMPARE(et,readers::ExtensionJPG);
    string2enum("jpeg", et); QCOMPARE(et,readers::ExtensionJPG);
    string2enum("tif", et);  QCOMPARE(et,readers::ExtensionTIFF);
    string2enum("tiff", et); QCOMPARE(et,readers::ExtensionTIFF);
    string2enum("mat", et);  QCOMPARE(et,readers::ExtensionMAT);
    string2enum("hdf", et);  QCOMPARE(et,readers::ExtensionHDF);
    string2enum("hdf4", et); QCOMPARE(et,readers::ExtensionHDF4);
    string2enum("hd4", et);  QCOMPARE(et,readers::ExtensionHDF4);
    string2enum("hdf5", et); QCOMPARE(et,readers::ExtensionHDF5);
    string2enum("hd5", et);  QCOMPARE(et,readers::ExtensionHDF5);
    string2enum("seq", et); QCOMPARE(et,readers::ExtensionSEQ);

    QVERIFY_EXCEPTION_THROWN({string2enum("xyz",et);},kipl::base::KiplException);

    QCOMPARE(enum2string(readers::ExtensionTXT),std::string("txt"));
    QCOMPARE(enum2string(readers::ExtensionDMP),std::string("dmp"));
    QCOMPARE(enum2string(readers::ExtensionDAT),std::string("dat"));
    QCOMPARE(enum2string(readers::ExtensionXML),std::string("xml"));
    QCOMPARE(enum2string(readers::ExtensionRAW),std::string("raw"));
    QCOMPARE(enum2string(readers::ExtensionFITS),std::string("fit"));
    QCOMPARE(enum2string(readers::ExtensionPNG),std::string("png"));
    QCOMPARE(enum2string(readers::ExtensionJPG),std::string("jpg"));
    QCOMPARE(enum2string(readers::ExtensionTIFF),std::string("tif"));
    QCOMPARE(enum2string(readers::ExtensionMAT),std::string("mat"));
    QCOMPARE(enum2string(readers::ExtensionHDF),std::string("hdf"));
    QCOMPARE(enum2string(readers::ExtensionHDF4),std::string("hd4"));
    QCOMPARE(enum2string(readers::ExtensionHDF5),std::string("hd5"));
    QCOMPARE(enum2string(readers::ExtensionSEQ),std::string("seq"));

    QVERIFY_EXCEPTION_THROWN({enum2string(static_cast<readers::eExtensionTypes>(999));},kipl::base::KiplException);

    QCOMPARE(readers::GetFileExtensionType("test.fits"),readers::ExtensionFITS);
    QCOMPARE(readers::GetFileExtensionType("test000.tif"),readers::ExtensionTIFF);
    QCOMPARE(readers::GetFileExtensionType("test.tif.fits"),readers::ExtensionFITS);

    std::ostringstream msg;

    msg<<readers::ExtensionDAT<<","<<readers::ExtensionFITS;
    QCOMPARE(msg.str(),std::string("dat,fit"));
}

void TReaderConfigTest::testDataSetBase()
{
    FileSet l1,l2;

    QCOMPARE(l1.m_nFirst,0);
    QCOMPARE(l1.m_nLast,99);
    QCOMPARE(l1.m_nStep,1);
    QCOMPARE(l1.m_nStride,1);
    QCOMPARE(l1.m_nRepeat,1);
    QCOMPARE(l1.getId(),0);
    QCOMPARE(l2.getId(),1);
    QCOMPARE(l1.m_sFilemask,std::string("image_####.tif"));
    QCOMPARE(l1.m_sVariableName,std::string("image"));
    QVERIFY(l1.m_nSkipList.empty());

    std::string xml;
    xml="<first>10</first>\n"
        "<last>200</last>\n"
        "<step>11</step>\n"
        "<stride>20</stride>\n"
        "<repeat>5</repeat>\n"
        "<filemask>new_####.tif</filemask>\n"
        "<variablename>new</variablename>\n"
        "<skiplist>1 2 3 4</skiplist>\n";

    l1.ParseXML(xml);
    QCOMPARE(l1.m_nFirst,10);
    QCOMPARE(l1.m_nLast,200);
    QCOMPARE(l1.m_nStep,11);
    QCOMPARE(l1.m_nStride,20);
    QCOMPARE(l1.m_nRepeat,5);

    QCOMPARE(l1.m_sFilemask,std::string("new_####.tif"));
    QCOMPARE(l1.m_sVariableName,std::string("new"));
    QCOMPARE(l1.m_nSkipList.size(),size_t(4));

    std::string xml2=l1.WriteXML(0);

    l2.ParseXML(xml2);
    QCOMPARE(l1.m_nFirst,  l2.m_nFirst);
    QCOMPARE(l1.m_nLast,   l2.m_nLast);
    QCOMPARE(l1.m_nStep,   l2.m_nStep);
    QCOMPARE(l1.m_nStride, l2.m_nStride);
    QCOMPARE(l1.m_nRepeat, l2.m_nRepeat);

    QCOMPARE(l1.m_sFilemask, l2.m_sFilemask);
    QCOMPARE(l1.m_sVariableName, l2.m_sVariableName);
    QCOMPARE(l1.m_nSkipList.size(),l2.m_nSkipList.size());

//    kipl::base::TImage<float,2> Load(int idx, kipl::base::eImageRotate rot, kipl::base::eImageFlip flip, size_t *crop=nullptr);
//    kipl::base::TImage<float,3> Load(kipl::base::eImageRotate rot,kipl::base::eImageFlip flip, size_t *crop=nullptr);

}

QTEST_APPLESS_MAIN(TReaderConfigTest)

#include "tst_treaderconfigtest.moc"

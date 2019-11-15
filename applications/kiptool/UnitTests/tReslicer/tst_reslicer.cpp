#include <QtTest>
#include <fstream>
#include <sstream>

#include <base/KiplException.h>

#include "../../src/Reslicer.h"

// add necessary includes here

class TestReslicer : public QObject
{
    Q_OBJECT

public:
    TestReslicer();
    ~TestReslicer();

private slots:
    void test_Process();
    void test_Config();

};

TestReslicer::TestReslicer()
{

}

TestReslicer::~TestReslicer()
{

}

void TestReslicer::test_Process()
{
    TIFFReslicer reslicer;

    QDir dir;
    QString confname=dir.homePath()+"/"+".imagingtools/reslicer.xml" ;
    if (dir.exists(confname)) {
        reslicer.ParseXML(confname.toStdString());
    }
    else {
        QSKIP("Couldn't find par file");
    }

    reslicer.m_bResliceXZ=true;
    reslicer.m_bResliceYZ=true;
    reslicer.process();
}

void TestReslicer::test_Config()
{
    TIFFReslicer reslicer;

    reslicer.m_bResliceXZ=false;
    reslicer.m_bResliceYZ=false;
    reslicer.m_nFirst=123;
    reslicer.m_nLast=345;
    reslicer.m_nFirstXZ=12;
    reslicer.m_nLastXZ=23;
    reslicer.m_nFirstYZ=45;
    reslicer.m_nLastYZ=56;
    reslicer.m_sDestinationMask="res_###.tif";
    reslicer.m_sDestinationPath="/data/hepp/";
    reslicer.m_sSourceMask="/data/hopp/tst_####.tif";
    std::ofstream outfile("reslice_test.xml");
    outfile<<reslicer.WriteXML(0);
    outfile.close();
    TIFFReslicer r2;

    std::ostringstream s;
    try {
        r2.ParseXML("reslice_test.xml");
    } catch (kipl::base::KiplException &e) {
        s<<"Parsing error: "<<e.what();
        QFAIL(s.str().c_str());
    }


    QCOMPARE(r2.m_bResliceXZ,false);
    QCOMPARE(r2.m_bResliceYZ,false);
    QCOMPARE(r2.m_nFirst,123);
    QCOMPARE(r2.m_nLast,345);
    QCOMPARE(r2.m_nFirstXZ,12);
    QCOMPARE(r2.m_nLastXZ,23);
    QCOMPARE(r2.m_nFirstYZ,45);
    QCOMPARE(r2.m_nLastYZ,56);
    QCOMPARE(r2.m_sDestinationPath,std::string("/data/hepp/"));
    QCOMPARE(r2.m_sSourceMask,std::string("/data/hopp/tst_####.tif"));
}

QTEST_APPLESS_MAIN(TestReslicer)

#include "tst_reslicer.moc"

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <io/DirAnalyzer.h>
#include <iostream>

class TDirAnalysisTest : public QObject
{
    Q_OBJECT
    
public:
    TDirAnalysisTest();
    
private Q_SLOTS:
    void testGetFileMask();
};

TDirAnalysisTest::TDirAnalysisTest()
{
}

void TDirAnalysisTest::testGetFileMask()
{
    kipl::io::DirAnalyzer da;
    kipl::io::FileItem fi;
    fi=da.GetFileMask("file_0123.tif");
    QCOMPARE(fi.m_sMask,std::string("file_####.tif"));
    QCOMPARE(fi.m_nIndex,123);
    QCOMPARE(fi.m_sExt,std::string("tif"));

    fi=da.GetFileMask("file0_0231.fits");
    QCOMPARE(fi.m_sMask,std::string("file0_####.fits"));
    QCOMPARE(fi.m_nIndex,231);
    QCOMPARE(fi.m_sExt,std::string("fits"));

    fi=da.GetFileMask("file_abc.tif");
    QCOMPARE(fi.m_sMask,std::string("file_abc.tif"));
    QCOMPARE(fi.m_nIndex,-1);
    QCOMPARE(fi.m_sExt,std::string("tif"));

    fi=da.GetFileMask("file_abc");
    QCOMPARE(fi.m_sMask,std::string("file_abc"));
    QCOMPARE(fi.m_nIndex,-1);
    QCOMPARE(fi.m_sExt,std::string(""));

    fi=da.GetFileMask("file_0001");
    QCOMPARE(fi.m_sMask,std::string("file_####"));
    QCOMPARE(fi.m_nIndex,1);
    QCOMPARE(fi.m_sExt,std::string(""));

    fi=da.GetFileMask("file1_0002");
    QCOMPARE(fi.m_sMask,std::string("file1_####"));
    QCOMPARE(fi.m_nIndex,2);
    QCOMPARE(fi.m_sExt,std::string(""));
}

QTEST_APPLESS_MAIN(TDirAnalysisTest)

#include "tst_tdiranalysistest.moc"

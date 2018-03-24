#include <QtCore/QString>
#include <QtTest/QtTest>

#include <io/DirAnalyzer.h>
#include <strings/filenames.h>

#include <iostream>
#include <fstream>
#include <algorithm>


class TDirAnalysisTest : public QObject
{
    Q_OBJECT
    
public:
    TDirAnalysisTest();
    
private Q_SLOTS:
    void testGetDirList();
    void testGetFileMask();
    void testAnalyzeDirList();
};

TDirAnalysisTest::TDirAnalysisTest()
{
}


void TDirAnalysisTest::testGetDirList()
{
    kipl::io::DirAnalyzer da;
    std::string path="testfiles";
    std::string mask0 = "file_####.fits";

    std::vector<std::string> fl0;

    std::fstream fs;

    std::string fname, fullname;
    std::string ext;

    QDir dir;
    if (!dir.exists(QString::fromStdString(path)))
        dir.mkdir(QString::fromStdString(path));

    for (int i=0; i<10; ++i) {
        kipl::strings::filenames::MakeFileName(mask0,i,fname,ext,'#','0');
        fl0.push_back(fname);
        fullname = path + kipl::strings::filenames::slash + fname;
        fs.open(fullname.c_str(), std::ios::out);
        fs.close();
    }
    std::string mask1 = "elif_###.tif";
    for (int i=4; i<6; ++i) {
        kipl::strings::filenames::MakeFileName(mask1,i,fname,ext,'#','0');
        fl0.push_back(fname);

        fullname = path + kipl::strings::filenames::slash + fname;
        fs.open(fullname.c_str(), std::ios::out);
        fs.close();
    }

    std::vector<std::string> flist=da.GetDirList(path);

    qDebug("%d, %d",(int)flist.size(),(int)fl0.size());
    QVERIFY(flist.size()==fl0.size()+2);
    int cnt=0;
    for (auto it=fl0.begin(); it!=fl0.end(); ++it,++cnt) {
        qDebug("%d",cnt);
        QVERIFY(std::find(flist.begin(),flist.end(),*it)!=flist.end());

    }
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

void TDirAnalysisTest::testAnalyzeDirList() {

    // Basic list
    std::string mask0 = "file_####.fits";

    std::vector<std::string> fl0;

    std::string fname;
    std::string ext;
    // Simple case: only one mask index in sequence
    for (int i=0; i<10; ++i) {
        kipl::strings::filenames::MakeFileName(mask0,i,fname,ext,'#','0');
        fl0.push_back(fname);
    }

    kipl::io::DirAnalyzer da;
    std::map<std::string,std::pair<int,int> > masks0;
    da.AnalyzeDirList(fl0,masks0);

    QVERIFY(masks0.size() == 1);
    QVERIFY(masks0.find(mask0) != masks0.end());
    QVERIFY(masks0[mask0].first == 0 );
    QVERIFY(masks0[mask0].second == 9 );


    // Simple case: two masks index in sequence
    std::string mask1 = "elif_###.tif";
    for (int i=4; i<6; ++i) {
        kipl::strings::filenames::MakeFileName(mask1,i,fname,ext,'#','0');
        fl0.push_back(fname);
    }

    da.AnalyzeDirList(fl0,masks0);

    for (auto it=masks0.begin(); it!=masks0.end(); ++it)
        std::cout<<it->first<<" "<<it->second.first<<" "<<it->second.second<<std::endl;

    QVERIFY(masks0.size() == 2);
    QVERIFY(masks0.find(mask0) != masks0.end());
    QVERIFY(masks0[mask0].first == 0 );
    QVERIFY(masks0[mask0].second == 9 );

    QVERIFY(masks0.find(mask1) != masks0.end());
    QVERIFY(masks0[mask1].first == 4 );
    QVERIFY(masks0[mask1].second == 5 );
}

QTEST_APPLESS_MAIN(TDirAnalysisTest)

#include "tst_tdiranalysistest.moc"

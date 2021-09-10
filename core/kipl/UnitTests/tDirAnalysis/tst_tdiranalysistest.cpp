#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QDebug>

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
    void MakeFiles(std::string mask, int N, int first);
private Q_SLOTS:
    void testGetDirList();
    void testGetFileMask();
    void testAnalyzeDirList();
    void testFileItem();
    void testDirAnalyzer(); // todo Check for redundant parts
    void testCountLinesInFile();

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
  //      qDebug("%d",cnt);
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

    fl0.push_back(".");
    fl0.push_back("..");
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


void TDirAnalysisTest::testFileItem()
{
    kipl::io::FileItem fi;

    QVERIFY(fi.m_nIndex==0);
    QVERIFY(fi.m_sExt=="tif");
    QVERIFY(fi.m_sMask=="noname_####.tif");
    QVERIFY(fi.m_sPath=="");

    kipl::io::FileItem fi2("hepp_####.abc",1,"abc","/home/data/");
    QVERIFY(fi2.m_nIndex==1);
    QVERIFY(fi2.m_sExt=="abc");
    QVERIFY(fi2.m_sMask=="hepp_####.abc");


    fi=fi2;

    QVERIFY(fi2.m_nIndex==fi.m_nIndex);
    QVERIFY(fi2.m_sExt==fi.m_sExt);
    QVERIFY(fi2.m_sMask==fi.m_sMask);
    QVERIFY(fi2.m_sPath==fi.m_sPath);
}

void  TDirAnalysisTest::testDirAnalyzer()
{
    MakeFiles("test_####.fits",5,1);
    MakeFiles("test2_#####.fits",3,6);

    kipl::io::DirAnalyzer di;

    kipl::io::FileItem fi=di.GetFileMask("hepp_1234.xyz");

    QVERIFY(fi.m_nIndex==1234);
    QVERIFY(fi.m_sExt=="xyz");
    QVERIFY(fi.m_sMask=="hepp_####.xyz");
    QVERIFY(fi.m_sPath=="");

    kipl::io::FileItem fi2=di.GetFileMask("hopp2_56780.abc");

    QVERIFY(fi2.m_nIndex==56780);
    QVERIFY(fi2.m_sExt=="abc");
    QVERIFY(fi2.m_sMask=="hopp2_#####.abc");
    QVERIFY(fi2.m_sPath=="");

    kipl::io::FileItem fi3=di.GetFileMask("hopp567.xyz");

    QVERIFY(fi3.m_nIndex==567);
    QVERIFY(fi3.m_sExt=="xyz");
    QVERIFY(fi3.m_sMask=="hopp###.xyz");
    QVERIFY(fi3.m_sPath=="");

    std::string fname="/home/data/hopp_567.xyz";
    kipl::strings::filenames::CheckPathSlashes(fname,false);
    kipl::io::FileItem fi4=di.GetFileMask(fname);

    QVERIFY(fi4.m_nIndex==567);
    QVERIFY(fi4.m_sExt=="xyz");
    qDebug() << fi4.m_sMask.c_str() << " vs. /home/data/hopp_###.xyz";
    std::string ref = "/home/data/hopp_###.xyz";
    kipl::strings::filenames::CheckPathSlashes(ref,false);
    QCOMPARE(fi4.m_sMask,ref);
    std::cout<<fi4.m_sPath<<std::endl;
    ref = "/home/data/";
    kipl::strings::filenames::CheckPathSlashes(ref,false);
    QCOMPARE(fi4.m_sPath,ref);

//    std::string path="\\P08062_wood\\raw_CCD";

    QDir dir;
    std::string path=dir.currentPath().toStdString();
    std::vector<std::string> dirlist=di.GetDirList(path);

    int cnt,first,last;




    QStringList qdirlist=dir.entryList(QDir::AllEntries | QDir::Hidden);
    std::sort(qdirlist.begin(),qdirlist.end());
    std::ostringstream msg;
    msg<<"di size="<<dirlist.size()<<", qdir size="<<qdirlist.size();

//    for (auto & item : dirlist)
//        qDebug() <<item.c_str();

//    for (auto & item : qdirlist)
//        qDebug() <<item;
    QVERIFY2(qdirlist.size()==static_cast<int>(dirlist.size()), msg.str().c_str());

    for (auto it=dirlist.begin(); it!=dirlist.end(); ++it)
        QVERIFY(qdirlist.contains(QString::fromStdString(*it)));


    di.AnalyzeMatchingNames("test_####.fits",cnt,first,last);
    QVERIFY(cnt==5);
    QVERIFY(first==1);
    QVERIFY(last==5);

    di.AnalyzeMatchingNames("test2_#####.fits",cnt,first,last);
    QVERIFY(cnt==3);
    QVERIFY(first==6);
    QVERIFY(last==8);
}

void  TDirAnalysisTest::MakeFiles(std::string mask, int N, int first)
{
    QDir dir;
    if (dir.exists("data")==false) {
        dir.mkdir("data");
    }

//    dir.setCurrent("data");

    std::string fname,ext;
    for (int i=0; i<N; ++i) {
        kipl::strings::filenames::MakeFileName(mask,i+first,fname,ext,'#');
        QFile f(QString::fromStdString(fname));
        f.open(QIODevice::WriteOnly);

        f.write(fname.c_str(),fname.size());

        f.close();
    }
//    dir.setCurrent("../");

}

void  TDirAnalysisTest::testCountLinesInFile()
{
    std::string fname="listfile.txt";
    QDir d;
    d.remove(QString::fromStdString(fname));
    std::ofstream outfile(fname.c_str());

    int N=5;
    for (int i=0; i<N; i++)
    {
        outfile<<i<<", text"<<i<<".fits"<<std::endl;
    }

    kipl::io::DirAnalyzer da;

    int cnt=0;
    da.AnalyzeFileList(fname,cnt);

    QVERIFY(N==cnt);
}
QTEST_APPLESS_MAIN(TDirAnalysisTest)

#include "tst_tdiranalysistest.moc"

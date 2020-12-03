#include <QString>
#include <QtTest>

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>

#include <base/KiplException.h>
#include <math/median.h>
#include <analyzefileext.h>
#include <fileset.h>
#include <buildfilelist.h>
#include <imagereader.h>
#include <imagewriter.h>
#include <readerexception.h>
#include <profile/Timer.h>

class TReaderConfigTest : public QObject
{
    Q_OBJECT

public:
    TReaderConfigTest();
    std::vector<float> goldenAngles(int n, int start, float arc);


private Q_SLOTS:
    void testAnalyzeFileExt();
    void testDataSetBase();
    void testBuildFileListAngles();
    void testImageSize();
    void testRead();
    void testCroppedRead();
    void testGetDose();
    void benchmarkReadVolume();
    void benchmarkReadVolumeParallel();


private:
    kipl::base::TImage<float> gradimg;
};

TReaderConfigTest::TReaderConfigTest() :
    gradimg({100,120})
{

    std::iota(gradimg.GetDataPtr(),gradimg.GetDataPtr()+gradimg.Size(),0);

    ImageWriter writer;

    writer.write(gradimg,"testread.tif");
    writer.write(gradimg,"testread.fits");
}

std::vector<float> TReaderConfigTest::goldenAngles(int n, int start, float arc)
{
   std::vector<float> gv;
   float phi=0.5f*(1.0f+sqrt(5.0f));

   for (int i=0; i<n; ++i)
       gv.push_back(static_cast<float>(fmod((i+start)*phi*arc,180)));

   return gv;
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
    QCOMPARE(l1.m_nCount,1);
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
    QCOMPARE(l1.m_nCount,5);

    QCOMPARE(l1.m_sFilemask,std::string("new_####.tif"));
    QCOMPARE(l1.m_sVariableName,std::string("new"));
    QCOMPARE(l1.m_nSkipList.size(),size_t(4));

    std::string xml2=l1.WriteXML(0);

    l2.ParseXML(xml2);
    QCOMPARE(l1.m_nFirst,  l2.m_nFirst);
    QCOMPARE(l1.m_nLast,   l2.m_nLast);
    QCOMPARE(l1.m_nStep,   l2.m_nStep);
    QCOMPARE(l1.m_nStride, l2.m_nStride);
    QCOMPARE(l1.m_nCount, l2.m_nCount);

    QCOMPARE(l1.m_sFilemask, l2.m_sFilemask);
    QCOMPARE(l1.m_sVariableName, l2.m_sVariableName);
    QCOMPARE(l1.m_nSkipList.size(),l2.m_nSkipList.size());

//    kipl::base::TImage<float,2> Load(int idx, kipl::base::eImageRotate rot, kipl::base::eImageFlip flip, size_t *crop=nullptr);
//    kipl::base::TImage<float,3> Load(kipl::base::eImageRotate rot,kipl::base::eImageFlip flip, size_t *crop=nullptr);

}

void TReaderConfigTest::testBuildFileListAngles()
{
    std::vector<FileSet> il;
    FileSet fs;
    fs.m_nFirst=0;
    fs.m_nLast=10;
    il.push_back(fs);

    std::map<float,std::string> list1=BuildProjectionFileList(il, 0, 0, 180);
    QCOMPARE(list1.size(),static_cast<size_t>(fs.m_nLast-fs.m_nFirst+1));
    float angle=0.0f;
    for (auto & item : list1)
    {
        QCOMPARE(item.first,angle);
        angle+=1;
    }

    // Starting from zero index
    std::map<float,std::string> list2=BuildProjectionFileList(il, 1, 0, 180);
    QCOMPARE(list2.size(),static_cast<size_t>(fs.m_nLast-fs.m_nFirst+1));
    auto gv=goldenAngles(11,0,180.0f);
    std::sort(gv.begin(),gv.end());
    auto git=gv.begin();
    for (auto & item : list2)
    {
        QCOMPARE(item.first,*git);
        ++git;
    }

    // Pick sequence from 10 to 20 with start index 0

    std::vector<FileSet> il3;
    FileSet fs3;
    fs3.m_nFirst=10;
    fs3.m_nLast=20;
    il3.push_back(fs3);
    std::map<float,std::string> list4=BuildProjectionFileList(il3, 1, 0, 180);
    QCOMPARE(list4.size(),static_cast<size_t>(fs.m_nLast-fs.m_nFirst+1));
    auto gv2=goldenAngles(11,10,180.0f);
    std::sort(gv2.begin(),gv2.end());
    git=gv2.begin();
    for (auto & item : list4)
    {
        QCOMPARE(item.first,*git);
        ++git;
    }

    // Starting from index 10

    std::vector<FileSet> il2;
    FileSet fs2;
    fs2.m_nFirst=10;
    fs2.m_nLast=20;
    il2.push_back(fs2);
    std::map<float,std::string> list3=BuildProjectionFileList(il2, 1, 10, 180);
    QCOMPARE(list3.size(),static_cast<size_t>(fs.m_nLast-fs.m_nFirst+1));
    git=gv.begin();
    for (auto & item : list3)
    {
        QCOMPARE(item.first,*git);
        ++git;
    }

    // Pick sequence from 20 to 30 with start index 10

    std::vector<FileSet> il4;
    FileSet fs4;
    fs4.m_nFirst=20;
    fs4.m_nLast=30;
    il4.push_back(fs4);
    std::map<float,std::string> list5=BuildProjectionFileList(il4, 1, 10, 180);
    QCOMPARE(list5.size(),static_cast<size_t>(fs4.m_nLast-fs4.m_nFirst+1));

    git=gv2.begin();
    for (auto & item : list5)
    {
        QCOMPARE(item.first,*git);
        ++git;
    }
}

void TReaderConfigTest::testImageSize()
{
    ImageReader reader;

    auto dimstiff = reader.imageSize("testread.tif");
    qDebug() << dimstiff[0] << dimstiff[1] << dimstiff[2];
    QCOMPARE(dimstiff[0],gradimg.Size(0));
    QCOMPARE(dimstiff[1],gradimg.Size(1));
    QCOMPARE(dimstiff[2],1UL);
    QCOMPARE(dimstiff.size(),3UL);

    auto dimsfits = reader.imageSize("testread.fits");
    qDebug() << dimsfits[0] << dimsfits[1] << dimsfits[2];
    QCOMPARE(dimsfits[0],gradimg.Size(0));
    QCOMPARE(dimsfits[1],gradimg.Size(1));
    QCOMPARE(dimsfits[2],1UL);
    QCOMPARE(dimsfits.size(),3UL);


    QVERIFY_EXCEPTION_THROWN(reader.imageSize("dfgdgdfbvxssrgsxdf.fits"),ReaderException);

}

void TReaderConfigTest::testRead()
{

    ImageReader reader;

    auto tmp = reader.Read("testread.tif");

    QCOMPARE(tmp.Size(0),gradimg.Size(0));
    QCOMPARE(tmp.Size(1),gradimg.Size(1));

    for (size_t i=0; i<gradimg.Size(); ++i)
        QCOMPARE(tmp[i],gradimg[i]);

    auto tmpfits = reader.Read("testread.fits");

    QCOMPARE(tmpfits.Size(0),gradimg.Size(0));
    QCOMPARE(tmpfits.Size(1),gradimg.Size(1));

    for (size_t i=0; i<gradimg.Size(); ++i)
        QCOMPARE(tmpfits[i],gradimg[i]);
}

void TReaderConfigTest::testCroppedRead()
{ 
    ImageReader reader;

    std::vector<size_t> roi={20,20,30,30};

    auto tmp = reader.Read("testread.tif", kipl::base::ImageFlipNone, kipl::base::ImageRotateNone,1.0f,roi);

    QCOMPARE(tmp.Size(0),roi[2]-roi[0]);
    QCOMPARE(tmp.Size(1),roi[3]-roi[1]);

    size_t idx=0UL;
    for (size_t  y = roi[1]; y < roi[3]; ++y)
        for (size_t x = roi[0]; x < roi[2]; ++x, ++idx)
            QCOMPARE(tmp[idx],gradimg(x,y));

    std::vector<size_t> roi2={200,20,30,30};
    QVERIFY_EXCEPTION_THROWN({
                                 auto t=reader.Read("testread.tif", kipl::base::ImageFlipNone, kipl::base::ImageRotateNone,1.0f,roi2);
                             },
                             kipl::base::KiplException);

    auto tmpfits = reader.Read("testread.fits", kipl::base::ImageFlipNone, kipl::base::ImageRotateNone,1.0f,roi);

    QCOMPARE(tmpfits.Size(0),roi[2]-roi[0]);
    QCOMPARE(tmpfits.Size(1),roi[3]-roi[1]);

    idx=0UL;
    for (size_t  y = roi[1]; y < roi[3]; ++y)
        for (size_t x = roi[0]; x < roi[2]; ++x, ++idx)
            QCOMPARE(tmpfits[idx],gradimg(x,y));
}

void TReaderConfigTest::testGetDose()
{
    ImageReader reader;
    std::vector<size_t> doseROI={20,20,30,30};
    float dose=reader.projectionDose("testread.tif",doseROI);
    std::vector<float> means(doseROI[3]-doseROI[1],0.0f);

    for (size_t y=doseROI[1]; y<doseROI[3]; ++y)
    {
        for (size_t x=doseROI[0]; x<doseROI[2]; ++x)
            means[y-doseROI[1]] += gradimg(x,y);
        means[y-doseROI[1]]/=doseROI[2]-doseROI[0];
    }
    float refDose = 0.0f;
    kipl::math::median(means,&refDose);

    QCOMPARE(dose,refDose);

}

void TReaderConfigTest::benchmarkReadVolume()
{
    ImageReader reader;

    reader.bThreadedReading = false;

//    std::string fname = "/Users/data/P20180255/00_XCalibration/xcal_#####.fits";
    std::string fname = "D:\\P20180255\\00_XCalibration\\xcal_#####.fits";
    size_t first = 1;
    size_t last  = 10;
    size_t step  = 1;

    kipl::base::TImage<float,3> img;

    kipl::profile::Timer timer;
    timer.Tic();
//    QBENCHMARK
//    {
        img = reader.Read(fname,first,last,step,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0,{});
//    }
        timer.Toc();


    for (size_t i=0; i<img.Size(2); ++i)
    {
        float *pSlice = img.GetLinePtr(0,i);
        double sum = 0.0;
        for (size_t j=0; j<img.Size(0)*img.Size(1); ++j)
            sum+=pSlice[j];
        qDebug() << i<<":"<<sum;
    }

    qDebug() << timer.elapsedTime();
}

void TReaderConfigTest::benchmarkReadVolumeParallel()
{
    ImageReader reader;

    reader.bThreadedReading = true;

    std::string fname = "/Users/data/P20180255/00_XCalibration/xcal_#####.fits";
    size_t first = 1;
    size_t last  = 8;
    size_t step  = 1;

    kipl::base::TImage<float,3> img;

    kipl::profile::Timer timer;
    timer.Tic();
//    QBENCHMARK
//    {
        img = reader.Read(fname,first,last,step,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0,{});
//    }
        timer.Toc();

    qDebug() << timer.elapsedTime();
}



QTEST_APPLESS_MAIN(TReaderConfigTest)

#include "tst_treaderconfigtest.moc"

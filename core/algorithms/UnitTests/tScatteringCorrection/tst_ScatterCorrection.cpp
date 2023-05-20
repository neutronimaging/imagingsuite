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
#include <SegmentBB.h>


class TestScatterCorrection : public QObject
{
    Q_OBJECT
    
public:
    TestScatterCorrection();
    
private Q_SLOTS:
    void SegmentBB_initialization();

    void SegmentBB_enums();

    void SegmentBB_segmentation();

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


void TestScatterCorrection::SegmentBB_initialization()
{
    QSKIP("Not implemented");
}

void TestScatterCorrection::SegmentBB_enums()
{
    QSKIP("Not implemented");
}

void TestScatterCorrection::SegmentBB_segmentation()
{
    SegmentBB bb_seg;
    kipl::base::TImage<float,2> img;
    
    std::string fname = dataPath+"2D/fits/BB/bbob_00001.fits";
    //std::string fname = dataPath+"2D/fits/BB/bbsample_00001.fits";
    kipl::strings::filenames::CheckPathSlashes(fname,false);

    kipl::io::ReadFITS(img,fname);

    kipl::base::TImage<float,2> res(img.dims()); 
    bb_seg.exec(img,res,{200UL,300UL,1900UL,1900UL});

    kipl::io::WriteTIFF(res,"res.tif",kipl::base::Float32);
}

QTEST_APPLESS_MAIN(TestScatterCorrection)

#include "tst_ScatterCorrection.moc"

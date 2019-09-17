#include <QtTest>

// add necessary includes here
#include <sstream>

#include <base/timage.h>
#include <base/KiplException.h>
#include <base/kiplenums.h>
#include <morphology/morphology.h>
#include <morphology/label.h>

#include <io/io_tiff.h>

class kiplmorphalgorithms : public QObject
{
    Q_OBJECT

public:
    kiplmorphalgorithms();
    ~kiplmorphalgorithms();

private slots:
    void test_LabelImage();
    void test_RemoveConnectedRegion();
    void test_LabelledItemsInfo();

};

kiplmorphalgorithms::kiplmorphalgorithms()
{

}

kiplmorphalgorithms::~kiplmorphalgorithms()
{

}

void kiplmorphalgorithms::test_LabelImage()
{
    kipl::base::TImage<float,2> img;

    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");

    kipl::base::TImage<int,2> result;
    size_t lblCnt=0;
    lblCnt=kipl::morphology::LabelImage(img,result,kipl::base::conn4);

    QCOMPARE(img.Size(),result.Size());
    QCOMPARE(lblCnt,24);

    kipl::io::WriteTIFF(result,"lblresult.tif");

}

void kiplmorphalgorithms::test_RemoveConnectedRegion()
{
    kipl::base::TImage<float,2> img;

    kipl::io::ReadTIFF(img,"../../imagingsuite/core/kipl/UnitTests/data/bilevel_ws.tif");

    kipl::base::TImage<int,2> result;
    size_t lblCnt=0;

    lblCnt=kipl::morphology::LabelImage(img,result,kipl::base::conn4);

    QCOMPARE(img.Size(),result.Size());
    QCOMPARE(lblCnt,24UL);

    std::vector< pair<size_t,size_t>> arealist;
    kipl::morphology::LabelArea(result,lblCnt,arealist);
    QCOMPARE(arealist.size(),lblCnt+1UL);

    int res=kipl::morphology::RemoveConnectedRegion(result, {},kipl::base::conn4);

    QCOMPARE(res,0);

    kipl::morphology::RemoveConnectedRegion(result, {arealist[10].second,arealist[20].second},kipl::base::conn4);

    kipl::io::WriteTIFF(result,"removelbl.tif");
}

void kiplmorphalgorithms::test_LabelledItemsInfo()
{
    QSKIP("Test is not implemented");
}

QTEST_APPLESS_MAIN(kiplmorphalgorithms)

#include "tst_kiplmorphalgorithms.moc"

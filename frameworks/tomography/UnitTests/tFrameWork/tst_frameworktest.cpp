#include <QString>
#include <QTextStream>
#include <QtTest>

#include <thread>
#include <chrono>

#include <base/timage.h>
#include <base/trotate.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <ProjectionReader.h>
#include <ReconHelpers.h>
#include <ReconException.h>
#include <processtiminglogger.h>


class FrameWorkTest : public QObject
{
    Q_OBJECT

public:
    FrameWorkTest();
    std::vector<float> goldenAngles(int n, int start, float arc);
    std::vector<float> invGoldenAngles(int n, int start, float arc);

private slots:
    void testProjectionReader();
    void testBuildFileList_GeneratedSequence();
    void testBuildFileList_GeneratedGolden();
    void testBuildFileList_GeneratedGolden_offset();
    void testBuildFileList_GeneratedGolden_delay();
    void testBuildFileList_GeneratedInvGolden();
    void testBuildFileList_GeneratedInvGolden_delay();
    void testBuildFileList();
    void testBuildFileList_SequenceDropProjections();
    void testBuildFileList_SequenceSkipProjections();
    void testBuildFileList_skipGolden();
    void testProcessTimingLogger();


private:
    kipl::base::TImage<unsigned short,2> m_img;
    kipl::base::TImage<float,2> m_fimg;
};

FrameWorkTest::FrameWorkTest()
{
    std::vector<size_t> dims={15, 20};

    m_img.resize(dims);
    m_fimg.resize(dims);

    for (size_t i=0; i<m_img.Size(); i++) {
        m_img[i]  = static_cast<int>(i);
        m_fimg[i] = static_cast<float>(i);
    }

    kipl::io::WriteFITS(m_img,"proj_0001.fits");
    kipl::io::WriteTIFF(m_img,"proj_0002.tif");
}

std::vector<float> FrameWorkTest::goldenAngles(int n, int start, float arc)
{
   std::vector<float> gv;
   float phi=0.5f*(1.0f+sqrt(5.0f));

   for (int i=0; i<n; ++i)
       gv.push_back(static_cast<float>(fmod((i+start)*phi*arc,180)));

   return gv;
}

std::vector<float> FrameWorkTest::invGoldenAngles(int n, int start, float arc)
{
   std::vector<float> gv;
   float phi=0.5f*(1.0f+sqrt(5.0f))-1;

   for (int i=0; i<n; ++i)
       gv.push_back(static_cast<float>(fmod((i+start)*phi*arc,180)));

   return gv;
}


void FrameWorkTest::testProjectionReader()
{
    QString msg;
    ProjectionReader reader;
    std::vector<size_t> dims;
    kipl::base::TImage<float,2> res_tiff;
    kipl::base::TImage<float,2> res_fits;
    kipl::base::TImage<float,2> ref;

    // ----------------------------------------
    // Basic read
    {
        dims=reader.GetImageSize("proj_0001.fits",1.0f);
        QVERIFY2(dims[0]==m_img.Size(0), "Reading fits size 0 error");
        QVERIFY2(dims[1]==m_img.Size(1), "Reading fits size 1 error");

        dims=reader.GetImageSize("proj_0002.tif",1.0f);
        QVERIFY2(dims[0]==m_img.Size(0), "Reading tiff size 0 error");
        QVERIFY2(dims[1]==m_img.Size(1), "Reading tiff size 1 error");

        // Basic read fits
        res_fits=reader.Read("proj_0001.fits");
        for (size_t i=0; i<m_img.Size(); i++) {
            QVERIFY2(res_fits[i]==m_fimg[i],"Data error in while reading");
        }

        // Basic read tiff
        res_tiff=reader.Read("proj_0002.tif");
        for (size_t i=0; i<m_img.Size(); i++) {
            QVERIFY2(res_tiff[i]==m_fimg[i],"Data error in while reading");
        }
    }
    // ----------------------------------------
    // Crop  reference data
    std::vector<size_t> crop={2,3,7,9};
    kipl::base::TImage<float,2> crop_ref;
    kipl::base::TSubImage<float,2> cropper;


    // ----------------------------------------
    // Cropping
    {
        crop_ref=cropper.Get(m_fimg,crop,false);
        // Cropped fits
        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,crop);

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with crop");
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with crop");

        for (size_t i=0; i<res_fits.Size(); ++i) {

            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        // Cropped tiff
        res_tiff=reader.Read("proj_0002.tif",kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,crop);

        QVERIFY2(res_tiff.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with crop");
        QVERIFY2(res_tiff.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with crop");

        for (size_t i=0; i<res_tiff.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_tiff[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_tiff[i]==crop_ref[i]
                     ,msg.toStdString().c_str());
        }
    }

    // Crop works rotation and mirror will only be tested with fits
    // todo: maybe it still makes sense to test with tiff too...

    // -----------------------------------------
    // Full read with mirrors
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorVertical(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);
        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horvert");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horvert");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;
        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate90,1.0f,{});
        rot_ref=rot.Rotate90(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate180,1.0f,{});

        rot_ref=rot.Rotate180(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate270,1.0f,{});

        rot_ref=rot.Rotate270(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with mirror and rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,{});

        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate180,1.0f,{});
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.Rotate180(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate270,1.0f,{});
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,{});
        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 90");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate180,1.0f,{});
        rot_ref=rot.Rotate180(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 180");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate270,1.0f,{});
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 270");

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            QTextStream(&msg) << "position "<<i<<": read="<<res_fits[i]<<", ref="<<rot_ref[i];
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    std::cout<<"Cropped read w. mirrors"<<std::endl;
    // -----------------------------------------
    // Cropped read with mirrors
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,crop);
        std::cout<<"MirrorHorizontalCrop"<<std::endl;
        rot_ref=rot.MirrorHorizontal(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);

        QTextStream(&msg) << "Size mismatch for fits reading with horizontal w crop read size "<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                          <<", ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());
        QVERIFY(res_fits.Size()==crop_ref.Size());
        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            msg.clear();
            QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,crop);
        std::cout<<"MirrorVerticalCrop"<<std::endl;
        rot_ref=rot.MirrorVertical(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.clear();
        QTextStream(&msg) <<"Size mismatch for fits reading with vertical w crop read size ("<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                          <<"), ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        for (size_t i=0; i<res_fits.Size(); ++i)
        {
            msg.clear();
            QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
            QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,crop);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);
        std::cout<<"MirrorHorizontalVerticalCrop"<<std::endl;

        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.clear();
        QTextStream(&msg) <<"Size mismatch for fits reading with horizontal/vertical w crop read size ("<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                          <<"), ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        QWARN("The horizontal/vertical test is currently excluded");
//        for (size_t i=0; i<res_fits.Size(); ++i) {
//            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
//            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
//        }
    }

    // -----------------------------------------
    // Cropped read with rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        try {
            std::cout<<"Rotate90"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);

            msg.clear();
            QTextStream(&msg) <<"Size mismatch for fits reading with rotate90 w crop read size ("<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                              <<"), ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];

                QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try {
            std::cout<<"Rotate180"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate180,1.0f,crop);

            rot_ref=rot.Rotate180(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);

            msg.clear();
            QTextStream(&msg) <<"Size mismatch for fits reading with rotate180 w crop read size ("<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                              <<"), ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];

                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try {
            std::cout<<"Rotate270"<<std::endl;
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate270,1.0f,crop);
            rot_ref=rot.Rotate270(m_fimg);
            crop_ref=cropper.Get(rot_ref,crop,false);

            msg.clear();
            QTextStream(&msg) <<"Size mismatch for fits reading with rotate270 w crop read size ("<<res_fits.Size(0)<<", "<<res_fits.Size(1)
                              <<"), ref size ("<<crop_ref.Size(0)<<", "<< crop_ref.Size(1)<<")";

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }
    }
    std::cout<<"Cropped rotate"<<std::endl;

    // -----------------------------------------
    // Cropped read with mirror and rotate
    std::cout<<"Cropped read w. mirror and rotate"<<std::endl;
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref, mirror_ref;

        // Horizontal + 90
        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        // Vertical + 90
        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e)
        {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e)
        {
            cout<<"Exception :"<<e.what()<<std::endl;
        }

        try
        {
            res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,crop);

            rot_ref=rot.Rotate90(m_fimg);
            mirror_ref=rot.MirrorHorizontal(rot_ref);
            crop_ref=cropper.Get(mirror_ref,crop,false);

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with horizontal");
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with horizontal");

            for (size_t i=0; i<res_fits.Size(); ++i)
            {
                msg.clear();
                QTextStream(&msg) <<"position "<<i<<": read="<<res_fits[i]<<", ref="<<crop_ref[i];
                QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
            }
        }
        catch (ReconException &e) {
            cout<<"Exception :"<<e.what()<<std::endl;
        }
    }
}



void FrameWorkTest::testBuildFileList_GeneratedSequence()
{
    std::ostringstream msg;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=360.0f;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);
    msg.str(""); msg<<"Expected size "<<config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1,msg.str().c_str());
    float sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); it++) {
        std::cout<<(it->first)<<", "<<(*it).second.name<<", "<<(*it).second.angle<<", "<<(*it).second.weight<<std::endl;
        sum+=(*it).second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;

    BuildFileList(config,ProjectionList);
    msg.str(""); msg<<"Expected size "<<config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1,msg.str().c_str());
    sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); it++) {
        std::cout<<(it->first)<<", "<<(*it).second.name<<", "<<(*it).second.angle<<", "<<(*it).second.weight<<std::endl;
        sum+=(*it).second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden()
{
    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 0;
    config.ProjectionInfo.nLastIndex      = 18;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.0f      , 291.2461f  , 222.49222f , 153.73837f ,  84.98446f ,
                             16.230604f, 307.47675f , 238.72289f , 169.96892f , 101.21517f ,
                             32.46121f , 323.70724f , 254.95349f , 186.19952f , 117.44578f ,
                             48.691597f, 339.93784f , 271.18408f , 202.43034f};

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(config,ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180b={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    std::sort(gv180b.begin(),gv180b.end());
    git=gv180b.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden_offset()
{
    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 10;
    config.ProjectionInfo.nLastIndex      = 28;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 10;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.0f      , 291.2461f  , 222.49222f , 153.73837f ,  84.98446f ,
                             16.230604f, 307.47675f , 238.72289f , 169.96892f , 101.21517f ,
                             32.46121f , 323.70724f , 254.95349f , 186.19952f , 117.44578f ,
                             48.691597f, 339.93784f , 271.18408f , 202.43034f};

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(config,ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180b={ 0.f       , 111.24611797f,  42.49223595f, 153.73835392f,
                            84.9844719f ,  16.23058987f, 127.47670785f,  58.72282582f,
                           169.9689438f , 101.21506177f,  32.46117975f, 143.70729772f,
                            74.9534157f ,   6.19971f, 117.44565165f,  48.69176962f,
                           159.9378876f ,  91.18400557f,  22.43012355f};

    std::sort(gv180b.begin(),gv180b.end());
    git=gv180b.begin();

    for (auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedGolden_delay()
{
   // Starting at index 10
    std::vector<float> gv180={32.46117974981095f,
                              143.70729772479237f,
                              74.95341569977299f,
                              6.19971f,
                              117.44565164973501f,
                              48.69176962471643f,
                              159.93788759969704f,
                              91.18400557467847f,
                              22.43012354965988f,
                              133.6762415246405f,
                              64.9223594996219f,
                              176.16847747460253f,
                              107.41459544958475f,
                              38.6611f,
                              149.90683139954598f,
                              81.15294937452657f,
                              12.3994f,
                              123.64518532449023f,
                              54.89130329947002f,
                              166.13742127445144f};

    std::ostringstream msg;
    size_t N=10;
    size_t i=0;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 10;
    config.ProjectionInfo.nLastIndex      = 29;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (auto &it: ProjectionList) {
        QCOMPARE(it.first,*git);
        ++git;
    }

    //
    config.ProjectionInfo.sFileMask       = "test_####.fits";
    config.ProjectionInfo.nFirstIndex     = 12;
    config.ProjectionInfo.nLastIndex      = 31;
    config.ProjectionInfo.fScanArc[0]     = 0.0f;
    config.ProjectionInfo.fScanArc[1]     = 180.0f;
    config.ProjectionInfo.scantype        = config.ProjectionInfo.GoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx = 2;

    ProjectionList.clear();
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    git=gv180.begin();

    for (auto &it: ProjectionList) {
        QCOMPARE(it.first,*git);
        ++git;
    }
}

void FrameWorkTest::testBuildFileList_GeneratedInvGolden()
{
    std::ostringstream msg;
    size_t N=10;

    ReconConfig config("");
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=0;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.scantype=config.ProjectionInfo.InvGoldenSectionScan;
    config.ProjectionInfo.nGoldenStartIdx=0;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    float sum=0.0f;

    std::vector<float> gv180={  0.f,        111.24612f ,   42.492245f,  153.73833f,    84.98449f,    16.23059f,
            127.47666f,    58.722794f , 169.96898f ,  101.2151f ,    32.46118f,   143.70726f,
             74.95333f,     6.19958f, 117.44559f,    48.691772f , 159.93796f,    91.18403f,
             22.430218f };


    QCOMPARE(ProjectionList.size(),gv180.size());
    std::sort(gv180.begin(),gv180.end());
    auto git=gv180.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    config.ProjectionInfo.fScanArc[1]=360.0f;
    BuildFileList(config,ProjectionList);
    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv360 = {   0.f      ,  111.24612f  , 222.49223f  , 333.7383f  ,   84.98449f ,  196.23059f,
                                  307.47665f  ,  58.722794f , 169.96898f ,  281.2151f  ,   32.46118f  , 143.70726f,
                                  254.95332f ,    6.19958f ,117.44559f ,  228.69177f ,  339.93796f ,   91.18403f,
                                  202.4302f   };

    QCOMPARE(ProjectionList.size(),gv360.size());
    std::sort(gv360.begin(),gv360.end());

    git = gv360.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());


    // Test odd number
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.nGoldenStartIdx = 1;

    N=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    BuildFileList(config,ProjectionList);
    msg.str(""); msg<<"Expected size "<<N<<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==N,msg.str().c_str());
    sum=0.0f;

    std::vector<float> gv180odd={  0.f,        111.24612f ,   42.492245f,  153.73833f,    84.98449f,    16.23059f,
            127.47666f,    58.722794f , 169.96898f ,  101.2151f ,    32.46118f,   143.70726f,
             74.95333f,     6.19958f, 117.44559f,    48.691772f , 159.93796f,    91.18403f,
             22.430218f };
    std::sort(gv180odd.begin(),gv180odd.end());
    git=gv180odd.begin();

    for (const auto &it: ProjectionList)
    {
        sum+=it.second.weight;
        QCOMPARE(it.first,*git);
        ++git;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testBuildFileList_GeneratedInvGolden_delay()
{

}

void FrameWorkTest::testBuildFileList()
{
    // File list
    std::ofstream listfile("listfile.txt");
    std::ostringstream fname,msg;
    size_t N=10;
    size_t i=0;
    for (i=0; i<N; ++i)
    {
        listfile<<18*i<<",file_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;
    }

    listfile<<18*i<<", file_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;

    ReconConfig config("");

    config.ProjectionInfo.sFileMask="listfile.txt";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=i+1;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(config,ProjectionList);

    QVERIFY(ProjectionList.size()==i+1);

    i=0;
    float sum=0.0f;
    for (auto it=ProjectionList.begin(); it!=ProjectionList.end(); ++it,++i)
    {
        fname.str("");
        msg.str("");
        fname<<"file_"<<setfill('0') << setw(5) << i <<".fits";
        msg<<fname.str()<<"!="<<it->second.name;
        QVERIFY2(it->second.name==fname.str(),msg.str().c_str());
        QVERIFY(it->second.angle==static_cast<float>(18*i));
        sum+=it->second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());
}

void FrameWorkTest::testProcessTimingLogger()
{
    ProcessTimingLogger ptl("test.json");

    std::map<std::string,std::map<std::string,std::string>> entryData = {{"data",{{"proj","500"}}},{"time",{{"bp","10"}}}};


    ptl.addLogEntry(entryData);
    std::this_thread::sleep_for(chrono::seconds(1));
    entryData["hardware"]={{"proj","500"}};
    ptl.addLogEntry(entryData);
}


void FrameWorkTest::testBuildFileList_SequenceDropProjections()
{
    std::ostringstream msg;
    ReconConfig config("");

    std::map<float,ProjectionInfo> ProjectionList;
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=21;
    config.ProjectionInfo.skipListMode = ReconConfig::cProjections::SkipMode_Drop;
    config.ProjectionInfo.nlSkipList = {5,9};
    config.ProjectionInfo.fScanArc={0.0f,180.0f};

    BuildFileList(config,ProjectionList);
    size_t expectedSize = config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1-config.ProjectionInfo.nlSkipList.size();
    msg.str(""); msg<<"Expected size "
        <<expectedSize
        <<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==expectedSize,msg.str().c_str());
    float sum=0.0f;
    for (const auto & item: ProjectionList) 
    {
        std::cout<<(item.first)<<", "
                <<item.second.name<<", "
                <<item.second.angle<<", "
                <<item.second.weight<<std::endl;
        sum+=item.second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    
}

void FrameWorkTest::testBuildFileList_SequenceSkipProjections()
{
    std::ostringstream msg;
    ReconConfig config("");

    std::map<float,ProjectionInfo> ProjectionList;
    ProjectionList.clear();
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=19;
    config.ProjectionInfo.skipListMode = ReconConfig::cProjections::SkipMode_Skip;
    config.ProjectionInfo.nlSkipList = {5,9};
    config.ProjectionInfo.fScanArc={0.0f,180.0f};


    BuildFileList(config,ProjectionList);
    size_t expectedSize = config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1-config.ProjectionInfo.nlSkipList.size();
    msg.str(""); msg<<"Expected size "
        <<expectedSize
        <<", got "<<ProjectionList.size();
    QVERIFY2(ProjectionList.size()==expectedSize,msg.str().c_str());
    float sum=0.0f;
    for (const auto & item: ProjectionList) 
    {
        std::cout<<(item.first)<<", "
                <<item.second.name<<", "
                <<item.second.angle<<", "
                <<item.second.weight<<std::endl;
        sum+=item.second.weight;
    }

    msg.str(""); msg<<"Expected 1.0, got "<<sum;
    QVERIFY2(qFuzzyCompare(sum,1.0f),msg.str().c_str());

    
}

void FrameWorkTest::testBuildFileList_skipGolden()
{

}

QTEST_APPLESS_MAIN(FrameWorkTest)

#include "tst_frameworktest.moc"

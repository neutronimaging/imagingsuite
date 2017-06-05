#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/trotate.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <ProjectionReader.h>
#include <ReconHelpers.h>
#include <ReconException.h>


class FrameWorkTest : public QObject
{
    Q_OBJECT

public:
    FrameWorkTest();

private Q_SLOTS:
    void testProjectionReader();
    void testBuildFileList_GeneratedSequence();
    void testBuildFileList_GeneratedGolden();
    void testBuildFileList();
    void testBuildFileList2();

private:
    kipl::base::TImage<unsigned short,2> m_img;
    kipl::base::TImage<float,2> m_fimg;
};

FrameWorkTest::FrameWorkTest()
{
    size_t dims[2]={15, 20};

    m_img.Resize(dims);
    m_fimg.Resize(dims);

    for (size_t i=0; i<m_img.Size(); i++) {
        m_img[i]  = i;
        m_fimg[i] = i;
    }

    kipl::io::WriteFITS(m_img,"proj_0001.fits");
    kipl::io::WriteTIFF(m_img,"proj_0002.tif");
}

void FrameWorkTest::testProjectionReader()
{
    QString msg;
    ProjectionReader reader;
    size_t dims[2];
    kipl::base::TImage<float,2> res_tiff;
    kipl::base::TImage<float,2> res_fits;
    kipl::base::TImage<float,2> ref;

    // ----------------------------------------
    // Basic read
    {
    reader.GetImageSize("proj_0001.fits",1.0f,dims);
    QVERIFY2(dims[0]==m_img.Size(0), "Reading fits size 0 error");
    QVERIFY2(dims[1]==m_img.Size(1), "Reading fits size 1 error");

    reader.GetImageSize("proj_0002.tif",1.0f,dims);
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
    size_t crop[4]={2,3,7,9};
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
            msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        // Cropped tiff
        res_tiff=reader.Read("proj_0002.tif",kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0f,crop);

        QVERIFY2(res_tiff.Size(0)==crop_ref.Size(0),"Size mismatch for fits reading with crop");
        QVERIFY2(res_tiff.Size(1)==crop_ref.Size(1),"Size mismatch for fits reading with crop");

        for (size_t i=0; i<res_tiff.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_tiff[i],crop_ref[i]);
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

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,nullptr);
        rot_ref=rot.MirrorHorizontal(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,nullptr);
        rot_ref=rot.MirrorVertical(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i], rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,nullptr);
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);
        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horvert");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horvert");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;
        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate90,1.0f,nullptr);
        rot_ref=rot.Rotate90(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 90");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate180,1.0f,nullptr);

        rot_ref=rot.Rotate180(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 180");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i], rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipNone,kipl::base::ImageRotate270,1.0f,nullptr);

        rot_ref=rot.Rotate270(m_fimg);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with rotate 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with rotate 270");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i], rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Full read with mirror and rotate
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate90,1.0f,nullptr);

        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r90");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate180,1.0f,nullptr);
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.Rotate180(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r180");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotate270,1.0f,nullptr);
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r270");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,nullptr);
        rot_ref=rot.Rotate90(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 90");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate180,1.0f,nullptr);
        rot_ref=rot.Rotate180(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 180");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate270,1.0f,nullptr);
        rot_ref=rot.Rotate270(m_fimg);
        rot_ref=rot.MirrorVertical(rot_ref)
                ;

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 270");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
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
        msg.sprintf("Size mismatch for fits reading with horizontal w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());
        QVERIFY(res_fits.Size()==crop_ref.Size());
        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,crop);
        std::cout<<"MirrorVerticalCrop"<<std::endl;
        rot_ref=rot.MirrorVertical(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.sprintf("Size mismatch for fits reading with vertical w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,crop);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.MirrorHorizontal(rot_ref);
        std::cout<<"MirrorHorizontalVerticalCrop"<<std::endl;

        crop_ref=cropper.Get(rot_ref,crop,false);

        msg.sprintf("Size mismatch for fits reading with horizontal/vertical w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

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

            msg.sprintf("Size mismatch for fits reading with rotate90 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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
            msg.sprintf("Size mismatch for fits reading with rotate180 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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
            msg.sprintf("Size mismatch for fits reading with rotate270 w crop read size (%zu, %zu), ref size (%zu, %zu)",
                        res_fits.Size(0),res_fits.Size(1),
                        crop_ref.Size(0), crop_ref.Size(1));

            QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
            QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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

            for (size_t i=0; i<res_fits.Size(); ++i) {
                msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
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
    size_t N=10;
    size_t i=0;

    ReconConfig config;
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=360.0f;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
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

    BuildFileList(&config,&ProjectionList);
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

    ReconConfig config;
// Test even number
    config.ProjectionInfo.sFileMask="test_####.fits";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=18;
    config.ProjectionInfo.fScanArc[0]=0.0f;
    config.ProjectionInfo.fScanArc[1]=180.0f;
    config.ProjectionInfo.scantype=config.ProjectionInfo.GoldenSectionScan;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);
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

    BuildFileList(&config,&ProjectionList);
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
//    ++i;listfile<<18*i<<"\tfile_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;
//    ++i;listfile<<18*i<<"\t  file_"<<setfill('0') << setw(5) << i <<".fits"<<std::endl;
    //++i;listfile<<"file_"<<setfill('0') << setw(5) << i <<".fits,"<<18*i<<std::endl;

    ReconConfig config;

    config.ProjectionInfo.sFileMask="listfile.txt";
    config.ProjectionInfo.nFirstIndex=1;
    config.ProjectionInfo.nLastIndex=i+1;

    std::map<float,ProjectionInfo> ProjectionList;
    BuildFileList(&config,&ProjectionList);

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

void FrameWorkTest::testBuildFileList2()
{
//    size_t N=50;
//    ReconConfig config;

//    config.ProjectionInfo.sFileMask="ct2.csv";
//    config.ProjectionInfo.nFirstIndex=1;
//    config.ProjectionInfo.nLastIndex=N;

//    std::map<float,ProjectionInfo> ProjectionList;
//    BuildFileList(&config,&ProjectionList,'\r');

//    QVERIFY(ProjectionList.size()==N);

}
QTEST_APPLESS_MAIN(FrameWorkTest)

#include "tst_frameworktest.moc"

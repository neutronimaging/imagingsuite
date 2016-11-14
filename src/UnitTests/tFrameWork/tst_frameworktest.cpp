#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <base/trotate.h>
#include <base/tsubimage.h>
#include <io/io_fits.h>
#include <io/io_tiff.h>

#include <ProjectionReader.h>


class FrameWorkTest : public QObject
{
    Q_OBJECT

public:
    FrameWorkTest();

private Q_SLOTS:
    void testProjectionReader();

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
    size_t cdims[2]={5,6};

    kipl::base::TImage<float,2> crop_ref(cdims);

    for (size_t y=0; y<crop_ref.Size(1); ++y) {
        memcpy(crop_ref.GetLinePtr(y),m_fimg.GetLinePtr(y+crop[1])+crop[0],sizeof(float)*(crop[2]-crop[0]));
    }


    // ----------------------------------------
    // Cropping
    {
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
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.Rotate90(rot_ref);

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
        rot_ref=rot.MirrorHorizontal(m_fimg);
        rot_ref=rot.Rotate270(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with horizontal r270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with horizontal r270");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate90,1.0f,nullptr);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.Rotate90(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 90");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 90");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate180,1.0f,nullptr);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.Rotate180(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 180");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 180");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotate270,1.0f,nullptr);
        rot_ref=rot.MirrorVertical(m_fimg);
        rot_ref=rot.Rotate270(rot_ref);

        QVERIFY2(res_fits.Size(0)==rot_ref.Size(0),"Size mismatch for fits reading with vertical 270");
        QVERIFY2(res_fits.Size(1)==rot_ref.Size(1),"Size mismatch for fits reading with vertical 270");

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],rot_ref[i]);
            QVERIFY2(res_fits[i]==rot_ref[i],msg.toStdString().c_str());
        }
    }

    // -----------------------------------------
    // Cropped read with mirrors
    {
        kipl::base::TRotate<float> rot;
        kipl::base::TImage<float,2> rot_ref;
        kipl::base::TSubImage<float,2> cropper;

        crop_ref.Resize(cdims);

        res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,crop);
        rot_ref=rot.MirrorHorizontal(m_fimg);
        crop_ref=cropper.Get(rot_ref,crop,false);
        msg.sprintf("Size mismatch for fits reading with horizontal w crop read size (%zu, %zu), ref size (%zu, %zu)",
                    res_fits.Size(0),res_fits.Size(1),
                    crop_ref.Size(0), crop_ref.Size(1));

        QVERIFY2(res_fits.Size(0)==crop_ref.Size(0),msg.toStdString().c_str());
        QVERIFY2(res_fits.Size(1)==crop_ref.Size(1),msg.toStdString().c_str());

        for (size_t i=0; i<res_fits.Size(); ++i) {
            msg.sprintf("position %zu: read=%f, ref=%f", i, res_fits[i],crop_ref[i]);
            QVERIFY2(res_fits[i]==crop_ref[i],msg.toStdString().c_str());
        }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with vertical");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with vertical");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with horvert");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with horvert");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }
    }

    // -----------------------------------------
    // Cropped read with rotate
    {
    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with horizontal");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with horizontal");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with vertical");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with vertical");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with horvert");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with horvert");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }
    }

    // -----------------------------------------
    // Cropped read with mirror and rotate
    {
    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontal,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with horizontal");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with horizontal");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with vertical");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with vertical");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }

    res_fits=reader.Read("proj_0001.fits",kipl::base::ImageFlipHorizontalVertical,kipl::base::ImageRotateNone,1.0f,nullptr);

    QVERIFY2(res_fits.Size(0)==m_img.Size(0),"Size mismatch for fits reading with horvert");
    QVERIFY2(res_fits.Size(1)==m_img.Size(1),"Size mismatch for fits reading with horvert");

    for (size_t i=0; i<res_fits.Size(); ++i) {
        msg.sprintf("position %zu: read=%f, ref=%d", i, res_fits[i],crop_ref[i]);
        QVERIFY2(res_fits[i]==static_cast<float>(crop_ref[i]),msg.toStdString().c_str());
    }
    }
}

QTEST_APPLESS_MAIN(FrameWorkTest)

#include "tst_frameworktest.moc"

#include "imgviewerprofiledialog.h"
#include "ui_imgviewerprofiledialog.h"

#include <base/tsubimage.h>

ImgViewerProfileDialog::ImgViewerProfileDialog(QWidget *parent) :
    QDialog(parent),
    logger("ImgViewerProfileDialog"),
    ui(new Ui::ImgViewerProfileDialog)
{
    ui->setupUi(this);
}

ImgViewerProfileDialog::~ImgViewerProfileDialog()
{
    delete ui;
}

void ImgViewerProfileDialog::setImage(kipl::base::TImage<float,2> &img)
{
    currentImage.Clone(img);
}

void ImgViewerProfileDialog::setROI(QRect &roi)
{
    currentROI = roi;
    std::vector<size_t> nroi = {static_cast<size_t>(roi.x()),
                                static_cast<size_t>(roi.y()),
                                static_cast<size_t>(roi.x()+roi.width()),
                                static_cast<size_t>(roi.y()+roi.height())};

    auto roiImage = kipl::base::TSubImage<float,2>::Get(currentImage,nroi);


}

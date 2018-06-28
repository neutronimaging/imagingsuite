#include "pixelsizedlg.h"
#include "ui_pixelsizedlg.h"

PixelSizeDlg::PixelSizeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PixelSizeDlg)
{
    ui->setupUi(this);
}

PixelSizeDlg::~PixelSizeDlg()
{
    delete ui;
}

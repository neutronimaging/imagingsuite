#ifndef PIXELSIZEDLG_H
#define PIXELSIZEDLG_H

#include <QDialog>

namespace Ui {
class PixelSizeDlg;
}

class PixelSizeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PixelSizeDlg(QWidget *parent = 0);
    ~PixelSizeDlg();

private:
    Ui::PixelSizeDlg *ui;
};

#endif // PIXELSIZEDLG_H

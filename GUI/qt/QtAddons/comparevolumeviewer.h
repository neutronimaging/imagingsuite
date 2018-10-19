#ifndef COMPAREVOLUMEVIEWER_H
#define COMPAREVOLUMEVIEWER_H

#include <QWidget>

#include <base/timage.h>

namespace Ui {
class CompareVolumeViewer;
}
namespace QtAddons {
class CompareVolumeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CompareVolumeViewer(QWidget *parent = nullptr);
    ~CompareVolumeViewer();

    void setImages(kipl::base::TImage<float,3> *pOrig, kipl::base::TImage<float,3> *pProc);
    void setImageLabel(QString lbl,int idx);
    void showDifference(bool show);

private slots:
    void on_checkBox_linkIntensity_toggled(bool checked);

    void on_horizontalSlider_slices_sliderMoved(int position);

    void on_spinBox_slices_valueChanged(int arg1);

    void on_comboBox_imagePlanes_currentIndexChanged(int index);

private:
    Ui::CompareVolumeViewer *ui;

    kipl::base::TImage<float,3> *pOriginal;
    kipl::base::TImage<float,3> *pProcessed;
    bool bShowDifference;
    void updateViews(int idx);
    void updateBounds();
};

}
#endif // COMPAREVOLUMEVIEWER_H

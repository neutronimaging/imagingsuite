#ifndef MERGEVOLUMESDIALOG_H
#define MERGEVOLUMESDIALOG_H

#include <QDialog>

#include <base/timage.h>
#include <logging/logger.h>

#include "mergevolume.h"


namespace Ui {
class MergeVolumesDialog;
}

class MergeVolumesDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit MergeVolumesDialog(QWidget *parent = nullptr);
    ~MergeVolumesDialog();

private slots:
    void on_pushButton_browseA_clicked();

    void on_pushButton_browseB_clicked();

    void on_pushButton_loaddata_clicked();

    void on_pushButton_loadA_clicked();

    void on_pushButton_loadB_clicked();

    void on_comboBox_mixorder_currentIndexChanged(int index);

    void on_pushButton_browseout_clicked();

    void on_pushButton_startmerge_clicked();

    void on_comboBox_result_currentIndexChanged(int index);

    void on_pushButton_TestMix_clicked();

    void on_spinBox_firstA_valueChanged(int arg1);

    void on_spinBox_lastA_valueChanged(int arg1);

    void on_spinBox_firstB_valueChanged(int arg1);

    void on_spinBox_lastB_valueChanged(int arg1);

protected:
    void UpdateDialog();
    void UpdateConfig();
    void SaveConfig();
    void LoadConfig();

    void LoadVerticalSlice(std::string filemask,
                                        int first,
                                        int last,
                                        kipl::base::TImage<float,2> *img);

    MergeVolume m_merger;
    kipl::base::TImage<float,2> m_VerticalImgA;
    kipl::base::TImage<float,2> m_VerticalImgB;
    kipl::base::TImage<float,2> m_VerticalImgResult;
    kipl::base::TImage<float,2> m_VerticalImgLocalResult;
    kipl::base::TImage<float,2> m_HorizontalSliceResult;

private:
    Ui::MergeVolumesDialog *ui;
};

#endif // MERGEVOLUMESDIALOG_H

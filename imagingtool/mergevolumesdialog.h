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
    explicit MergeVolumesDialog(QWidget *parent = 0);
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

    void on_checkBox_crop_toggled(bool checked);

protected:
    void UpdateDialog();
    void UpdateConfig();
    void LoadVerticalSlice(std::string filemask,
                                        int first,
                                        int last,
                                        kipl::base::TImage<float,2> *img);

    MergeVolume m_merger;
    kipl::base::TImage<float,2> m_VerticalImgA;
    kipl::base::TImage<float,2> m_VerticalImgB;
    kipl::base::TImage<float,2> m_imgResult;
private:
    Ui::MergeVolumesDialog *ui;
};

#endif // MERGEVOLUMESDIALOG_H

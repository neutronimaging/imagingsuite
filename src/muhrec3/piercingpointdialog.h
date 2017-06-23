#ifndef PIERCINGPOINTDIALOG_H
#define PIERCINGPOINTDIALOG_H

#include <QDialog>
#include <ReconConfig.h>
#include <logging/logger.h>

namespace Ui {
class PiercingPointDialog;
}

class PiercingPointDialog : public QDialog
{
    Q_OBJECT

    kipl::logging::Logger logger;
public:
    explicit PiercingPointDialog(QWidget *parent = 0);
    ~PiercingPointDialog();

    int exec(ReconConfig &config);

    pair<float,float> getPosition() { return position; }

private slots:
    void on_checkBox_useROI_toggled(bool checked);

    void on_pushButton_getROI_clicked();

    void on_pushButton_estimate_clicked();

private:
    void UpdateDialog();
    void UpdateConfig();

    Ui::PiercingPointDialog *ui;


    size_t roi[4];
    bool correctGain;
    bool useROI;
    pair<float,float> position;
    kipl::base::TImage<float,2> ob;
    kipl::base::TImage<float,2> dc;
};

#endif // PIERCINGPOINTDIALOG_H

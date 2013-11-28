#ifndef KIPTOOLMAINWINDOW_H
#define KIPTOOLMAINWINDOW_H

#include <QMainWindow>
#include <logging/logger.h>
#include <KiplProcessConfig.h>

namespace Ui {
class KipToolMainWindow;
}

class KipToolMainWindow : public QMainWindow
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit KipToolMainWindow(QWidget *parent = 0);
    ~KipToolMainWindow();
    
private slots:
    void on_button_browsedatapath_clicked();

    void on_button_getROI_clicked();

    void on_button_loaddata_clicked();

    void on_button_browsedestination_clicked();

    void on_check_crop_stateChanged(int arg1);

    void on_button_savedata_clicked();

    void on_check_showorighist_stateChanged(int arg1);

    void on_combo_plotselector_currentIndexChanged(int index);

    void on_slider_images_sliderMoved(int position);

private:
    void UpdateDialog();
    void UpdateConfig();
    void SetupCallbacks();
    Ui::KipToolMainWindow *ui;

    KiplProcessConfig m_config;
};

#endif // KIPTOOLMAINWINDOW_H

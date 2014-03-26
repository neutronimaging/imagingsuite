#ifndef IMAGINGTOOLMAIN_H
#define IMAGINGTOOLMAIN_H

#include <QMainWindow>
#include "ImagingToolConfig.h"
#include <logging/logger.h>


namespace Ui {
class ImagingToolMain;
}

class ImagingToolMain : public QMainWindow
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit ImagingToolMain(QWidget *parent = 0);
    ~ImagingToolMain();
    
protected slots:
    void f2t_BrowseSrcPath();
    void f2t_Preview();
    void f2t_FindProjections();
    void f2t_GetROI();
    void f2t_TakePath();
    void f2t_BrowseDestPath();
    void f2t_Convert();

private slots:
    void on_button_ResliceBrowseInPath_clicked();

    void on_reslice_button_BrowseInPath_clicked();

    void on_reslice_button_BrowseOutPath_clicked();

    void on_reslice_button_process_clicked();

private:
    Ui::ImagingToolMain *ui;
    ImagingToolConfig m_config;

    void UpdateDialog();
    void UpdateConfig();
    void SaveConfig();
    void LoadConfig();
};

#endif // IMAGINGTOOLMAIN_H

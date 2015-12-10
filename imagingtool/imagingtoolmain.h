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

private slots:
    void on_actionMerge_volume_triggered();

    void on_actionReslice_triggered();

    void on_actionConvertFiles_triggered();

    void on_actionGeneric_to_TIFF_triggered();

private:
    Ui::ImagingToolMain *ui;
    ImagingToolConfig m_config;

    void UpdateDialog();
    void UpdateConfig();
    void SaveConfig();
    void LoadConfig();
};

#endif // IMAGINGTOOLMAIN_H

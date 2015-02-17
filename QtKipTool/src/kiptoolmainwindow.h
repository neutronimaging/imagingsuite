#ifndef KIPTOOLMAINWINDOW_H
#define KIPTOOLMAINWINDOW_H

#include <QMainWindow>
#include <logging/logger.h>
#include <KiplProcessConfig.h>
#include <KiplEngine.h>
#include <KiplFactory.h>

#include <list>

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
    
protected slots:
    void on_button_browsedatapath_clicked();
    void on_button_getROI_clicked();
    void on_button_loaddata_clicked();
    void on_button_browsedestination_clicked();
    void on_check_crop_stateChanged(int arg1);
    void on_button_savedata_clicked();
    void on_combo_plotselector_currentIndexChanged(int index);
    void on_slider_images_sliderMoved(int position);

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionQuit_triggered();
    void on_actionStart_processing_triggered();
    void on_actionProcessing_history_triggered();
    void on_actionClear_History_triggered();
    void on_actionAbout_triggered();

private slots:
    void on_combo_sliceplane_activated(int index);

    void on_check_linkviewers_toggled(bool checked);

    void on_tabWidget_plots_currentChanged(int index);

    void on_slider_hprofile_sliderMoved(int position);

    void on_slider_vprofile_sliderMoved(int position);

private:
    void UpdateDialog();
    void UpdateConfig();
    void SetupCallbacks();
    void UpdateMatrixROI();
    void LoadDefaults();
    void UpdateHistogramView();
    void UpdatePlotView();
    void SaveConfiguration(QString qfname);
    void LoadConfiguration(QString qfname);

    Ui::KipToolMainWindow *ui;
    KiplEngine *m_Engine;
    KiplFactory m_Factory;

    std::map<std::string, std::map<std::string, kipl::containers::PlotData<float,float> > > m_PlotList;
    std::map<std::string, kipl::containers::PlotData<float,size_t> > m_HistogramList;
    kipl::containers::PlotData<float,size_t> m_OriginalHistogram;


    QString m_sFileName;
    KiplProcessConfig m_config;
    kipl::base::TImage<float,3> m_OriginalImage;
    bool m_bRescaleViewers;
    bool m_bJustLoaded;
    kipl::base::eImagePlanes m_eSlicePlane;
    kipl::base::TImage<float,2> m_SliceOriginal;
    kipl::base::TImage<float,2> m_SliceResult;
    int m_nSliceSizeX;
    int m_nSliceSizeY;

    std::list<std::pair<KiplProcessConfig, kipl::base::TImage<float,2> > >  m_configHistory;
};

#endif // KIPTOOLMAINWINDOW_H

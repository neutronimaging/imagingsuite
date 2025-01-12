//<LICENSE>
#ifndef MUHRECMAINWINDOW_H
#define MUHRECMAINWINDOW_H

#include <QMainWindow>

#include <list>

#include "MuhrecInteractor.h"
#include "PreProcModuleConfigurator.h"
#include <ApplicationBase.h>

#include <ReconConfig.h>
#include <ReconFactory.h>
#include <ReconEngine.h>
#include <logging/logger.h>

#include <loggingdialog.h>

namespace Ui {
class MuhRecMainWindow;
}

class MuhRecMainWindow : public QMainWindow, public ApplicationBase
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit MuhRecMainWindow(QApplication *app, QWidget *parent = 0);
    ~MuhRecMainWindow();
    void UpdateDialog();
    void UpdateConfig();
    MuhrecInteractor * GetInteractor();

private:
    Ui::MuhRecMainWindow *ui;
    QtAddons::LoggingDialog *logdlg;
    QApplication *m_QtApp;
    void SetApplicationPath(std::string path) {m_sApplicationPath=path;}
    void SlicesChanged(int x);
//    void SlicesCBCTChanged(int x);


    // Collection of call-backs
    void PreviewProjection(int x);
    void ProjectionIndexChanged(int x);
    void UpdateCBCTDistances();
    void UpdatePiercingPoint();

protected slots:
    void StoreGeometrySetting();
    void ClearGeometrySettings();
    void ViewGeometryList();
    void MatrixROIChanged(int x);

    void PreviewProjection();

    void DisplaySlice();

    // Menu slots
    void MenuFileNew();
    void MenuFileOpen();
    void MenuFileSave();
    void MenuFileSaveAs();
    void MenuFileQuit();
    void MenuHelpAbout();
    void MenuReconstructStart();

protected:
    /// Sets up all call-back functions during the initialization.
    void SetupCallBacks();
    void lookForReferences(std::string &path);
    void UpdateDoseROI();
    void SetImageDimensionLimits(const std::vector<size_t> &dims);
    void CenterOfRotationChanged();
    // Other methods
    void UpdateMemoryUsage(const std::vector<size_t> &roi);
    void SetImageSizeToAdjustment();

    void LoadDefaults(bool checkCurrent);
    void LoadReconSettings();
    void ExecuteReconstruction();
    bool reconstructToDisk();
    void saveCurrentRecon();

    //Cone beam related
    void ComputeVolumeSizeSpacing();
    void ComputeVolumeSize();
    void ComputeVoxelSpacing();
    void set_slicelevels(float level);

private slots:
    void on_buttonBrowseReference_clicked();

    void on_buttonBrowseDC_clicked();

    void on_buttonGetPathDC_clicked();

    void on_comboSlicePlane_activated(int index);

    void on_actionPreferences_triggered();

    void on_actionReconstruct_to_disk_triggered();

    void on_spinSlicesFirst_valueChanged(int arg1);

    void on_spinSlicesLast_valueChanged(int arg1);

    void on_actionRemove_CurrentRecon_xml_triggered();

    void on_actionReport_a_bug_triggered();

    void on_actionVideo_tutorials_triggered();

    void on_checkCBCT_clicked(bool checked);

    void on_checkCBCT_stateChanged(int arg1);

    void on_buttonGetPP_clicked();

    void on_dspinResolution_valueChanged(double arg1);

    void on_actionRegister_for_news_letter_triggered();

    void on_sliderProjections_sliderMoved(int position);

    void on_spinFirstProjection_valueChanged(int arg1);

    void on_spinLastProjection_valueChanged(int arg1);

    void on_comboFlipProjection_currentIndexChanged(int index);

    void on_comboRotateProjection_currentIndexChanged(int index);

    void on_buttonPreview_clicked();

    void on_pushButton_logging_clicked();

    void on_actionUser_manual_triggered();

    void on_radioButton_fullTurn_clicked();

    void on_radioButton_halfTurn1_clicked();

    void on_radioButton_halfTurn2_clicked();

    void on_radioButton_customTurn_clicked();

    void on_checkCorrectTilt_clicked(bool checked);

    void on_widgetProjectionROI_valueChanged(int x0, int y0, int x1, int y1);

    void on_buttonProjectionPath_clicked();

    void on_buttonBrowseDestinationPath_clicked();

    void on_buttonTakePath_clicked();

    void on_buttonGetSkipList_clicked();

    void on_dspinRotationCenter_valueChanged(double arg1);

    void on_dspinTiltAngle_valueChanged(double arg1);

    void on_dspinTiltPivot_valueChanged(double arg1);

    void on_sliderSlices_sliderMoved(int position);

    void on_button_FindCenter_clicked();

    void on_dspinRotateRecon_valueChanged(double arg1);

    void on_dialRotateRecon_sliderMoved(int position);

    void on_buttonSaveMatrix_clicked();

    void on_dspinGrayLow_valueChanged(double low);

    void on_dspinGrayHigh_valueChanged(double arg1);

    void on_sliceViewer_levelsChanged(float low, float high);

    void on_pushButton_levels95p_clicked();

    void on_pushButton_levels99p_clicked();

    void on_pushButton_levels999p_clicked();
    
    void on_pushButtonGetSliceROI_clicked();

    void on_comboDataSequence_currentIndexChanged(int index);

    void on_actionShow_repository_triggered();

    void on_spinBox_projPerView_valueChanged(int arg1);

    void on_actionGlobal_settings_triggered();

    void on_pushButton_measurePixelSize_clicked();

    void on_spinBoxSlices_valueChanged(int arg1);

    void on_spinBoxProjections_valueChanged(int arg1);

    void on_radioButton_SOD_toggled(bool checked);

    void on_radioButton_SDD_toggled(bool checked);

    void on_radioButton_Magnification_toggled(bool checked);

    void on_dspinSOD_valueChanged(double arg1);

    void on_dspinSDD_valueChanged(double arg1);

    void on_doubleSpinBox_magnification_valueChanged(double arg1);

    void on_dspinPiercPointX_valueChanged(double arg1);

    void on_dspinPiercPointY_valueChanged(double arg1);

    void on_comboBox_projectionViewer_currentIndexChanged(int index);

    void on_spinFirstOpenBeam_valueChanged(int arg1);

    void on_spinOpenBeamCount_valueChanged(int arg1);

    void on_editOpenBeamMask_editingFinished();

    void on_editProjectionMask_editingFinished();

    void on_editDarkMask_editingFinished();

    void on_spinFirstDark_valueChanged(int arg1);

    void on_spinDarkCount_valueChanged(int arg1);

    void on_actionConvert_files_triggered();

    void on_comboBox_skiplist_currentIndexChanged(int index);

private:
    // Data members
    ReconConfig      m_Config;    ///<! Current configuration data
    ReconConfig      m_LastReconConfig;

    MuhrecInteractor m_Interactor;
    PreProcModuleConfigurator m_ModuleConfigurator;

    ReconEngine     *m_pEngine;
    ReconFactory     m_Factory;

    int              m_nCurrentPage;
    size_t           m_nRequiredMemory;
    std::string      m_sApplicationPath;
    std::string      m_sHomePath;
    std::string      m_sConfigPath;
    std::string      m_sConfigFilename; ///<! Name of the configuration file
    std::string      m_sPreviewMask;
    int              m_nPreviewFirst;
    int              m_nPreviewLast;
    std::map<float, ProjectionInfo> m_ProjectionList;

    kipl::base::TImage<float,2>     m_PreviewImage;
    kipl::base::TImage<float,2>     m_SliceImage;
    kipl::base::TImage<float,2>     m_LastMidSlice;
    kipl::base::TImage<float,3>     m_NexusTomo;

    kipl::base::eImagePlanes m_eSlicePlane;
    size_t m_nSliceSizeX;
    size_t m_nSliceSizeY;
    std::vector<int> m_oldROI;
    int m_oldRotateDial;
    double m_oldRotateSpin;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > > m_StoredReconList;
    bool m_bCurrentReconStored;
};

#endif // MUHRECMAINWINDOW_H

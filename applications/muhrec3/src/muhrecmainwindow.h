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
    QApplication *m_QtApp;
    void SetApplicationPath(std::string path) {m_sApplicationPath=path;}
    void SlicesChanged(int x);
    void SlicesCBCTChanged(int x);


    // Collection of call-backs
protected slots:
    void BrowseProjectionPath();
    void BrowseDestinationPath();
    void TakeProjectionPath();
    void GetSkipList();

    void GetDoseROI();
    void GetReconROI();
    void BinningChanged();
    void FlipChanged();
    void RotateChanged();
    void DoseROIChanged(int x);
    void ProjROIChanged(int x);
    void CenterOfRotationChanged(int x);
    void CenterOfRotationChanged(double x);
    void ConfigureGeometry();
    void StoreGeometrySetting();
    void ClearGeometrySettings();
    void ViewGeometryList();
    void GrayLevelsChanged(double x);
    void GetMatrixROI();
    void MatrixROIChanged(int x);
    void UseMatrixROI(int x);
    void SaveMatrix();
    void ProjectionIndexChanged(int x);
    void PreviewProjection();
    void PreviewProjection(int x);
    void DisplaySlice();
    void DisplaySlice(int x);

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
    void UpdateMatrixROI();
    void UpdateDoseROI();
    void SetImageDimensionLimits(size_t const * const dims);
    void CenterOfRotationChanged();
    // Other methods
    void UpdateMemoryUsage(size_t *roi);
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

    void on_checkCBCT_clicked(bool checked);

    void on_checkCBCT_stateChanged(int arg1);

    void on_buttonGetPP_clicked();

    void on_dspinResolution_valueChanged(double arg1);

//    void on_comboDirRotation_currentIndexChanged(int);

    void on_spinSubVolumeSizeZ0_valueChanged(int arg1);

    void on_spinSubVolumeSizeZ1_valueChanged(int arg1);

    void on_actionRegister_for_news_letter_triggered();

private:
    // Data members
    ReconConfig      m_Config;    //<! Current configuration data
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
    std::string      m_sConfigFilename; //<! Name of the configuration file
    std::map<float, ProjectionInfo> m_ProjectionList;

    kipl::base::TImage<float,2>     m_PreviewImage;
    kipl::base::TImage<float,2>     m_SliceImage;
    kipl::base::TImage<float,2>     m_LastMidSlice;
    kipl::base::TImage<float,3>     m_NexusTomo;

    kipl::base::eImagePlanes m_eSlicePlane;
    size_t m_nSliceSizeX;
    size_t m_nSliceSizeY;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > > m_StoredReconList;
    bool m_bCurrentReconStored;
};

#endif // MUHRECMAINWINDOW_H

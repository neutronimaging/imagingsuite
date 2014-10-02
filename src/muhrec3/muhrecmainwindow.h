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
    virtual void UpdateDialog();
    virtual void UpdateConfig();
    MuhrecInteractor * GetInteractor();

private:
    Ui::MuhRecMainWindow *ui;
    QApplication *m_QtApp;
    void SetApplicationPath(std::string path) {m_sApplicationPath=path;}

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
    void ReconROIChanged(int x);
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

    void LoadDefaults();
    void LoadReconSettings();

private slots:
    void on_buttonBrowseReference_clicked();

    void on_buttonBrowseDC_clicked();

    void on_buttonGetPathDC_clicked();

    void on_comboSlicePlane_activated(int index);

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
    std::string      m_sConfigFilename; //<! Name of the configuration file
    std::map<float, ProjectionInfo> m_ProjectionList;

    kipl::base::TImage<float,2>     m_PreviewImage;
    kipl::base::TImage<float,2>     m_SliceImage;
    kipl::base::TImage<float,2>     m_LastMidSlice;

    kipl::base::eImagePlanes m_eSlicePlane;
    size_t m_nSliceSizeX;
    size_t m_nSliceSizeY;

    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > > m_StoredReconList;
    bool m_bCurrentReconStored;
};

#endif // MUHRECMAINWINDOW_H

#ifndef MUHRECMAINWINDOW_H
#define MUHRECMAINWINDOW_H

#include <QMainWindow>
#include "MuhrecInteractor.h"
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
    explicit MuhRecMainWindow(std::string application_path, QWidget *parent = 0);
    ~MuhRecMainWindow();
    virtual void UpdateDialog();
    virtual void UpdateConfig();

private:
    Ui::MuhRecMainWindow *ui;
    void SetApplicationPath(std::string path) {m_sApplicationPath=path;}

    // Collection of call-backs
protected slots:
    void BrowseProjectionPath();
    void BrowseReferencePath();
    void BrowseDestinationPath();
    void TakeProjectionPath();

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
    void GrayLevelsChanged(double x);
    void GetMatrixROI();
    void MatrixROIChanged(int x);
    void UseMatrixROI(int x);
    void SaveMatrix();
    void ProjectionIndexChanged(int x);
    void PreviewProjection();
    void PreviewProjection(int x);

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
    void DisplayNewSlice();

private:
    // Data members
    ReconConfig      m_Config;    //<! Current configuration data
    MuhrecInteractor m_Interactor;
    ReconEngine     *m_pEngine;
    ReconFactory     m_Factory;
    int              m_nCurrentPage;
    size_t           m_nRequiredMemory;
    std::string      m_sApplicationPath;
    std::string      m_sConfigFilename; //<! Name of the configuration file
    std::map<float, ProjectionInfo> m_ProjectionList;
    kipl::base::TImage<float,2>     m_PreviewImage;
    kipl::base::TImage<float,2>     m_SliceImage;
};

#endif // MUHRECMAINWINDOW_H

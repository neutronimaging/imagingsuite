#ifndef __SPOTCLEAN2DLG_H
#define __SPOTCLEAN2DLG_H

#include <base/timage.h>
#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <imageviewerwidget.h>
#include <plotter.h>

#include <SpotClean2.h>
#include <SpotClean.h>
#include <ReconConfig.h>
#include <QComboBox>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class SpotClean2Dlg : public ConfiguratorDialogBase
{
public:
    SpotClean2Dlg(QWidget * parent = NULL);
	virtual ~SpotClean2Dlg(void);
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> img);
private:
    virtual void ApplyParameters();
    virtual void UpdateDialog();
    virtual void UpdateParameters();
	void UpdateParameterList(std::map<std::string, std::string> &parameters);
    QVBoxLayout m_vbox_main;
    QHBoxLayout m_hbox_viewers;

    QFrame m_frame_original;
    QFrame m_frame_processed;
    QFrame m_frame_difference;
    QFrame m_frame_plots;
    QtAddons::ImageViewerWidget m_viewer_original;
    QtAddons::ImageViewerWidget m_viewer_processed;
    QtAddons::ImageViewerWidget m_viewer_difference;
    QtAddons::Plotter           m_plot;

    QHBoxLayout m_hbox_params_plot;
    QVBoxLayout m_vbox_parameters;
    QHBoxLayout m_hbox_parameters;
    QDoubleSpinBox m_entry_gamma;
    QDoubleSpinBox m_entry_sigma;
    QSpinBox m_entry_iterations;
    QSpinBox m_entry_maxarea;
    QComboBox m_combo_detectionmethod;
    QLabel lbl_gamma;
    QLabel lbl_sigma;
    QLabel lbl_iterations;
    QLabel lbl_maxarea;
    QLabel lbl_detection;

    QHBoxLayout m_hbox_minmax;
    QDoubleSpinBox m_entry_min;
    QDoubleSpinBox m_entry_max;
    QLabel lbl_min;
    QLabel lbl_max;

	ReconConfig *m_Config;
	SpotClean2 cleaner;
	kipl::base::TImage<float,3> m_Projections;
	ImagingAlgorithms::DetectionMethod m_eDetectionMethod;
	float m_fGamma;
	float m_fSigma;
	int   m_nIterations;
	float m_fMaxLevel;
	float m_fMinLevel;
	size_t m_nMaxArea;
};

#endif

#ifndef __MORPHSPOTCLEANDLG_H
#define __MORPHSPOTCLEANDLG_H

#include <base/timage.h>
#include <morphology/morphology.h>
#include <ConfiguratorDialogBase.h>
#include <ConfigBase.h>
#include <imageviewerwidget.h>
#include <plotter.h>

#include <MorphSpotCleanModule.h>
#include <ReconConfig.h>
#include <QComboBox>
#include <QFrame>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class MorphSpotCleanDlg : public ConfiguratorDialogBase
{
public:
    MorphSpotCleanDlg(QWidget * parent = NULL);
    virtual ~MorphSpotCleanDlg(void);
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
    QDoubleSpinBox m_spin_threshold;
    QComboBox m_combo_CleanMethod;
    QComboBox m_combo_Connectivity;
    QLabel lbl_threshold;
    QLabel lbl_Method;
    QLabel lbl_Connectivity;

	ReconConfig *m_Config;
    MorphSpotCleanModule m_Cleaner;
	kipl::base::TImage<float,3> m_Projections;
    kipl::morphology::MorphConnect m_eConnectivity;
    ImagingAlgorithms::eMorphCleanMethod m_eCleanMethod;
    float m_fThreshold;
};

#endif

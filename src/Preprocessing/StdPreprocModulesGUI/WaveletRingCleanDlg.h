/*
 * WaveletRingCleanDlg.h
 *
 *  Created on: Mar 1, 2012
 *      Author: kaestner
 */

#ifndef WaveletRingCleanDlg_H_
#define WaveletRingCleanDlg_H_

#include <ConfiguratorDialogBase.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QFrame>

#include <base/timage.h>

#include <ConfigBase.h>
#include <imageviewerwidget.h>
#include <plotter.h>

#include <WaveletRingClean.h>
#include <StripeFilter.h>
#include <ReconConfig.h>

class WaveletRingCleanDlg: public ConfiguratorDialogBase {
public:
    WaveletRingCleanDlg(QWidget * parent = NULL);
	virtual ~WaveletRingCleanDlg();
    virtual int exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img);
protected slots:
    void on_refresh();
    void on_change_level();

private:
	void prepare_waveletcombobox();
    virtual void ApplyParameters();
	void UpdateDialog();
	void UpdateParameters();
	void UpdateParameterList(std::map<std::string, std::string> &parameters);
    kipl::base::TImage<float,2> GetSinogram(kipl::base::TImage<float,3> &proj, size_t index);

    QVBoxLayout m_vbox_main;
    QHBoxLayout m_hbox_viewers;

    QtAddons::ImageViewerWidget m_viewer_original;
    QtAddons::ImageViewerWidget m_viewer_processed;
    QtAddons::ImageViewerWidget m_viewer_difference;

    QFrame m_frame_original;
    QFrame m_frame_processed;
    QFrame m_frame_difference;
    QVBoxLayout m_vbox_parameters;
    QHBoxLayout m_hbox_parameters;
    QSpinBox m_entry_levels;
    QDoubleSpinBox m_entry_sigma;
    QComboBox m_combobox_wavelets;
    QComboBox m_combobox_method;
    QLabel lbl_waveletname;
    QLabel lbl_levels;
    QLabel lbl_sigma;

	ReconConfig *m_Config;
	WaveletRingClean cleaner;
	kipl::base::TImage<float,3> m_Projections;
	int m_nLevels;
	float m_fSigma;
	std::string m_sWaveletName;
	bool m_bParallel;
	ImagingAlgorithms::StripeFilterOperation m_eCleaningMethod;

};

#endif /* WaveletRingCleanDlg_H_ */

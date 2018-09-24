#include "issfilterdlg.h"
#include "ui_issfilterdlg.h"

ISSFilterDlg::ISSFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("ISSFilter", false, true, true,parent),
    ui(new Ui::ISSFilterDlg)
{
    ui->setupUi(this);
}

ISSFilterDlg::~ISSFilterDlg()
{
    delete ui;
}

void ISSFilterDlg::UpdateDialog()
{
    ui->groupBox_normalization->setChecked(!m_bAutoScale);
    ui->doubleSpinBox_intercept->setValue(m_fIntercept);
    ui->doubleSpinBox_slope->setValue(m_fSlope);
    ui->doubleSpinBox_alpha->setValue(m_fAlpha);
    ui->doubleSpinBox_lambda->setValue(m_fLambda);
    ui->doubleSpinBox_tau->setValue(m_fTau);
    ui->spinBox_iterations->setValue(m_nIterations);

    ui->comboBox_initialImage->setCurrentIndex(static_cast<int>(m_eInitialImage));
    ui->comboBox_regularization->setCurrentIndex(static_cast<int>(m_eRegularization));
}

void ISSFilterDlg::UpdateParameters()
{
    m_bAutoScale=!ui->groupBox_normalization->isChecked();
    m_fSlope=ui->doubleSpinBox_slope->value();
    m_fIntercept=ui->doubleSpinBox_intercept->value();

    m_fTau=ui->doubleSpinBox_tau->value();
    m_fLambda=ui->doubleSpinBox_lambda->value();
    m_fAlpha=ui->doubleSpinBox_alpha->value();
    m_nIterations=ui->spinBox_iterations->value();
//    std::string m_sIterationPath;
//    bool m_bSaveIterations;

    m_eRegularization=static_cast<akipl::scalespace::eRegularizationType>(ui->comboBox_regularization->currentIndex());
    m_eInitialImage=static_cast<akipl::scalespace::eInitialImageType>(ui->comboBox_initialImage->currentIndex());
}

void ISSFilterDlg::ApplyParameters()
{

}

#include "doublethresholddlg.h"
#include "ui_doublethresholddlg.h"

DoubleThresholdDlg::DoubleThresholdDlg(QWidget *parent) :
    ConfiguratorDialogBase("DoubleThreshold", true, false, true,parent),
    ui(new Ui::DoubleThresholdDlg)
{
    ui->setupUi(this);
}

DoubleThresholdDlg::~DoubleThresholdDlg()
{
    delete ui;
}

void DoubleThresholdDlg::UpdateDialog()
{

}

void DoubleThresholdDlg::UpdateParameters()
{

}

void DoubleThresholdDlg::ApplyParameters()
{

}

void DoubleThresholdDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{

}

int DoubleThresholdDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{

}

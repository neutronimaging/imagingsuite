//<LICENSE>

#include <sstream>

#include <ParameterHandling.h>
#include <ReconException.h>

#include "generalfilterdlg.h"
#include "ui_generalfilterdlg.h"



GeneralFilterDlg::GeneralFilterDlg(QWidget *parent) :
    ConfiguratorDialogBase("GeneralFilterDlg",
                           true, // empty dialog
                           false, // has apply
                           false, // need images
                           parent),
    ui(new Ui::GeneralFilterDlg)
{
    ui->setupUi(this);
}

GeneralFilterDlg::~GeneralFilterDlg()
{
    delete ui;
}


int GeneralFilterDlg::exec(ConfigBase * config, std::map<std::string, std::string> &parameters, kipl::base::TImage<float,3> & img)
{
    int res=0;
    std::ostringstream msg;

    try {
        string2enum(GetStringParameter(parameters,"type"),filterType);
    }
    catch (...) {
        msg.str("");
        msg<<"Parameter for filter type missing, using box filter.";
        logger(logger.LogWarning,msg.str());

        filterType = GeneralFilter::FilterBox;
    }

    try {
        filterSize = GetFloatParameter(parameters,"size");
    }
    catch (...) {
        msg.str("");

        switch (filterType) {
            case GeneralFilter::FilterBox   : filterSize = 3.0; break;
            case GeneralFilter::FilterGauss : filterSize = 1.5; break;
            default: throw(ReconException("Unknown filter type",__FILE__,__LINE__));
        }

        msg<<"Parameter for filter size missing, using "<<filterSize;
        logger(logger.LogWarning,msg.str());
    }

    res = QDialog::exec();

    if (res==QDialog::Accepted) {
        UpdateParameters();
        UpdateParameterList(parameters);
    }

    return res;
}

void GeneralFilterDlg::ApplyParameters()
{

}

void GeneralFilterDlg::UpdateDialog()
{
    ui->spinBox_FilterSize->setValue(filterSize);
    ui->comboFilter->setCurrentIndex(static_cast<int>(filterType));
}

void GeneralFilterDlg::UpdateParameters()
{
    filterSize = ui->spinBox_FilterSize->value();
    filterType = static_cast<GeneralFilter::eGeneralFilter>(ui->comboFilter->currentIndex());
}

void GeneralFilterDlg::UpdateParameterList(std::map<std::string, std::string> &parameters)
{
    parameters.clear();

    parameters["type"]=enum2string(filterType);
    parameters["size"]=kipl::strings::value2string(filterSize);
}

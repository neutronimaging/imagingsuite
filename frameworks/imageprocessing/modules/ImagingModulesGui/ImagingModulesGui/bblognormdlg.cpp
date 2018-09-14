
#include "bblognormdlg.h"
#include "ui_bblognormdlg.h"

bblognormDlg::bblognormDlg(QWidget *parent) :
    ConfiguratorDialogBase("BBLogNorm",true,false,true,parent),
    ui(new Ui::bblognormDlg)
{


        std::stringstream msg;
        msg<<"Setting up the dialog for BBLogNorm";
        logger(kipl::logging::Logger::LogDebug,msg.str());
        ui->setupUi(this);
}

bblognormDlg::~bblognormDlg()
{
    delete ui;
}

int bblognormDlg::exec(ConfigBase *config, std::map<string, string> &parameters, kipl::base::TImage<float, 3> &img)
{

}

void bblognormDlg::ApplyParameters()
{

}


void bblognormDlg::UpdateDialog()
{

}

void bblognormDlg::UpdateParameters()
{

}

void bblognormDlg::UpdateParameterList(std::map<string, string> &parameters)
{

}

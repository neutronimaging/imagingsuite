//<LICENSE>

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    m_MemoryLimit(1500),
    m_LogLevel(kipl::logging::Logger::LogMessage)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::UpdateDialog()
{
    ui->spinBox_MemoryLimit->setValue(m_MemoryLimit);
    ui->comboBox_LogLevel->setCurrentIndex(static_cast<int>(m_LogLevel));
}

void PreferencesDialog::UpdateConfig()
{
    m_MemoryLimit=ui->spinBox_MemoryLimit->value();
    m_LogLevel=static_cast<kipl::logging::Logger::LogLevel>(ui->comboBox_LogLevel->currentIndex());
}

int PreferencesDialog::exec()
{
    UpdateDialog();

    int res=QDialog::exec();

    UpdateConfig();

    return res;
}

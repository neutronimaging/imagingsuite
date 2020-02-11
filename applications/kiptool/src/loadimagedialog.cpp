//<LICENSE>

#include "loadimagedialog.h"
#include "ui_loadimagedialog.h"

#include <QMessageBox>
#include <QtConcurrent>
#include <QFuture>

#include <KiplFrameworkException.h>
#include <ModuleException.h>
#include <base/KiplException.h>
#include "ImageIO.h"

LoadImageDialog::LoadImageDialog(kipl::interactors::InteractionBase *interactor, QWidget *parent) :
    QDialog(parent),
    logger("LoadImageDialog"),
    ui(new Ui::LoadImageDialog),
    m_Interactor(interactor)
{
    ui->setupUi(this);
    if (interactor==nullptr)
        throw KiplFrameworkException("Progress dialog can't open without valid interactor");

    connect(this,&LoadImageDialog::updateProgress,this,&LoadImageDialog::changedProgress);
    connect(this,&LoadImageDialog::processFailure,this,&LoadImageDialog::on_processFailure);
}

LoadImageDialog::~LoadImageDialog()
{
    delete ui;
}

int LoadImageDialog::exec(KiplProcessConfig *config, kipl::base::TImage<float,3> *img)
{
    if (img==nullptr)
    {
        logger(logger.LogError,"Called load image dialog with uninitialized image");
        return Rejected;
    }

    m_img=img;

    if (config==nullptr)
    {
        logger(logger.LogError,"Called load image dialog without config");
        return Rejected;
    }

    mConfig=config;

    finish=false;
    logger(kipl::logging::Logger::LogMessage,"Start");

    m_Interactor->Reset();

    logger(logger.LogMessage,"Starting with threads");

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    QFuture<int> proc_thread=QtConcurrent::run(this,&LoadImageDialog::process);
    QFuture<int> progress_thread=QtConcurrent::run(this,&LoadImageDialog::progress);

    int res=exec();

    finish=true;
    if (res==QDialog::Rejected) {
        logger(kipl::logging::Logger::LogVerbose,"Cancel requested by user");
        Abort();
    }

    proc_thread.waitForFinished();
    progress_thread.waitForFinished();
    logger(kipl::logging::Logger::LogVerbose,"Threads are joined");
    return res;

}

int LoadImageDialog::progress()
{
    logger(kipl::logging::Logger::LogMessage,"Progress thread is started");
    QThread::msleep(250);
    while (!m_Interactor->Finished() && !m_Interactor->Aborted() ){
        emit updateProgress(m_Interactor->CurrentProgress(),
                            m_Interactor->CurrentOverallProgress(),
                            QString::fromStdString(m_Interactor->CurrentMessage()));

        QThread::msleep(50);
    }
    logger(kipl::logging::Logger::LogMessage,"Progress thread end");

    return 0;
}

void LoadImageDialog::changedProgress(float progress, float overallProgress, QString msg)
{
    ui->progressBar->setValue(static_cast<int>(progress*100));

    ui->label_message->setText(msg);
}

int LoadImageDialog::process()
{
    logger(kipl::logging::Logger::LogMessage,"Process thread is started");
    ostringstream msg;

    bool failed=false;
    try {
        *m_img = LoadVolumeImage(*mConfig,m_Interactor);
    }
    catch (KiplFrameworkException &e)
    {
        msg<<"ReconException with message: "<<e.what();
        failed =true;
    }
    catch (ModuleException &e) {
        msg<<"ModuleException with message: "<<e.what();
        failed = true;
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"KiplException with message: "<<e.what();
        failed = true ;
    }
    catch (std::exception &e)
    {
        msg<<"STL exception with message: "<<e.what();
        failed = true;
    }
    catch (...)
    {
        msg<<"An unknown exception";
        failed =true;
    }

    if (failed==true)
    {
        logger(kipl::logging::Logger::LogMessage,msg.str());
        finish=false;
        emit processFailure(QString::fromStdString(msg.str()));
        return 0;
    }
    logger(kipl::logging::Logger::LogMessage,"Loading done");

    finish=true;
    m_Interactor->Done();
    this->accept();

    return 0;
}

void LoadImageDialog::Abort()
{
    if (m_Interactor!=nullptr) {
        m_Interactor->Abort();
    }
    this->reject();
}

bool LoadImageDialog::Finished()
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->Finished();
    }

    return true;
}

void LoadImageDialog::on_processFailure(QString msg)
{
    QMessageBox dlg;
    dlg.setWindowTitle("Failed to load image");

    dlg.setText("Could not load the image.");
    dlg.setDetailedText(msg);
    dlg.exec();
    Abort();
}

void LoadImageDialog::on_buttonBox_rejected()
{
    Abort();
    this->reject();
}

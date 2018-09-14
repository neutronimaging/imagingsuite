//<LICENSE>

#include <QMessageBox>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>

#include "processdialog.h"
#include "ui_processdialog.h"
#include <KiplFrameworkException.h>
#include <ModuleException.h>
#include <base/KiplException.h>

ProcessDialog::ProcessDialog(kipl::interactors::InteractionBase *interactor, QWidget *parent) :
    QDialog(parent),
    logger("ProcessDialog"),
    ui(new Ui::ProcessDialog),
    fraction(0.0f),
    finish(false),
    m_Interactor(interactor)
{
    ui->setupUi(this);
    if (interactor==nullptr)
        throw KiplFrameworkException("Progress dialog can't open without valid interactor");
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}


int ProcessDialog::exec(KiplEngine * engine, kipl::base::TImage<float,3> *img)
{
    if (img==nullptr)
    {
        logger(logger.LogError,"Called recon dialog with uninitialized image");
        return Rejected;
    }

    m_img=img;

    if (engine==nullptr)
    {
        logger(logger.LogError,"Called recon dialog with unallocated engine");
        return Rejected;
    }

    m_Engine=engine;

    finish=false;
    logger(kipl::logging::Logger::LogMessage,"Start");

    m_Interactor->Reset();

    logger(logger.LogMessage,"Starting with threads");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    QFuture<int> proc_thread=QtConcurrent::run(this,&ProcessDialog::process);
    QFuture<int> progress_thread=QtConcurrent::run(this,&ProcessDialog::progress);

    int res=exec();

    finish=true;
    if (res==QDialog::Rejected) {
        qDebug() <<"calling abort process";
        logger(kipl::logging::Logger::LogVerbose,"Cancel requested by user");
        Abort();
    }

    proc_thread.waitForFinished();
    progress_thread.waitForFinished();
    logger(kipl::logging::Logger::LogVerbose,"Threads are joined");
    return res;

}

int ProcessDialog::progress()
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

void ProcessDialog::changedProgress(float progress, float overallProgress, QString msg)
{
    ui->progressBar->setValue(static_cast<int>(progress*100));
    ui->progressBar_overall->setValue(static_cast<int>(overallProgress*100));

    ui->label_message->setText(msg);
}

int ProcessDialog::process()
{
    logger(kipl::logging::Logger::LogMessage,"Process thread is started");
    ostringstream msg;

    bool failed=false;
    try {
        if (m_Engine!=nullptr)
            m_Engine->Run(m_img);
        else {
            logger(logger.LogError,"Trying to start an unallocated engine.");
            failed=true;
        }
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
    logger(kipl::logging::Logger::LogMessage,"Reconstruction done");

    finish=true;
    m_Interactor->Done();
    this->accept();

    return 0;
}

void ProcessDialog::Abort()
{
    if (m_Interactor!=nullptr) {
        m_Interactor->Abort();
    }
    this->reject();
}

bool ProcessDialog::Finished()
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->Finished();
    }

    return true;
}

void ProcessDialog::on_processFailure(QString msg)
{
    QMessageBox dlg;
    dlg.setWindowTitle("Reconstruction error");

    dlg.setText("Reconstruction failed");
    dlg.setDetailedText(msg);
    dlg.exec();
    Abort();
}

void ProcessDialog::on_buttonCancel_clicked()
{
    this->reject();
}

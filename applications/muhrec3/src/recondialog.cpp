//<LICENSE>

#include <QMessageBox>
#include <QtConcurrent>
#include <QFuture>
#include "recondialog.h"
#include "ui_recondialog.h"


#include <iostream>
#include <ReconException.h>
#include <ModuleException.h>
#include <base/KiplException.h>

ReconDialog::ReconDialog(kipl::interactors::InteractionBase *interactor, QWidget *parent) :
    QDialog(parent),
    logger("ReconDialog"),
    ui(new Ui::ReconDialog),
    fraction(0.0f),
    finish(false),
    m_Engine(nullptr),
    m_Interactor(interactor),
    m_bRerunBackproj(false)
{
    ui->setupUi(this);
}

ReconDialog::~ReconDialog()
{
    delete ui;
}

int ReconDialog::exec(ReconEngine * engine, bool bRerunBackProj)
{
    if (engine==nullptr)
    {
        logger(logger.LogError,"Called recon dialog with unallocated engine");
        return Rejected;
    }

    m_Engine=engine;
    m_bRerunBackproj=bRerunBackProj;
    finish=false;
    logger(kipl::logging::Logger::LogMessage,"Start");

    m_Interactor->Reset();

#ifdef _MSC_VER
    #undef USEDIALOG
//    #define USEDIALOG
#else
    #define USEDIALOG
#endif

#ifdef USEDIALOG
    logger(logger.LogMessage,"Starting with threads");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    QFuture<int> proc_thread=QtConcurrent::run(this,&ReconDialog::process);
    QFuture<int> progress_thread=QtConcurrent::run(this,&ReconDialog::progress);

    int res=exec();

    finish=true;
    if (res==QDialog::Rejected) {
        std::cout<<"calling abort process"<<std::endl;
        logger(kipl::logging::Logger::LogVerbose,"Cancel requested by user");
        Abort();
    }

    proc_thread.waitForFinished();
    progress_thread.waitForFinished();
    logger(kipl::logging::Logger::LogVerbose,"Threads are joined");
    return res;
#else
    bool bFailure=false;

    ostringstream msg;
    try {
        process();
    }
    catch (std::exception &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bFailure=true;
    }
    catch (ReconException &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bFailure=true;
    }
    catch (kipl::base::KiplException &e) {
        msg<<"Reconstruction failed: "<<endl
            <<e.what();
        bFailure=true;
    }
    catch (...) {
        msg<<"Reconstruction failed";
        bFailure=true;
    }

    if (bFailure) {
        QMessageBox error_dlg(this);
        error_dlg.setText("Reconstruction failed");
        error_dlg.setDetailedText(QString::fromStdString(msg.str()));
        error_dlg.exec();
        logger(kipl::logging::Logger::LogError,msg.str());
        return Rejected;
    }
    return Accepted;
#endif
}

int ReconDialog::progress()
{
    logger(kipl::logging::Logger::LogMessage,"Progress thread is started");
    QThread::msleep(250);
    while (!m_Interactor->Finished() && !m_Interactor->Aborted() ){
        ui->progressBar->setValue(m_Interactor->CurrentProgress()*100);
        ui->progressBar_overall->setValue(m_Interactor->CurrentOverallProgress()*100);

        ui->label_message->setText(QString::fromStdString(m_Interactor->CurrentMessage()));

        QThread::msleep(50);
    }
    logger(kipl::logging::Logger::LogMessage,"Progress thread end");

    return 0;
}

int ReconDialog::process()
{
    logger(kipl::logging::Logger::LogMessage,"Process thread is started");
    ostringstream msg;
    QMessageBox dlg;
    dlg.setWindowTitle("Reconstruction error");
    bool failed=false;
    try {
        if (m_Engine!=nullptr)
            m_Engine->Run3D(m_bRerunBackproj);
        else {
            logger(logger.LogError,"Trying to start an unallocated engine.");
            failed=true;
        }
    }
    catch (ReconException &e)
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
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        logger(kipl::logging::Logger::LogMessage,msg.str());
        finish=false;
        Abort();
        return 0;
    }
    logger(kipl::logging::Logger::LogMessage,"Reconstruction done");

    finish=true;
    m_Interactor->Done();
    this->accept();
    return 0;
}

void ReconDialog::Abort()
{
    if (m_Interactor!=nullptr) {
        m_Interactor->Abort();
    }
}

bool ReconDialog::Finished()
{
    if (m_Interactor!=nullptr) {
        return m_Interactor->Finished();
    }

    return true;
}


void ReconDialog::on_buttonCancel_clicked()
{
    this->reject();
}

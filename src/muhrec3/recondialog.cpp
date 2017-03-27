//#include "stdafx.h"

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
    m_Engine(NULL),
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
    m_Engine=engine;
    m_bRerunBackproj=bRerunBackProj;
    finish=false;
    logger(kipl::logging::Logger::LogMessage,"Start");

    m_Interactor->Reset();
#if NEVERDO
    logger(logger.LogMessage,"Starting with threads");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    QFuture<int> proc_thread=QtConcurrent::run(&this->process());
    QFuture<int> progress_thread=QtConcurrent::run(&this->progress());
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
    logger(kipl::logging::Logger::LogVerbose,"Progress thread is started");

    while (!m_Interactor->Finished() && !m_Interactor->Aborted() ){
        ui->progressBar->setValue(m_Interactor->CurrentProgress());

        QThread::sleep(1);


    }
    logger(kipl::logging::Logger::LogVerbose,"Progress thread end");
}

int ReconDialog::process()
{
    logger(kipl::logging::Logger::LogVerbose,"Process thread is started");
    ostringstream msg;
    QMessageBox dlg;
    dlg.setWindowTitle("Reconstruction error");
    try {
        m_Engine->Run3D(m_bRerunBackproj);
    }
    catch (ReconException &e)
    {
        msg<<"ReconException with message: "<<e.what();
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        Abort();
    }
    catch (ModuleException &e) {
        msg<<"ModuleException with message: "<<e.what();
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        Abort();
    }
    catch (kipl::base::KiplException &e)
    {
        msg<<"KiplException with message: "<<e.what();
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        Abort();
    }
    catch (std::exception &e)
    {
        msg<<"STL exception with message: "<<e.what();
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        Abort();
    }
    catch (...)
    {
        msg<<"An unknown exception";
        dlg.setText("Reconstruction failed");
        dlg.setDetailedText(QString::fromStdString(msg.str()));
        dlg.exec();
        Abort();
    }

    logger(kipl::logging::Logger::LogMessage,"Reconstruction done");
    finish=true;
}

void ReconDialog::Abort()
{
    if (m_Interactor!=NULL) {
        m_Interactor->Abort();
    }
}

bool ReconDialog::Finished()
{
    if (m_Interactor!=NULL) {
        return m_Interactor->Finished();
    }

    return true;
}
void ReconDialog::pressedCancel()
{

}

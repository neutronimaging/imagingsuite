//<LICENSE>

#include <QMessageBox>

#include "recondialog.h"
#include "ui_recondialog.h"


#include <iostream>
#include <thread>
#include <chrono>
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
    connect(this,&ReconDialog::updateProgress,this,&ReconDialog::changedProgress);
    connect(this,&ReconDialog::processFailure,this,&ReconDialog::on_processFailure);
    connect(this,&ReconDialog::processDone,this,&ReconDialog::on_processDone);
}

ReconDialog::~ReconDialog()
{
    delete ui;
}

int ReconDialog::exec(ReconEngine * engine, bool bRerunBackProj)
{
    if (engine==nullptr)
    {
        logger.error("Called recon dialog with unallocated engine");
        return Rejected;
    }

    m_Engine=engine;
    m_bRerunBackproj=bRerunBackProj;
    finish=false;
    logger.message("Start");

    m_Interactor->Reset();
    std::ostringstream msg;

    msg<<"Starting the threads (this="<<this<<")";
    logger.message(msg.str());
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);

    auto process_thread  = std::thread([=,this] { process(); });
    auto progress_thread = std::thread([=,this] { progress(); });

    int res=QDialog::exec();

    logger.message("Left dialog");
    finish=true;
    if (res==QDialog::Rejected)
    {
        logger.verbose("Calling abort process");
        logger.message("Cancel requested by user");
        Abort();
    }

    logger.message("Joining threads");
    process_thread.join();
    progress_thread.join();
    logger.message("Threads are joined");
    return res;
}

void ReconDialog::progress()
{
    std::ostringstream msg;
    msg<<"Progress thread (this="<<this<<")";
    logger.message(msg.str());

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    while (!m_Interactor->Finished() && !m_Interactor->Aborted() )
    {
        emit updateProgress(m_Interactor->CurrentProgress(),
                            m_Interactor->CurrentOverallProgress(),
                            QString::fromStdString(m_Interactor->CurrentMessage()));

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    logger.message("Progress thread end");
}

void ReconDialog::changedProgress(float progress, float overallProgress, const QString & msg)
{
    ui->progressBar->setValue(static_cast<int>(progress*100));
    ui->progressBar_overall->setValue(static_cast<int>(overallProgress*100));

    ui->label_message->setText(msg);
}

void ReconDialog::process()
{
    std::ostringstream msg;
    msg<<"Process thread (this="<<this<<")";
    logger.message(msg.str());

    msg.str("");
    bool failed=false;
    try {
        if (m_Engine!=nullptr)
            m_Engine->Run3D(m_bRerunBackproj);
        else
        {
            logger.error("Trying to start an unallocated engine.");
            failed=true;
        }
    }
    catch (ReconException &e)
    {
        msg<<"ReconException with message: "<<e.what();
        failed = true;
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
        logger.error(msg.str());
        finish = false;
        emit processFailure(QString::fromStdString(msg.str()));
        return;
    }
    logger.message("Reconstruction done");

    finish=true;
    m_Interactor->Done();
    emit processDone();

    logger.message("Process signaled accept");
}

void ReconDialog::Abort()
{
    if (m_Interactor!=nullptr)
    {
        m_Interactor->Abort();
    }
    this->reject();
}

bool ReconDialog::Finished()
{
    if (m_Interactor!=nullptr)
    {
        return m_Interactor->Finished();
    }

    return true;
}

void ReconDialog::on_processFailure(const QString & msg)
{
    QMessageBox dlg;
    dlg.setWindowTitle("Reconstruction error");

    dlg.setText("Reconstruction failed");
    dlg.setDetailedText(msg);
    dlg.exec();
    Abort();
}

void ReconDialog::on_processDone()
{
    accept();
}

void ReconDialog::on_buttonCancel_clicked()
{
    this->reject();
}

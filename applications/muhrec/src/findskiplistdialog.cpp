//<LICENSE>

#include "findskiplistdialog.h"
#include "ui_findskiplistdialog.h"
#include <strings/filenames.h>
#include <ProjectionReader.h>
#include <base/KiplException.h>
#include <QMessageBox>
#include <plotter.h>
#include <set>

#include <sstream>

FindSkipListDialog::FindSkipListDialog(QWidget *parent) :
    QDialog(parent),
    logger("FindSkipListDialog"),
    ui(new Ui::FindSkipListDialog),
    m_nMaxNumberProjections(0)
{
    ui->setupUi(this);

    ui->plotProjectionDose->hideLegend();
}

FindSkipListDialog::~FindSkipListDialog()
{
    delete ui;
}

int FindSkipListDialog::exec(ReconConfig &config)
{
    ostringstream msg;
    try {
        LoadDoseList(config);
    }
    catch (kipl::base::KiplException &e) {
        QMessageBox msgbox;

        msg.str("");
        msg<<"Failed to load projection doses, with exception :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());
        msgbox.setText("Failed to load projection doses.");
        msgbox.setWindowTitle("Load Error");
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();
        return QDialog::Rejected;
    }
    catch (std::exception &e) {
        QMessageBox msgbox;

        msg.str("");
        msg<<"Failed to load projection doses, with exception :\n"<<e.what();
        logger(kipl::logging::Logger::LogError,msg.str());

        msgbox.setText("Failed to load projection doses.");
        msgbox.setWindowTitle("Load Error");
        msgbox.setDetailedText(QString::fromStdString(msg.str()));
        msgbox.exec();

        return QDialog::Rejected;
    }
    m_nMaxNumberProjections=config.ProjectionInfo.nLastIndex-config.ProjectionInfo.nFirstIndex+1;
    ui->spinExpNumProj->setMaximum(m_nMaxNumberProjections);
    ui->spinExpNumProj->setMinimum(0);
    ui->spinExpNumProj->setValue(m_nMaxNumberProjections);
    on_spinExpNumProj_valueChanged(m_nMaxNumberProjections);

    return QDialog::exec();
}

void FindSkipListDialog::LoadDoseList(ReconConfig &config)
{
    ostringstream msg;
    ProjectionReader reader;
    m_DoseData.clear();
    m_SortedDoses.clear();
    float fDose=0.0f;
    for (size_t i=config.ProjectionInfo.nFirstIndex; i<=config.ProjectionInfo.nLastIndex; i++) {

               fDose=reader.GetProjectionDose(config.ProjectionInfo.sPath,
                                    config.ProjectionInfo.sFileMask,
                                    i,
                                    config.ProjectionInfo.eFlip,
                                    config.ProjectionInfo.eRotate,
                                    config.ProjectionInfo.fBinning,
                                    config.ProjectionInfo.dose_roi);

               m_DoseData.append(QPoint(static_cast<float>(i),fDose));
               m_SortedDoses.insert(std::make_pair(fDose,i));
    }
    ui->plotProjectionDose->setCurveData(0,m_DoseData);
    if (m_SortedDoses.size()!=static_cast<size_t>(m_DoseData.size())) {
        msg.str("");
        msg<<"Dose data size missmatch. size(DosePlot)="<<m_DoseData.size()<<", size(SortedDoses)="<<m_SortedDoses.size();
        throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
    }
}

QString FindSkipListDialog::getSkipList()
{
    return ui->editSkipList->text();
}



void FindSkipListDialog::on_pushButton_updatePlot_clicked()
{
    int i=0;
    ui->plotProjectionDose->clearAllCursors();

    for (const auto & idx : m_sortedSkip)
    {
        ui->plotProjectionDose->setCursor(i,new QtAddons::PlotCursor(idx,QColor("tomato"),QtAddons::PlotCursor::Vertical));
        ++i;
    }
}

void FindSkipListDialog::on_spinExpNumProj_valueChanged(int x)
{
    std::ostringstream skipstr;
    int nSkipCnt=m_nMaxNumberProjections-x;

    std::multimap<float,int>::iterator it=m_SortedDoses.begin();

    m_sortedSkip.clear();

    for (int i=0; i<nSkipCnt; i++, it++)
    {
        m_sortedSkip.insert(it->second);
    }
    // Todo: Sort the list
    std::set<int>::iterator it2;
    for (const auto & projIdx : m_sortedSkip)
    {
        skipstr<<projIdx<<" ";
    }

    ui->editSkipList->setText(QString::fromStdString(skipstr.str()));

    if (nSkipCnt<10)
    {
        on_pushButton_updatePlot_clicked();
        ui->pushButton_updatePlot->hide();

    }
    else
    {
        ui->pushButton_updatePlot->show();
    }
}

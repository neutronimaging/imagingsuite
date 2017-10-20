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
    connect(ui->spinExpNumProj,SIGNAL(valueChanged(int)),SLOT(ChangedNumberOfProjections(int)));

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
    ChangedNumberOfProjections(m_nMaxNumberProjections);
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

void FindSkipListDialog::ChangedNumberOfProjections(int x)
{
    std::ostringstream skipstr;
    int nSkipCnt=m_nMaxNumberProjections-x;
    ui->plotProjectionDose->clearAllPlotCursors();
    std::multimap<float,int>::iterator it=m_SortedDoses.begin();
    std::set<int> sortedSkip;
    for (int i=0; i<nSkipCnt; i++, it++) {
        sortedSkip.insert(it->second);
        ui->plotProjectionDose->setPlotCursor(i,QtAddons::PlotCursor(it->second,QColor("green"),QtAddons::PlotCursor::Vertical));
    }
    // Todo: Sort the list
   std::set<int>::iterator it2;
    for (it2=sortedSkip.begin(); it2!=sortedSkip.end(); it2++) {
        skipstr<<*it2<<" ";
    }


    ui->editSkipList->setText(QString::fromStdString(skipstr.str()));
}

QString FindSkipListDialog::getSkipList()
{
    return ui->editSkipList->text();
}

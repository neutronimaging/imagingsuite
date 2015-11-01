#include "findskiplistdialog.h"
#include "ui_findskiplistdialog.h"

#include <QMessageBox>

#include <strings/filenames.h>
#include <base/timage.h>
#include <io/io_fits.h>
#include <base/kiplenums.h>
#include <strings/string2array.h>

#include <ImageReader.h>
#include <buildfilelist.h>
#include <datasetbase.h>

#include <set>
#include <sstream>


FindSkipListDialog::~FindSkipListDialog()
{
    delete ui;
}

FindSkipListDialog::FindSkipListDialog(QWidget *parent) :
    QDialog(parent),
    logger("FindSkipListDialog"),
    ui(new Ui::FindSkipListDialog),
    m_nMaxNumberProjections(0)
{
    ui->setupUi(this);

    m_nROI[0]=0;
    m_nROI[1]=0;
    m_nROI[2]=10;
    m_nROI[3]=10;
    ui->skip_spin_x0->setValue(0);
    ui->skip_spin_y0->setValue(0);
    ui->skip_spin_x1->setValue(10);
    ui->skip_spin_y1->setValue(10);

    connect(ui->skip_spin_expnumproj,SIGNAL(valueChanged(int)),SLOT(ChangedNumberOfProjections(int)));
    connect(ui->skip_spin_x0, SIGNAL(valueChanged(int)),SLOT(ChangedROI(int)));
    connect(ui->skip_spin_y0, SIGNAL(valueChanged(int)),SLOT(ChangedROI(int)));
    connect(ui->skip_spin_x1, SIGNAL(valueChanged(int)),SLOT(ChangedROI(int)));
    connect(ui->skip_spin_y1, SIGNAL(valueChanged(int)),SLOT(ChangedROI(int)));
 //   connect(ui->skip_button_getdoselist,SIGNAL(clicked()),SLOT(LoadDoseList()));
    connect(ui->skip_button_getroi,SIGNAL(clicked()),SLOT(GetROI()));


}

int FindSkipListDialog::exec(std::list<std::string> &filelist)
{
    std::ostringstream msg;
    m_FileList=filelist;

    m_nMaxNumberProjections=filelist.size();

    ui->skip_spin_expnumproj->setMaximum(m_nMaxNumberProjections);
    ui->skip_spin_expnumproj->setMinimum(0);
    ui->skip_spin_expnumproj->setValue(m_nMaxNumberProjections);
    ChangedNumberOfProjections(m_nMaxNumberProjections);

    std::string fname=filelist.front();

    if (QFile::exists(QString::fromStdString(fname))) {
        kipl::base::TImage<float,2> img;
        kipl::io::ReadFITS(img,fname.c_str(),NULL);
        ui->skip_imageviewer->set_image(img.GetDataPtr(),img.Dims());
    }
    else {
        msg.str("");
        msg<<"File does not exist ("<<fname<<")";
        logger(kipl::logging::Logger::LogWarning,msg.str());
    }

    return QDialog::exec();
}

int FindSkipListDialog::exec(std::string path, std::string fmask, int first, int last)
{
//    m_nFirst    = first;
//    m_nLast     = last;
//    m_sFileMask = fmask;
//    m_sPath     = path;

    ImageLoader il;
    il.m_filemask=path+fmask;
    il.m_nFirst=first;
    il.m_nLast=last;
    il.m_nStep=1;
    std::list<ImageLoader> ll;
    ll.push_back(il);
    std::list<std::string> flist=BuildFileList(ll);

    return exec(flist);
}

void FindSkipListDialog::LoadDoseList()
{
    std::ostringstream msg;
    ImageReader reader;
    m_DoseData.clear();
    m_SortedDoses.clear();
    UpdateParameters();
    float fDose=0.0f;
    int i=0;
    try {
        for (auto it=m_FileList.begin(); it!=m_FileList.end(); it++,i++) {

                   fDose=reader.GetProjectionDose(*it,
                                        kipl::base::ImageFlipNone,
                                        kipl::base::ImageRotateNone,
                                        1.0f,
                                        m_nROI);

                   m_DoseData.append(QPoint(static_cast<float>(i),fDose));
                   m_SortedDoses.insert(std::make_pair(fDose,i));
        }
        ui->skip_plot->setCurveData(0,m_DoseData);
        if (m_SortedDoses.size()!=m_DoseData.size()) {
            msg.str("");
            msg<<"Dose data size missmatch. size(DosePlot)="<<m_DoseData.size()<<", size(SortedDoses)="<<m_SortedDoses.size();
            throw kipl::base::KiplException(msg.str(),__FILE__,__LINE__);
        }

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
        reject();
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

        reject();
    }

    ui->skip_plot->setCurveData(0,m_DoseData);

}

void FindSkipListDialog::ChangedNumberOfProjections(int x)
{
    std::ostringstream skipstr;
    int nSkipCnt=m_nMaxNumberProjections-x;
    ui->skip_plot->clearAllPlotCursors();
    std::multimap<float,int>::iterator it=m_SortedDoses.begin();


    for (int i=0; i<nSkipCnt; i++, it++) {
        m_sortedSkip.insert(it->second);
        ui->skip_plot->setPlotCursor(i,QtAddons::PlotCursor(it->second,QColor("green"),QtAddons::PlotCursor::Vertical));
    }
    // Todo: Sort the list
   std::set<int>::iterator it2;
    for (it2=m_sortedSkip.begin(); it2!=m_sortedSkip.end(); it2++) {
        skipstr<<*it2<<" ";
    }

    ui->skip_edit_skiplist->setText(QString::fromStdString(skipstr.str()));
}

std::list<int> FindSkipListDialog::getSkipList()
{
    std::list<int> slist;

    std::string skipstr = ui->skip_edit_skiplist->text().toStdString();
    kipl::strings::String2List(skipstr,slist);

    return slist;
}

void FindSkipListDialog::GetROI()
{
    QRect ROI = ui->skip_imageviewer->get_marked_roi();

    ui->skip_spin_x0->setValue(ROI.x());
    ui->skip_spin_y0->setValue(ROI.y());
    ui->skip_spin_x1->setValue(ROI.x()+ROI.width());
    ui->skip_spin_y1->setValue(ROI.y()+ROI.height());

    ui->skip_imageviewer->set_rectangle(ROI,QColor(Qt::yellow),0);
}

void FindSkipListDialog::ChangedROI(int x)
{


}

void FindSkipListDialog::UpdateParameters()
{
    m_nROI[0]=ui->skip_spin_x0->value();
    m_nROI[1]=ui->skip_spin_y0->value();
    m_nROI[2]=ui->skip_spin_x1->value();
    m_nROI[3]=ui->skip_spin_y1->value();
}




void FindSkipListDialog::on_skip_button_getdoselist_clicked()
{
    LoadDoseList();
}

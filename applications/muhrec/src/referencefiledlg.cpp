#include "referencefiledlg.h"
#include "ui_referencefiledlg.h"

#include <vector>
#include <map>
#include <string>

#include <QStringList>

#include <io/DirAnalyzer.h>

ReferenceFileDlg::ReferenceFileDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReferenceFileDlg)
{
    ui->setupUi(this);
}

ReferenceFileDlg::~ReferenceFileDlg()
{
    delete ui;
}

void ReferenceFileDlg::setPath(const QString &_path)
{
    kipl::io::DirAnalyzer da;
    path=_path;
    ui->label_path->setText(path);
    std::vector<std::string> filelist= da.GetDirList(path.toStdString());

    da.AnalyzeDirList(filelist,masklist);
    QStringList names;
    int pos;
    fillComboBox(ui->comboBox_openBeam);
    names<<"ob"<<"ff"<<"open"<<"flat"<<"nob"<<"xob"<<"nff"<<"xff";
    pos = suggestSelection(names);
    ui->comboBox_openBeam->setCurrentIndex(pos);

    fillComboBox(ui->comboBox_darkCurrent);
    names<<"dc"<<"dark"<<"di"<<"ndc"<<"ndi"<<"xdc"<<"xdi";
    pos = suggestSelection(names);
    ui->comboBox_darkCurrent->setCurrentIndex(pos);
}

bool ReferenceFileDlg::getOpenBeamMask(QString &mask,int &first, int &last)
{
    mask = ui->comboBox_openBeam->currentText();

    first = masklist[mask.toStdString()].first;
    last  = masklist[mask.toStdString()].second;

    mask=path+mask;
 //   qDebug("%s, %d, %d",qUtf8Printable(mask),first,last);

    return ui->checkBox_openBeam->isChecked();
}

bool ReferenceFileDlg::getDarkCurrentMask(QString &mask,int &first, int &last)
{
    mask=ui->comboBox_darkCurrent->currentText();

    first = masklist[mask.toStdString()].first;
    last  = masklist[mask.toStdString()].second;

    mask=path+mask;
  //  qDebug("%s, %d, %d",qUtf8Printable(mask),first,last);

    return ui->checkBox_darkCurrent->isChecked();
}

void ReferenceFileDlg::fillComboBox(QComboBox *cb)
{
    cb->clear();

    QStringList sl;
    for (auto it=masklist.begin(); it!=masklist.end(); ++it ) {
    //    qDebug("%s, %d, %d",it->first.c_str(),it->second.first,it->second.second);
        sl.append(QString::fromStdString(it->first));
    }

    cb->addItems(sl);
}

int ReferenceFileDlg::suggestSelection(QStringList &names)
{
    int i=0;
    for (auto it=masklist.begin(); it!=masklist.end(); ++it, ++i) {
        for (auto name : names) {

            if (it->first.substr(0,name.size())==name.toStdString())
                return i;
        }
    }

    return 0;
}

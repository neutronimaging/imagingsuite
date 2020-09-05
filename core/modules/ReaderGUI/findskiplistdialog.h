#ifndef FINDSKIPLISTDIALOG_H
#define FINDSKIPLISTDIALOG_H

#include "readergui_global.h"

#include <map>
#include <set>
#include <vector>
#include <string>

#include <QDialog>
#include <QString>
#include <string>
#include <QVector>
#include <QPointF>


#include <logging/logger.h>


namespace Ui {
class FindSkipListDialog;
}

class READERGUISHARED_EXPORT FindSkipListDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit FindSkipListDialog(QWidget *parent = 0);
    ~FindSkipListDialog();
    
    virtual int exec(std::string path, std::string fmask, int first, int last);
    virtual int exec(std::vector<std::string> &filelist);
    std::vector<int> getSkipList();
    QString getSkipListString();

protected slots:
    void ChangedNumberOfProjections(int x);
    void GetROI();
    void LoadDoseList();

private slots:
    void on_skip_button_getdoselist_clicked();

    void on_skip_spin_x0_valueChanged(int arg1);

    void on_skip_spin_y0_valueChanged(int arg1);

    void on_skip_spin_x1_valueChanged(int arg1);

    void on_skip_spin_y1_valueChanged(int arg1);

private:
    Ui::FindSkipListDialog *ui;
    void UpdateParameters();

    std::vector<std::string> m_FileList;
//    std::string m_sPath;
//    std::string m_sFileMask;
//    int m_nFirst;
//    int m_nLast;
    int m_nMaxNumberProjections;
    std::vector<size_t> m_nROI;
    QRect m_DoseROI;
    QVector<QPointF> m_DoseData;
    std::multimap<float,int > m_SortedDoses;
    std::set<int> m_sortedSkip;
};

#endif // FINDSKIPLISTDIALOG_H



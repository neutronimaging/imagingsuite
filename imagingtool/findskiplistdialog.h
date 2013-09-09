#ifndef FINDSKIPLISTDIALOG_H
#define FINDSKIPLISTDIALOG_H

#include <QDialog>
#include <QString>
#include <string>
#include <QVector>
#include <QPointF>
#include <map>

#include <logging/logger.h>


namespace Ui {
class FindSkipListDialog;
}

class FindSkipListDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit FindSkipListDialog(QWidget *parent = 0);
    ~FindSkipListDialog();
    
    int exec(std::string path, std::string fmask, int first, int last);
    QString getSkipList();

protected slots:
    void ChangedNumberOfProjections(int x);
    void GetROI();
    void ChangedROI(int x);
    void LoadDoseList();

private:
    Ui::FindSkipListDialog *ui;
    void UpdateParameters();

    std::string m_sPath;
    std::string m_sFileMask;
    int m_nFirst;
    int m_nLast;
    int m_nMaxNumberProjections;
    size_t m_nROI[4];
    QVector<QPointF> m_DoseData;
    std::multimap<float,int > m_SortedDoses;
};

#endif // FINDSKIPLISTDIALOG_H



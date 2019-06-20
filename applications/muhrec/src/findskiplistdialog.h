#ifndef FINDSKIPLISTDIALOG_H
#define FINDSKIPLISTDIALOG_H

#include <QDialog>
#include <ReconConfig.h>
#include <logging/logger.h>
#include <map>
#include <QString>

namespace Ui {
class FindSkipListDialog;
}

class FindSkipListDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit FindSkipListDialog(QWidget *parent = nullptr);
    ~FindSkipListDialog();
    
    int exec(ReconConfig &config);
    QString getSkipList();

protected:
    virtual int exec() { return QDialog::exec();}
    void LoadDoseList(ReconConfig &config);

private slots:
    void on_pushButton_updatePlot_clicked();

    void on_spinExpNumProj_valueChanged(int arg1);

private:
    Ui::FindSkipListDialog *ui;

    int m_nMaxNumberProjections;
    QVector<QPointF> m_DoseData;
    std::multimap<float,int > m_SortedDoses;
    std::set<int> m_sortedSkip;
};

#endif // FINDSKIPLISTDIALOG_H

#ifndef REFERENCEFILEDLG_H
#define REFERENCEFILEDLG_H

#include <map>
#include <string>
#include <QDialog>

class QComboBox;

namespace Ui {
class ReferenceFileDlg;
}

class ReferenceFileDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ReferenceFileDlg(QWidget *parent = 0);
    ~ReferenceFileDlg();

    void setPath(const QString &_path);
    bool getOpenBeamMask(QString &mask,int &first, int &last);
    bool getDarkCurrentMask(QString &mask, int &first, int &last);

private:
    void fillComboBox(QComboBox *cb);
    int suggestSelection(QStringList &names);

    Ui::ReferenceFileDlg *ui;
    QString path;

    std::map<std::string, std::pair<int,int>> masklist;
};

#endif // REFERENCEFILEDLG_H

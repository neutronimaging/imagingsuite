//
// This file is part of the i KIPL image processing tool by Anders Kaestner
// (c) 2008 Anders Kaestner
// Distribution is only allowed with the permission of the author.
//
// Revision information
// $Author$
// $Date$
// $Rev$
//
#ifndef CONFIGHISTORYDIALOG_H
#define CONFIGHISTORYDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QListWidgetItem>

#include <KiplProcessConfig.h>
#include <base/timage.h>

namespace Ui {
class ConfigHistoryDialog;
}

class ConfigHistoryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigHistoryDialog(QWidget *parent = 0);
    ~ConfigHistoryDialog();

    void setList(std::list<std::pair<KiplProcessConfig, kipl::base::TImage<float,2> > >  &configList);
    KiplProcessConfig getSelectedConfig() { return m_SelectedConfig; }
private slots:
    void on_list_configurations_itemClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    QPixmap CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi);

    Ui::ConfigHistoryDialog *ui;
    std::list<std::pair<KiplProcessConfig, kipl::base::TImage<float,2> > > m_configList;
    KiplProcessConfig m_SelectedConfig;
};

#endif // CONFIGHISTORYDIALOG_H

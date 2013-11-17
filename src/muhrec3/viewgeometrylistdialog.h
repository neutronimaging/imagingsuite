#ifndef VIEWGEOMETRYLISTDIALOG_H
#define VIEWGEOMETRYLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <list>
#include <string>
#include <base/timage.h>
#include <ReconConfig.h>


namespace Ui {
class ViewGeometryListDialog;
}

enum eChangedConfigFields {
    ConfigField_None = 0,
    ConfigField_Tilt = 1,
    ConfigField_ROI  = 2,

};

class ViewGeometryListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ViewGeometryListDialog(QWidget *parent = 0);
    ~ViewGeometryListDialog();

    void setList(std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >  &reconList);
    eChangedConfigFields changedConfigFields();
    
    // Collection of call-backs
protected slots:
    void ClearAllChecked();
    void ShowSelected(QListWidgetItem *current,QListWidgetItem *previous);
    void ComputeTilt();
    void MaxROI();

private:
    void SetupCallbacks();
    QPixmap CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi);
    Ui::ViewGeometryListDialog *ui;
    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > > m_reconList;
    eChangedConfigFields m_eChangeConfigFields;
};

#endif // VIEWGEOMETRYLISTDIALOG_H

#ifndef VIEWGEOMETRYLISTDIALOG_H
#define VIEWGEOMETRYLISTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <list>
#include <string>

#include <base/timage.h>
#include <logging/logger.h>

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
    kipl::logging::Logger logger;
public:
    explicit ViewGeometryListDialog(QWidget *parent = 0);
    ~ViewGeometryListDialog();

    void setList(std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > >  &reconList);
    int changedConfigFields();
    void getTilt(float &center, float &tilt, float &pivot);
    void getROI(size_t *roi);
    
    // Collection of call-backs
protected slots:
    void ClearAllChecked();
    void ShowSelected(QListWidgetItem *current);
    void ComputeTilt();
    void MaxROI();

private slots:

    void on_buttonSelectAll_clicked();

private:
    void SetupCallbacks();
    QPixmap CreateIconFromImage(kipl::base::TImage<float,2> &img, float lo, float hi);
    Ui::ViewGeometryListDialog *ui;
    std::list<std::pair<ReconConfig, kipl::base::TImage<float,2> > > m_reconList;
    int m_eChangeConfigFields;
    double m_fTilt;
    double m_fPivot;
    double m_fCenter;
    size_t m_nMatrixROI[4];
};

#endif // VIEWGEOMETRYLISTDIALOG_H

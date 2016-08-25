#ifndef IMAGEVIEWERINFODIALOG_H
#define IMAGEVIEWERINFODIALOG_H

#include <QtAddons_global.h>
#include <QDialog>
#include <QRect>
#include <QPoint>
#include <QVector>

#include <base/timage.h>
#include <logging/logger.h>

namespace Ui {
class ImageViewerInfoDialog;
}

class QTADDONSSHARED_EXPORT ImageViewerInfoDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ImageViewerInfoDialog(QWidget *parent = nullptr);
    ~ImageViewerInfoDialog();
    void updateInfo(kipl::base::TImage<float, 2> img, QRect roi);

    void setHistogram(const QVector<QPointF> &hist);

private slots:
    void on_check_showglobal_toggled(bool checked);

private:
    Ui::ImageViewerInfoDialog *ui;
    kipl::base::TImage<float,2> *m_CurrentImage;
    kipl::base::TImage<float,2> m_roiImage;
    QVector<QPointF> m_GlobalHistogram;
    QVector<QPointF> m_LocalHistogram;
};

#endif // IMAGEVIEWERINFODIALOG_H

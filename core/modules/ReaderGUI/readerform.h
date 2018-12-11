#ifndef READERFORM_H
#define READERFORM_H

#include "readergui_global.h"
#include <QWidget>

#include <logging/logger.h>
#include <datasetbase.h>
#include <imageviewerwidget.h>
#include <uxroiwidget.h>

namespace Ui {
class ReaderForm;
}

class READERGUISHARED_EXPORT ReaderForm : public QWidget
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ReaderForm(QWidget *parent = nullptr);
    ~ReaderForm();

    ImageLoader getReaderConfig();
    void setReaderConfig(ImageLoader &cfg);

    void setLabel(QString str);
    void showMinimalForm();
    void showFullForm();
    void setHideRepeat(bool x);
    void setHideStride(bool x);
    void setHideMirrorRotate(bool x);
    void setHideROI(bool x);

    /// \brief Registers a handle to the viewer which will be used for selection and visualization of ROIs.
    /// \param viewer A pointer to the viewer widget.
    void registerViewer(QtAddons::ImageViewerWidget *viewer);

    QtAddons::uxROIWidget * roiWidget();
private slots:
    void on_button_browse_clicked();

private:
    Ui::ReaderForm *ui;
};

#endif // READERFORM_H

#ifndef READERFORM_H
#define READERFORM_H

#include "readergui_global.h"
#include <QWidget>

#include <logging/logger.h>
#include <datasetbase.h>

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
    void showMirrorRotate(bool x);
    void showROI(bool x);

private slots:
    void on_button_browse_clicked();

private:
    Ui::ReaderForm *ui;
};

#endif // READERFORM_H

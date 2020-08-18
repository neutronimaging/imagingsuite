//<LICENSE>

#ifndef LOADIMAGEDIALOG_H
#define LOADIMAGEDIALOG_H

#include <QDialog>
#include <KiplProcessConfig.h>
#include <base/timage.h>
#include <interactors/interactionbase.h>
#include <logging/logger.h>

namespace Ui {
class LoadImageDialog;
}

class LoadImageDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit LoadImageDialog(kipl::interactors::InteractionBase *interactor, QWidget *parent = nullptr);
    ~LoadImageDialog();

    int progress();
    int process();
    virtual int exec(KiplProcessConfig *config, kipl::base::TImage<float,3> *img);
private:
    Ui::LoadImageDialog *ui;

    virtual int exec() { return QDialog::exec(); }
protected:
    void Abort();
    bool Finished();

    float fraction;
    bool finish;

    KiplProcessConfig *mConfig;
    kipl::base::TImage<float,3> * m_img;
    kipl::interactors::InteractionBase * m_Interactor;

private slots:
    void on_processFailure(QString msg);
    void changedProgress(float progress, float overallProgress, QString msg);

    void on_buttonBox_rejected();

signals:
    void updateProgress(float progress, float overallProgress, QString msg);
    void processFailure(QString msg);
};

#endif // LOADIMAGEDIALOG_H

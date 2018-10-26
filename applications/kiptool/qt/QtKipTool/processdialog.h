#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>

#include <logging/logger.h>
#include <base/timage.h>
#include <KiplEngine.h>
#include <interactors/interactionbase.h>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ProcessDialog(kipl::interactors::InteractionBase *interactor,QWidget *parent = nullptr);
    ~ProcessDialog();
    int progress();
    int process();
    virtual int exec(KiplEngine *engine, kipl::base::TImage<float, 3> *img);

private:
    Ui::ProcessDialog *ui;

    virtual int exec() { return QDialog::exec(); }
protected:
    void Abort();
    bool Finished();

    float fraction;
    bool finish;
    KiplEngine * m_Engine;
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

#endif // PROCESSDIALOG_H

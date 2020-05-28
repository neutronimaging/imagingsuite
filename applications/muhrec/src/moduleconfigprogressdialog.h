#ifndef MODULECONFIGPROGRESSDIALOG_H
#define MODULECONFIGPROGRESSDIALOG_H

#include <QDialog>
#include <ReconEngine.h>
#include <logging/logger.h>
#include <interactors/interactionbase.h>

namespace Ui {
class ModuleConfigProgressDialog;
}

class ModuleConfigProgressDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;
public:
    explicit ModuleConfigProgressDialog(kipl::interactors::InteractionBase *interactor,QWidget *parent = nullptr);
    ~ModuleConfigProgressDialog();
    int progress();
    int process();
    int exec(ReconEngine *engine, const std::vector<size_t> &roi, string lastModule);
    kipl::base::TImage<float,3> getImage();

private:
    Ui::ModuleConfigProgressDialog *ui;
    virtual int exec() { return QDialog::exec(); }

protected:
    void Abort();
    bool Finished();

    float fraction;
    bool finish;
    std::vector<size_t> mROI;
    std::string mLastModule;
    ReconEngine * m_Engine;
    kipl::interactors::InteractionBase * m_Interactor;
    kipl::base::TImage<float,3> m_Image;


private slots:
    void on_processFailure(QString msg);
    void changedProgress(float progress, float overallProgress, QString msg);

    void on_buttonBox_rejected();

signals:
    void updateProgress(float progress, float overallProgress, QString msg);
    void processFailure(QString msg);
};

#endif // MODULECONFIGPROGRESSDIALOG_H

#ifndef RECONDIALOG_H
#define RECONDIALOG_H

#include <QDialog>
#include <ReconEngine.h>
#include <logging/logger.h>
#include <interactors/interactionbase.h>

namespace Ui {
class ReconDialog;
}

class ReconDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit ReconDialog(kipl::interactors::InteractionBase *interactor, QWidget *parent = 0);
    ~ReconDialog();
    int progress();
    int process();
    virtual int exec(ReconEngine *engine, bool bRerunBackProj);

private:
    Ui::ReconDialog *ui;

    virtual int exec() { return QDialog::exec(); }
protected:
    void Abort();
    bool Finished();

    float fraction;
    bool finish;
    ReconEngine * m_Engine;
    kipl::interactors::InteractionBase * m_Interactor;
    bool m_bRerunBackproj;
private slots:
    void on_buttonCancel_clicked();
};

#endif // RECONDIALOG_H

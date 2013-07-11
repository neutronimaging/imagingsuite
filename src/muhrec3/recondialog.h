#ifndef RECONDIALOG_H
#define RECONDIALOG_H

#include <QDialog>
#include <ReconEngine.h>
#include <logging/logger.h>
#include <InteractionBase.h>

namespace Ui {
class ReconDialog;
}

class ReconDialog : public QDialog
{
    Q_OBJECT
    kipl::logging::Logger logger;

public:
    explicit ReconDialog(InteractionBase *interactor, QWidget *parent = 0);
    ~ReconDialog();
    void progress();
    void process();
    virtual int exec(ReconEngine *engine);

protected slots:
    void pressedCancel();

private:
    Ui::ReconDialog *ui;

    virtual int exec() { return this->exec(); }
protected:
    void Abort();
    bool Finished();

    float fraction;
    bool finish;
    ReconEngine * m_Engine;
    InteractionBase * m_Interactor;
};

#endif // RECONDIALOG_H

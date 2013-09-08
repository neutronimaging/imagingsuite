#ifndef FINDSKIPLISTDIALOG_H
#define FINDSKIPLISTDIALOG_H

#include <QDialog>

namespace Ui {
class FindSkipListDialog;
}

class FindSkipListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FindSkipListDialog(QWidget *parent = 0);
    ~FindSkipListDialog();
    
private:
    Ui::FindSkipListDialog *ui;
};

#endif // FINDSKIPLISTDIALOG_H

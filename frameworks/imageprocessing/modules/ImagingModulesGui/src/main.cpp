#include "bblognormdlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bblognormDlg w;
    w.show();

    return a.exec();
}

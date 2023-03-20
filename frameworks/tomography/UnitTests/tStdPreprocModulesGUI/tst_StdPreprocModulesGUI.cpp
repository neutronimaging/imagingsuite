//<LICENSE>

#include <algorithm>

#include <QString>
#include <QtTest>
#include <QDebug>

#include <MorphSpotCleanModule.h>
#include <ModuleException.h>
#include <strings/filenames.h>
#include <io/io_tiff.h>
#include <base/timage.h>
#include <StdPreprocModulesGUI.h>
#include <bblognormdlg.h>
#include <bblognorm.h>

class TStdPreprocModulesGUI : public QObject
{
    Q_OBJECT

public:
    TStdPreprocModulesGUI();

private Q_SLOTS:
    void testBBLogNormDlg();

};

TStdPreprocModulesGUI::TStdPreprocModulesGUI()
{

}

void TStdPreprocModulesGUI::testBBLogNormDlg()
{
    BBLogNormDlg dlg;
    BBLogNorm mod;

    auto modpars = mod.GetParameters();
    auto dlgpars = dlg.parameters();

    for (auto & par : modpars) 
    {
        QVERIFY2(dlgpars.count(par.first)!=0,par.first.c_str());
    }
}

QTEST_MAIN(TStdPreprocModulesGUI)

#include "tst_StdPreprocModulesGUI.moc"

#include <iostream>
#include <vector>

#include <QString>
#include <QtTest>

#include <base/timage.h>
#include <io/io_tiff.h>
#include <base/KiplException.h>
#include <kmeans.h>

class TSegmentation : public QObject
{
    Q_OBJECT

public:
    TSegmentation();

private Q_SLOTS:
    void kmeans_ctor();

};

TSegmentation::TSegmentation()
{
}

void TSegmentation::kmeans_ctor()
{

}



QTEST_APPLESS_MAIN(TSegmentation)

#include "tst_segmentation.moc"

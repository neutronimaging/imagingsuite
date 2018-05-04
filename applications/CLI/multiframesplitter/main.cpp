#include <iostream>
#include <sstream>

#include <QCoreApplication>
#include <QVector>
#include <QString>

#include <analyzefileext.h>
#include <base/timage.h>
#include <base/KiplException.h>
#include <logging/logger.h>
#include <strings/filenames.h>

#include <imagereader.h>
#include <imagewriter.h>

kipl::logging::Logger logger("MultiFrameSplitter");

int process(QCoreApplication &a);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    process(a);

    return a.exec();
}

int process(QCoreApplication &a)
{
    std::ostringstream msg;
    QVector<QString> qargs=a.arguments().toVector();

    int cnt=qargs.size();
    if (cnt==1) {
        logger(logger.LogMessage,"No arguments");
        return 0;
    }

    size_t dims[10];
    int nDims=0;
    ImageReader reader;

    std::string srcfname=qargs[1].toStdString();
    nDims=reader.GetImageSize(srcfname,1.0f,dims);

    if (nDims<3) {
        logger(logger.LogWarning,"This is a single frame image");
        return 0;
    }

    cnt=dims[2];

    msg.str("");
    msg<<srcfname<<" has "<<cnt<<" slices";
    logger(logger.LogMessage,msg.str());

    std::string path;
    std::string fname;
    std::vector<std::string> exts;

    kipl::strings::filenames::StripFileName(srcfname,path,fname,exts);

    std::string fmask=qargs[2].toStdString();
    int first = 0 ;
    int last  = cnt-1;

    std::string destname,ext;
    ImageWriter writer;
    kipl::base::TImage<float,2> img;
    for (int i=first; i<=last; ++i) {
        reader.Read(srcfname,kipl::base::ImageFlipNone,kipl::base::ImageRotateNone,1.0,nullptr,i);
        kipl::strings::filenames::MakeFileName(fmask,i,destname,ext,'#','0');
        writer.write(img,destname);
    }

    return cnt;
}
